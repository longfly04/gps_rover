#ifndef OSMTILELAYER_H
#define OSMTILELAYER_H

#include <QCache>
#include <QObject>
#include <QPixmap>
#include <QPointF>
#include <QSet>
#include <QSize>
#include <QString>
#include <QVector>

class QNetworkAccessManager;
class QPainter;

class OsmTileLayer : public QObject
{
    Q_OBJECT

public:
    struct ViewState {
        QSize viewportSize;
        double centerWorldX;
        double centerWorldY;
        double zoomFactor;
        double rotationRadians;
    };

    explicit OsmTileLayer(QObject *parent = nullptr);
    ~OsmTileLayer() override;

    bool draw(QPainter &painter, const ViewState &viewState);
    QString attributionText() const;

    static QPointF geographicToWorld(double lat, double lon);
    static QPointF worldToGeographic(double worldX, double worldY);

signals:
    void repaintRequested();

private:
    const QPixmap* loadTile(int zoom, int x, int y);
    void scheduleVisibleTiles(const QVector<QString> &orderedTileKeys);
    void processRequestQueue();
    void startTileRequest(const QString &key);
    bool parseTileKey(const QString &key, int &zoom, int &x, int &y) const;
    QString tileKey(int zoom, int x, int y) const;
    QString tileFilePath(int zoom, int x, int y) const;
    QString resolveProjectCacheDirectory() const;
    QPointF worldToScreen(const ViewState &viewState, double worldX, double worldY) const;
    int chooseTileZoom(const ViewState &viewState) const;

private:
    QNetworkAccessManager* m_networkManager;
    QCache<QString, QPixmap> m_memoryCache;
    QSet<QString> m_pendingTileKeys;
    QSet<QString> m_queuedTileKeys;
    QVector<QString> m_requestQueue;
    QString m_tileUrlTemplate;
    QString m_attributionText;
    QString m_cacheDirectory;
    int m_activeRequestCount;
};

#endif // OSMTILELAYER_H
