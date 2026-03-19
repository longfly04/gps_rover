#include "osmtilelayer.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPainter>
#include <QPolygonF>
#include <QTransform>
#include <QUrl>

#include <algorithm>
#include <cmath>
#include <utility>
#include <QStringList>

#include "../core/common/logger.h"

namespace {
constexpr double kPi = 3.14159265358979323846;
constexpr double kEarthRadiusMeters = 6378137.0;
constexpr double kMercatorMaxLatitude = 85.05112878;
constexpr double kHalfWorldExtent = kEarthRadiusMeters * kPi;
constexpr int kTileSize = 256;
constexpr int kMinTileZoom = 0;
constexpr int kMaxRequestZoom = 18;
constexpr int kMemoryCacheCost = 256;
constexpr int kMaxConcurrentRequests = 4;
constexpr int kMaxQueuedRequests = 48;
constexpr char kTileRequestUserAgent[] = "GPSSeeder/0.1 (Qt Widgets satellite basemap)";
constexpr char kTileRequestReferrer[] = "https://www.arcgis.com/";

int wrapTileX(int tileX, int tileCount)
{
    if (tileCount <= 0) {
        return tileX;
    }

    const int wrapped = tileX % tileCount;
    return wrapped < 0 ? wrapped + tileCount : wrapped;
}

QPointF rotateWorldOffset(double dx, double dy, double rotationRadians)
{
    if (std::abs(rotationRadians) < 1e-12) {
        return QPointF(dx, dy);
    }

    const double cosTheta = std::cos(rotationRadians);
    const double sinTheta = std::sin(rotationRadians);
    return QPointF(dx * cosTheta - dy * sinTheta,
                   dx * sinTheta + dy * cosTheta);
}
}

OsmTileLayer::OsmTileLayer(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_memoryCache(kMemoryCacheCost)
    , m_pendingTileKeys()
    , m_queuedTileKeys()
    , m_requestQueue()
    , m_tileUrlTemplate(QStringLiteral("https://server.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/%1/%3/%2"))
    , m_attributionText(QStringLiteral("Imagery © Esri, Maxar, Earthstar Geographics"))
    , m_cacheDirectory(resolveProjectCacheDirectory())
    , m_activeRequestCount(0)
{
    QDir().mkpath(m_cacheDirectory);
    LOG_INFO("Basemap provider: Esri World Imagery");
    LOG_INFO("Basemap tile cache directory: %s", m_cacheDirectory.toStdString().c_str());
    LOG_INFO("Basemap tile cache mode: local-first persistent storage");
}

OsmTileLayer::~OsmTileLayer() = default;

bool OsmTileLayer::draw(QPainter &painter, const ViewState &viewState)
{
    if (viewState.viewportSize.width() <= 0 || viewState.viewportSize.height() <= 0 || viewState.zoomFactor <= 0.0) {
        return false;
    }

    const int zoom = chooseTileZoom(viewState);
    const int tileCount = 1 << zoom;
    const double tileWorldSize = (2.0 * kHalfWorldExtent) / static_cast<double>(tileCount);
    const double halfDiagonalWorld = std::hypot(static_cast<double>(viewState.viewportSize.width()),
                                                static_cast<double>(viewState.viewportSize.height()))
                                   / (2.0 * viewState.zoomFactor);

    const double minWorldX = viewState.centerWorldX - halfDiagonalWorld;
    const double maxWorldX = viewState.centerWorldX + halfDiagonalWorld;
    const double minWorldY = viewState.centerWorldY - halfDiagonalWorld;
    const double maxWorldY = viewState.centerWorldY + halfDiagonalWorld;

    const int xStart = static_cast<int>(std::floor((minWorldX + kHalfWorldExtent) / tileWorldSize)) - 1;
    const int xEnd = static_cast<int>(std::floor((maxWorldX + kHalfWorldExtent) / tileWorldSize)) + 1;
    const int yStart = std::max(0, static_cast<int>(std::floor((kHalfWorldExtent - maxWorldY) / tileWorldSize)) - 1);
    const int yEnd = std::min(tileCount - 1, static_cast<int>(std::floor((kHalfWorldExtent - minWorldY) / tileWorldSize)) + 1);

    if (yStart > yEnd) {
        return false;
    }

    bool drewTile = false;
    QVector<std::pair<QString, double>> missingTiles;
    missingTiles.reserve(std::max(0, (xEnd - xStart + 1) * (yEnd - yStart + 1)));

    painter.save();

    for (int unwrappedTileX = xStart; unwrappedTileX <= xEnd; ++unwrappedTileX) {
        const int wrappedTileX = wrapTileX(unwrappedTileX, tileCount);

        for (int tileY = yStart; tileY <= yEnd; ++tileY) {
            const QPixmap* tilePixmap = loadTile(zoom, wrappedTileX, tileY);
            if (!tilePixmap) {
                const double tileCenterWorldX = -kHalfWorldExtent + (static_cast<double>(unwrappedTileX) + 0.5) * tileWorldSize;
                const double tileCenterWorldY = kHalfWorldExtent - (static_cast<double>(tileY) + 0.5) * tileWorldSize;
                const QPointF delta = rotateWorldOffset(tileCenterWorldX - viewState.centerWorldX,
                                                        tileCenterWorldY - viewState.centerWorldY,
                                                        viewState.rotationRadians);
                missingTiles.push_back({tileKey(zoom, wrappedTileX, tileY), std::hypot(delta.x(), delta.y())});
                continue;
            }

            const double west = -kHalfWorldExtent + static_cast<double>(unwrappedTileX) * tileWorldSize;
            const double east = west + tileWorldSize;
            const double north = kHalfWorldExtent - static_cast<double>(tileY) * tileWorldSize;
            const double south = north - tileWorldSize;

            const QPointF topLeft = worldToScreen(viewState, west, north);
            const QPointF topRight = worldToScreen(viewState, east, north);
            const QPointF bottomRight = worldToScreen(viewState, east, south);
            const QPointF bottomLeft = worldToScreen(viewState, west, south);

            QTransform tileTransform;
            const QPolygonF sourceRect(QRectF(0.0, 0.0, tilePixmap->width(), tilePixmap->height()));
            const QPolygonF targetQuad{topLeft, topRight, bottomRight, bottomLeft};
            if (QTransform::quadToQuad(sourceRect, targetQuad, tileTransform)) {
                painter.save();
                painter.setTransform(tileTransform, true);
                painter.drawPixmap(0, 0, *tilePixmap);
                painter.restore();
                drewTile = true;
            }
        }
    }

    painter.restore();

    if (!missingTiles.isEmpty()) {
        std::sort(missingTiles.begin(), missingTiles.end(), [](const auto &lhs, const auto &rhs) {
            if (std::abs(lhs.second - rhs.second) > 1e-6) {
                return lhs.second < rhs.second;
            }
            return lhs.first < rhs.first;
        });

        QVector<QString> orderedTileKeys;
        orderedTileKeys.reserve(missingTiles.size());
        for (const auto &entry : missingTiles) {
            orderedTileKeys.push_back(entry.first);
        }
        scheduleVisibleTiles(orderedTileKeys);
    }

    return drewTile;
}

QString OsmTileLayer::attributionText() const
{
    return m_attributionText;
}

QPointF OsmTileLayer::geographicToWorld(double lat, double lon)
{
    const double clampedLat = std::clamp(lat, -kMercatorMaxLatitude, kMercatorMaxLatitude);
    const double latRad = clampedLat * kPi / 180.0;
    const double lonRad = lon * kPi / 180.0;

    const double x = kEarthRadiusMeters * lonRad;
    const double y = kEarthRadiusMeters * std::log(std::tan(kPi / 4.0 + latRad / 2.0));
    return QPointF(x, std::clamp(y, -kHalfWorldExtent, kHalfWorldExtent));
}

QPointF OsmTileLayer::worldToGeographic(double worldX, double worldY)
{
    const double clampedWorldY = std::clamp(worldY, -kHalfWorldExtent, kHalfWorldExtent);
    const double lat = (2.0 * std::atan(std::exp(clampedWorldY / kEarthRadiusMeters)) - kPi / 2.0) * 180.0 / kPi;

    double lon = worldX / kEarthRadiusMeters * 180.0 / kPi;
    double wrappedLon = std::fmod(lon + 180.0, 360.0);
    if (wrappedLon < 0.0) {
        wrappedLon += 360.0;
    }
    lon = wrappedLon - 180.0;

    return QPointF(lat, lon);
}

const QPixmap* OsmTileLayer::loadTile(int zoom, int x, int y)
{
    const QString key = tileKey(zoom, x, y);
    if (const QPixmap* cachedPixmap = m_memoryCache.object(key)) {
        return cachedPixmap;
    }

    const QString filePath = tileFilePath(zoom, x, y);
    if (!QFileInfo::exists(filePath)) {
        return nullptr;
    }

    QPixmap* tilePixmap = new QPixmap();
    if (!tilePixmap->load(filePath)) {
        delete tilePixmap;
        LOG_WARN("Basemap tilelocal decode failed: %s", filePath.toStdString().c_str());
        QFile::remove(filePath);
        return nullptr;
    }

    m_memoryCache.insert(key, tilePixmap, 1);
    LOG_INFO("Basemap tilelocal hit: %s", key.toStdString().c_str());
    return m_memoryCache.object(key);
}

void OsmTileLayer::scheduleVisibleTiles(const QVector<QString> &orderedTileKeys)
{
    int availableSlots = std::max(0, kMaxQueuedRequests - static_cast<int>(m_requestQueue.size()));
    int droppedCount = 0;

    for (const QString &key : orderedTileKeys) {
        if (availableSlots <= 0) {
            ++droppedCount;
            continue;
        }

        if (m_memoryCache.contains(key) || m_pendingTileKeys.contains(key) || m_queuedTileKeys.contains(key)) {
            continue;
        }

        m_requestQueue.push_back(key);
        m_queuedTileKeys.insert(key);
        --availableSlots;
        LOG_INFO("Basemap tilequeued: %s", key.toStdString().c_str());
    }

    if (droppedCount > 0) {
        LOG_INFO("Basemap tilequeue saturated: dropped=%d active=%d queued=%d",
                 droppedCount,
                 m_activeRequestCount,
                 static_cast<int>(m_requestQueue.size()));
    }

    processRequestQueue();
}

void OsmTileLayer::processRequestQueue()
{
    while (m_activeRequestCount < kMaxConcurrentRequests && !m_requestQueue.isEmpty()) {
        const QString key = m_requestQueue.front();
        m_requestQueue.removeAt(0);
        m_queuedTileKeys.remove(key);
        startTileRequest(key);
    }
}

void OsmTileLayer::startTileRequest(const QString &key)
{
    if (m_memoryCache.contains(key) || m_pendingTileKeys.contains(key)) {
        return;
    }

    int zoom = 0;
    int x = 0;
    int y = 0;
    if (!parseTileKey(key, zoom, x, y)) {
        LOG_WARN("Basemap tilekey parse failed: %s", key.toStdString().c_str());
        return;
    }

    m_pendingTileKeys.insert(key);
    ++m_activeRequestCount;
    LOG_INFO("Basemap tilelocal miss: %s", key.toStdString().c_str());

    const QString url = m_tileUrlTemplate.arg(zoom).arg(x).arg(y);
    LOG_INFO("Basemap tilerequest: %s", url.toStdString().c_str());

    QNetworkRequest networkRequest{QUrl(url)};
    networkRequest.setHeader(QNetworkRequest::UserAgentHeader, QString::fromLatin1(kTileRequestUserAgent));
    networkRequest.setRawHeader("Referer", QByteArray(kTileRequestReferrer));
    networkRequest.setRawHeader("Accept", "image/png,image/*;q=0.9,*/*;q=0.1");
    networkRequest.setTransferTimeout(15000);
    networkRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    networkRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);

    QNetworkReply* reply = m_networkManager->get(networkRequest);
    connect(reply, &QNetworkReply::finished, this, [this, reply, key, zoom, x, y]() {
        m_pendingTileKeys.remove(key);
        m_activeRequestCount = std::max(0, m_activeRequestCount - 1);

        const int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (reply->error() != QNetworkReply::NoError) {
            LOG_WARN("Basemap tiledownload failed: %s, http=%d, error=%s",
                     key.toStdString().c_str(),
                     httpStatus,
                     reply->errorString().toStdString().c_str());
            reply->deleteLater();
            processRequestQueue();
            return;
        }

        const QByteArray payload = reply->readAll();
        QPixmap* tilePixmap = new QPixmap();
        if (payload.isEmpty() || !tilePixmap->loadFromData(payload)) {
            delete tilePixmap;
            LOG_WARN("Basemap tiledecode failed after download: %s", key.toStdString().c_str());
            reply->deleteLater();
            processRequestQueue();
            return;
        }

        const QString filePath = tileFilePath(zoom, x, y);
        const QFileInfo fileInfo(filePath);
        if (!QDir().mkpath(fileInfo.dir().absolutePath())) {
            LOG_WARN("Basemap tilecache directory create failed: %s", fileInfo.dir().absolutePath().toStdString().c_str());
        } else {
            QFile file(filePath);
            if (!file.open(QIODevice::WriteOnly) || file.write(payload) != payload.size()) {
                LOG_WARN("Basemap tilesave failed: %s", filePath.toStdString().c_str());
            }
        }

        m_memoryCache.insert(key, tilePixmap, 1);
        emit repaintRequested();
        reply->deleteLater();
        processRequestQueue();
    });
}

bool OsmTileLayer::parseTileKey(const QString &key, int &zoom, int &x, int &y) const
{
    const QStringList parts = key.split('/');
    if (parts.size() != 3) {
        return false;
    }

    bool okZoom = false;
    bool okX = false;
    bool okY = false;
    zoom = parts[0].toInt(&okZoom);
    x = parts[1].toInt(&okX);
    y = parts[2].toInt(&okY);
    return okZoom && okX && okY;
}

QString OsmTileLayer::tileKey(int zoom, int x, int y) const
{
    return QStringLiteral("%1/%2/%3").arg(zoom).arg(x).arg(y);
}

QString OsmTileLayer::tileFilePath(int zoom, int x, int y) const
{
    return QDir(m_cacheDirectory).filePath(tileKey(zoom, x, y) + QStringLiteral(".png"));
}

QString OsmTileLayer::resolveProjectCacheDirectory() const
{
    QDir dir(QCoreApplication::applicationDirPath());
    for (int depth = 0; depth < 8; ++depth) {
        if (dir.exists(QStringLiteral("CMakeLists.txt"))
            || dir.exists(QStringLiteral("CLAUDE.md"))
            || dir.exists(QStringLiteral(".git"))) {
            return dir.filePath(QStringLiteral("resource/map/satellite"));
        }

        if (!dir.cdUp()) {
            break;
        }
    }

    return QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("resource/map"));
}

QPointF OsmTileLayer::worldToScreen(const ViewState &viewState, double worldX, double worldY) const
{
    const QPointF center(viewState.viewportSize.width() / 2.0, viewState.viewportSize.height() / 2.0);
    const QPointF worldOffset = rotateWorldOffset(worldX - viewState.centerWorldX,
                                                  worldY - viewState.centerWorldY,
                                                  viewState.rotationRadians);
    const double screenX = center.x() + worldOffset.x() * viewState.zoomFactor;
    const double screenY = center.y() - worldOffset.y() * viewState.zoomFactor;
    return QPointF(screenX, screenY);
}

int OsmTileLayer::chooseTileZoom(const ViewState &viewState) const
{
    const double metersPerPixel = 1.0 / std::max(viewState.zoomFactor, 1e-9);
    const double zoomFloat = std::log2((2.0 * kHalfWorldExtent) / (static_cast<double>(kTileSize) * metersPerPixel));
    return std::clamp(static_cast<int>(std::round(zoomFloat)), kMinTileZoom, kMaxRequestZoom);
}
