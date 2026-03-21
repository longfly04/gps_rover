#include "statestimator.h"

#include <cmath>
#include <stdexcept>
#include <utility>

#include <QPointF>

#include "../core/common/logger.h"

namespace {
constexpr double kPi = 3.14159265358979323846;
constexpr double kCoarseEvalIntervalSec = 1.0;
constexpr double kFineLookaheadSec = 3.0;
constexpr double kMinVyForPrediction = 1e-4;
constexpr double kEpsilon = 1e-9;

static double normalizeHeadingDeg(double headingDeg)
{
    while (headingDeg < 0.0) {
        headingDeg += 360.0;
    }
    while (headingDeg >= 360.0) {
        headingDeg -= 360.0;
    }
    return headingDeg;
}
}

StateEstimator::StateEstimator()
    : m_coordinate(nullptr),
      m_useKalman(false),
      m_lowPassAlpha(0.2),
      m_lastTimestamp(0.0),
      m_lastLat(0.0),
      m_lastLon(0.0),
      m_lastSpeed(0.0),
      m_lastHeading(0.0),
      m_historyMaxSize(20),
      m_nextTriggerVectorIndex(0),
      m_triggerStopY(0.0),
      m_inFineMode(false),
      m_lastCoarseEvalTimestamp(0.0),
      m_lastTravelDirection(0),
      m_averageSpeed(0.0),
      m_averageDirectionDeg(0.0),
      m_averagePointSpacing(0.0),
      m_windowVx(0.0),
      m_windowVy(0.0),
      m_hasPrediction(false),
      m_predictedTriggerX(0.0),
      m_predictedTriggerY(0.0),
      m_predictedTriggerTimeUs(0),
      m_predictedCountdownSec(-1.0),
      m_predictedTriggerVectorIndex(-1),
      m_hasLastTriggerEvent(false),
      m_lastTriggerX(0.0),
      m_lastTriggerY(0.0),
      m_lastTriggerTimeUs(0),
      m_triggerSequence(0),
      m_deadzoneM(1.0)
{
    updateTriggerPoseFields();
    LOG_INFO("StateEstimator initialized with OA local coordinate system");
}

StateEstimator::~StateEstimator() = default;

void StateEstimator::setCoordinate(Coordinate* coordinate)
{
    m_coordinate = coordinate;
    LOG_INFO("Coordinate converter set for StateEstimator");
}

void StateEstimator::setUseKalman(bool useKalman)
{
    m_useKalman = useKalman;
    LOG_INFO("Setting useKalman to: %d", useKalman);
}

void StateEstimator::setLowPassAlpha(double alpha)
{
    m_lowPassAlpha = std::max(0.0, std::min(1.0, alpha));
    LOG_INFO("Setting lowPassAlpha to: %f", m_lowPassAlpha);
}

void StateEstimator::setTriggerPlan(const BlockPlanResult& plan)
{
    m_triggerLines = plan.triggerLines;
    m_triggerStopY = plan.triggerStopY;
    m_nextTriggerVectorIndex = m_triggerLines.empty() ? -1 : 0;
    m_inFineMode = false;
    m_lastCoarseEvalTimestamp = 0.0;
    m_lastTravelDirection = 0;
    clearTriggerPrediction();
    m_hasLastTriggerEvent = false;
    m_lastTriggerX = 0.0;
    m_lastTriggerY = 0.0;
    m_lastTriggerTimeUs = 0;
    m_triggerSequence = 0;
    updateTriggerPoseFields();

    if (!m_triggerLines.empty()) {
        LOG_INFO("Trigger plan configured: %d lines, startY=%.4f, stopY=%.4f",
                 static_cast<int>(m_triggerLines.size()),
                 m_triggerLines.front().y,
                 m_triggerStopY);
    } else {
        LOG_INFO("Trigger plan cleared");
    }
}

void StateEstimator::update(const SensorData& data)
{
    try {
        if (data.timestamp <= 0.0) {
            throw std::runtime_error("Invalid timestamp");
        }
        if (std::isnan(data.lat) || std::isnan(data.lon)) {
            throw std::runtime_error("Invalid position data");
        }
        if (!m_coordinate || !m_coordinate->isInitialized()) {
            LOG_WARN("Coordinate not initialized, skipping state update");
            return;
        }

        const QPointF localXY = m_coordinate->wgs84ToXY(data.lat, data.lon);
        const double x = localXY.x();
        const double y = localXY.y();

        m_buffer.push_back(LocalPoint{x, y, data.timestamp});
        if (m_buffer.size() > static_cast<size_t>(m_historyMaxSize)) {
            m_buffer.erase(m_buffer.begin());
        }

        double regressionVx = 0.0;
        double regressionVy = 0.0;
        estimateVelocity(regressionVx, regressionVy);
        updateWindowStats(regressionVx, regressionVy);

        if (std::abs(regressionVx) > kEpsilon || std::abs(regressionVy) > kEpsilon) {
            const double headingRad = std::atan2(regressionVx, regressionVy);
            m_currentPose.heading = normalizeHeadingDeg(headingRad * 180.0 / kPi);
        } else {
            m_currentPose.heading = normalizeHeadingDeg(data.heading);
        }
        m_currentPose.magneticHeading = normalizeHeadingDeg(data.heading);

        if (m_useKalman) {
            updateKalman(data, x, y);
        } else {
            updateLowPass(data, x, y);
        }

        m_currentPose.timestamp = data.timestamp;
        m_currentPose.latitude = data.lat;
        m_currentPose.longitude = data.lon;

        m_lastTimestamp = data.timestamp;
        m_lastLat = data.lat;
        m_lastLon = data.lon;
        m_lastSpeed = data.speed;
        m_lastHeading = data.heading;

        checkCrossing();
        updateTriggerPoseFields();
    } catch (const std::exception& e) {
        LOG_Error("Error updating state estimator: %s", e.what());
    }
}

void StateEstimator::updateLowPass(const SensorData& data, double x, double y)
{
    auto velocity = calculateVelocity(data);
    const double vx = velocity.first;
    const double vy = velocity.second;

    m_currentPose.x = m_lowPassAlpha * x + (1.0 - m_lowPassAlpha) * m_currentPose.x;
    m_currentPose.y = m_lowPassAlpha * y + (1.0 - m_lowPassAlpha) * m_currentPose.y;
    m_currentPose.vx = m_lowPassAlpha * vx + (1.0 - m_lowPassAlpha) * m_currentPose.vx;
    m_currentPose.vy = m_lowPassAlpha * vy + (1.0 - m_lowPassAlpha) * m_currentPose.vy;
}

void StateEstimator::updateKalman(const SensorData& data, double x, double y)
{
    auto velocity = calculateVelocity(data);
    const double vx = velocity.first;
    const double vy = velocity.second;
    const double kalmanGain = 0.1;

    m_currentPose.x = m_currentPose.x + kalmanGain * (x - m_currentPose.x);
    m_currentPose.y = m_currentPose.y + kalmanGain * (y - m_currentPose.y);
    m_currentPose.vx = m_currentPose.vx + kalmanGain * (vx - m_currentPose.vx);
    m_currentPose.vy = m_currentPose.vy + kalmanGain * (vy - m_currentPose.vy);
}

std::pair<double, double> StateEstimator::calculateVelocity(const SensorData& data)
{
    if (std::abs(m_windowVx) > kEpsilon || std::abs(m_windowVy) > kEpsilon) {
        return std::make_pair(m_windowVx, m_windowVy);
    }

    double vx = 0.0;
    double vy = 0.0;

    if (data.speed > 0.0) {
        const double headingRad = data.heading * kPi / 180.0;
        vx = data.speed * std::sin(headingRad);
        vy = data.speed * std::cos(headingRad);
    } else if (m_lastTimestamp > 0.0 && data.timestamp > m_lastTimestamp && m_buffer.size() >= 2) {
        const LocalPoint& p1 = m_buffer[m_buffer.size() - 2];
        const LocalPoint& p2 = m_buffer.back();
        const double dt = p2.timestamp - p1.timestamp;
        if (dt > kEpsilon) {
            vx = (p2.x - p1.x) / dt;
            vy = (p2.y - p1.y) / dt;
        }
    }

    return std::make_pair(vx, vy);
}

void StateEstimator::estimateVelocity(double& vx, double& vy)
{
    if (m_buffer.size() < 2) {
        vx = 0.0;
        vy = 0.0;
        return;
    }

    const int n = static_cast<int>(m_buffer.size());
    double sumT = 0.0;
    double sumX = 0.0;
    double sumY = 0.0;
    double sumTX = 0.0;
    double sumTY = 0.0;
    double sumT2 = 0.0;
    const double t0 = m_buffer.front().timestamp;

    for (const auto& p : m_buffer) {
        const double t = p.timestamp - t0;
        sumT += t;
        sumX += p.x;
        sumY += p.y;
        sumTX += t * p.x;
        sumTY += t * p.y;
        sumT2 += t * t;
    }

    const double denom = n * sumT2 - sumT * sumT;
    if (std::abs(denom) < kEpsilon) {
        vx = 0.0;
        vy = 0.0;
        return;
    }

    vx = (n * sumTX - sumT * sumX) / denom;
    vy = (n * sumTY - sumT * sumY) / denom;
}

void StateEstimator::updateWindowStats(double regressionVx, double regressionVy)
{
    m_windowVx = regressionVx;
    m_windowVy = regressionVy;
    m_averageSpeed = std::hypot(regressionVx, regressionVy);

    if (m_averageSpeed > kEpsilon) {
        m_averageDirectionDeg = normalizeHeadingDeg(std::atan2(regressionVx, regressionVy) * 180.0 / kPi);
    } else {
        m_averageDirectionDeg = 0.0;
    }

    if (m_buffer.size() < 2) {
        m_averagePointSpacing = 0.0;
        return;
    }

    double totalSpacing = 0.0;
    int spacingCount = 0;
    for (size_t i = 1; i < m_buffer.size(); ++i) {
        const double dx = m_buffer[i].x - m_buffer[i - 1].x;
        const double dy = m_buffer[i].y - m_buffer[i - 1].y;
        totalSpacing += std::hypot(dx, dy);
        ++spacingCount;
    }
    m_averagePointSpacing = spacingCount > 0 ? totalSpacing / static_cast<double>(spacingCount) : 0.0;
}

void StateEstimator::checkCrossing()
{
    if (m_buffer.empty()) {
        clearTriggerPrediction();
        m_inFineMode = false;
        return;
    }

    if (m_triggerLines.empty()) {
        clearTriggerPrediction();
        m_inFineMode = false;
        m_nextTriggerVectorIndex = -1;
        return;
    }

    const LocalPoint* previousPoint = m_buffer.size() >= 2 ? &m_buffer[m_buffer.size() - 2] : nullptr;
    const LocalPoint& currentPoint = m_buffer.back();

    int direction = inferTravelDirection(previousPoint, &currentPoint);
    const bool directionChanged = direction != 0 && direction != m_lastTravelDirection;
    if (direction != 0) {
        if (directionChanged) {
            LOG_INFO("Trigger traversal direction changed: %d -> %d at Y=%.4f",
                     m_lastTravelDirection,
                     direction,
                     currentPoint.y);
            clearTriggerPrediction();
            m_inFineMode = false;
        }
        m_lastTravelDirection = direction;
    } else {
        direction = m_lastTravelDirection;
    }

    if (direction == 0) {
        clearTriggerPrediction();
        m_inFineMode = false;
        return;
    }

    const double syncReferenceY = previousPoint ? previousPoint->y : currentPoint.y;
    if (directionChanged || !hasActiveTrigger()) {
        syncTriggerTraversal(syncReferenceY, direction);
    }

    advancePastCurrentPosition(syncReferenceY, direction, true);

    if (previousPoint) {
        while (hasActiveTrigger()) {
            const TriggerLineSpec* triggerLine = currentTriggerLine();
            if (!triggerLine) {
                break;
            }

            double triggerX = 0.0;
            qint64 triggerTimeUs = 0;
            if (!interpolateCrossing(*previousPoint, currentPoint, triggerLine->y, triggerX, triggerTimeUs)) {
                break;
            }

            if (m_predictedTriggerVectorIndex == m_nextTriggerVectorIndex) {
                clearTriggerPrediction();
            }

            commitTrigger(triggerX, triggerLine->y, triggerTimeUs, "gps-interpolation");
            m_nextTriggerVectorIndex += direction;
            m_inFineMode = false;
        }
    }

    advancePastCurrentPosition(currentPoint.y, direction, true);
    if (!hasActiveTrigger()) {
        clearTriggerPrediction();
        m_inFineMode = false;
        return;
    }

    const TriggerLineSpec* triggerLine = currentTriggerLine();
    if (!triggerLine) {
        clearTriggerPrediction();
        m_inFineMode = false;
        return;
    }

    const double distanceToLineY = static_cast<double>(direction) * (triggerLine->y - currentPoint.y);
    const double directionalVy = static_cast<double>(direction) * m_windowVy;

    if (m_averageSpeed <= kEpsilon || directionalVy <= kMinVyForPrediction) {
        m_inFineMode = false;
        clearTriggerPrediction();
        return;
    }

    const bool shouldEvaluateCoarse = !m_inFineMode
        && (m_lastCoarseEvalTimestamp <= 0.0
            || (currentPoint.timestamp - m_lastCoarseEvalTimestamp) >= kCoarseEvalIntervalSec);
    if (shouldEvaluateCoarse) {
        m_lastCoarseEvalTimestamp = currentPoint.timestamp;
        if (distanceToLineY >= -kEpsilon && distanceToLineY <= m_averageSpeed * kFineLookaheadSec) {
            m_inFineMode = true;
            LOG_INFO("Entering fine trigger mode: line=%d, distanceY=%.4f, avgSpeed=%.4f, direction=%d, vy=%.4f",
                     triggerLine->lineIndex,
                     distanceToLineY,
                     m_averageSpeed,
                     direction,
                     directionalVy);
        }
    }

    if (!m_inFineMode) {
        clearTriggerPrediction();
        return;
    }

    const double countdownSec = distanceToLineY / directionalVy;
    if (!std::isfinite(countdownSec)) {
        clearTriggerPrediction();
        return;
    }

    m_hasPrediction = true;
    m_predictedCountdownSec = countdownSec;
    m_predictedTriggerX = currentPoint.x + m_windowVx * countdownSec;
    m_predictedTriggerY = triggerLine->y;
    m_predictedTriggerTimeUs = static_cast<qint64>(std::llround((currentPoint.timestamp + countdownSec) * 1e6));
    m_predictedTriggerVectorIndex = m_nextTriggerVectorIndex;

    LOG_DEBUG("Fine trigger refresh: line=%d, countdown=%.6f s, predicted=(%.4f, %.4f), direction=%d, timeUs=%lld",
              triggerLine->lineIndex,
              m_predictedCountdownSec,
              m_predictedTriggerX,
              m_predictedTriggerY,
              direction,
              static_cast<long long>(m_predictedTriggerTimeUs));

    if (countdownSec <= 0.0) {
        if (commitPredictedTrigger()) {
            advancePastCurrentPosition(currentPoint.y, direction, false);
        }
    }
}

void StateEstimator::updateTriggerPoseFields()
{
    const TriggerLineSpec* triggerLine = currentTriggerLine();
    m_currentPose.hasNextTrigger = triggerLine != nullptr;
    m_currentPose.nextTriggerIndex = triggerLine ? triggerLine->lineIndex : 0;
    m_currentPose.nextTriggerLineY = triggerLine ? triggerLine->y : 0.0;

    if (m_hasPrediction && m_predictedTriggerVectorIndex == m_nextTriggerVectorIndex) {
        m_currentPose.nextTriggerCountdownSec = m_predictedCountdownSec;
        m_currentPose.predictedTriggerX = m_predictedTriggerX;
        m_currentPose.predictedTriggerY = m_predictedTriggerY;
        m_currentPose.predictedTriggerTimeUs = m_predictedTriggerTimeUs;
    } else {
        m_currentPose.nextTriggerCountdownSec = -1.0;
        m_currentPose.predictedTriggerX = 0.0;
        m_currentPose.predictedTriggerY = 0.0;
        m_currentPose.predictedTriggerTimeUs = 0;
    }

    m_currentPose.hasLastTriggerEvent = m_hasLastTriggerEvent;
    m_currentPose.lastTriggerX = m_lastTriggerX;
    m_currentPose.lastTriggerY = m_lastTriggerY;
    m_currentPose.lastTriggerTimeUs = m_lastTriggerTimeUs;
    m_currentPose.triggerSequence = m_triggerSequence;
}

void StateEstimator::clearTriggerPrediction()
{
    m_hasPrediction = false;
    m_predictedTriggerX = 0.0;
    m_predictedTriggerY = 0.0;
    m_predictedTriggerTimeUs = 0;
    m_predictedCountdownSec = -1.0;
    m_predictedTriggerVectorIndex = -1;
}

bool StateEstimator::hasActiveTrigger() const
{
    return m_nextTriggerVectorIndex >= 0
        && m_nextTriggerVectorIndex < static_cast<int>(m_triggerLines.size());
}

const TriggerLineSpec* StateEstimator::currentTriggerLine() const
{
    if (!hasActiveTrigger()) {
        return nullptr;
    }
    return &m_triggerLines[static_cast<size_t>(m_nextTriggerVectorIndex)];
}

int StateEstimator::inferTravelDirection(const LocalPoint* previousPoint, const LocalPoint* currentPoint) const
{
    if (previousPoint && currentPoint) {
        const double deltaY = currentPoint->y - previousPoint->y;
        const double pairThreshold = std::max(0.01, m_averagePointSpacing * 0.25);
        if (std::abs(deltaY) >= pairThreshold) {
            return deltaY > 0.0 ? 1 : -1;
        }
    }

    if (std::abs(m_windowVy) > kMinVyForPrediction) {
        return m_windowVy > 0.0 ? 1 : -1;
    }

    if (std::abs(m_currentPose.vy) > kMinVyForPrediction) {
        return m_currentPose.vy > 0.0 ? 1 : -1;
    }

    return 0;
}

int StateEstimator::findNextTriggerIndex(double currentY, int direction) const
{
    if (m_triggerLines.empty() || direction == 0) {
        return -1;
    }

    if (direction > 0) {
        for (int i = 0; i < static_cast<int>(m_triggerLines.size()); ++i) {
            if (m_triggerLines[static_cast<size_t>(i)].y >= currentY - kEpsilon) {
                return i;
            }
        }
        return -1;
    }

    for (int i = static_cast<int>(m_triggerLines.size()) - 1; i >= 0; --i) {
        if (m_triggerLines[static_cast<size_t>(i)].y <= currentY + kEpsilon) {
            return i;
        }
    }

    return -1;
}

void StateEstimator::syncTriggerTraversal(double currentY, int direction)
{
    if (direction == 0) {
        return;
    }

    const int nextVectorIndex = findNextTriggerIndex(currentY, direction);
    if (nextVectorIndex == m_nextTriggerVectorIndex) {
        return;
    }

    if (m_predictedTriggerVectorIndex != nextVectorIndex) {
        clearTriggerPrediction();
    }
    m_inFineMode = false;
    m_nextTriggerVectorIndex = nextVectorIndex;

    LOG_DEBUG("Trigger traversal synced: direction=%d, currentY=%.4f, nextLine=%d",
              direction,
              currentY,
              currentTriggerLine() ? currentTriggerLine()->lineIndex : 0);
}

void StateEstimator::advancePastCurrentPosition(double currentY, int direction, bool logSkip)
{
    if (direction == 0) {
        return;
    }

    while (hasActiveTrigger()) {
        const TriggerLineSpec* triggerLine = currentTriggerLine();
        if (!triggerLine) {
            break;
        }

        const double signedDistance = static_cast<double>(direction) * (triggerLine->y - currentY);
        if (signedDistance >= -m_deadzoneM) {
            break;
        }

        if (logSkip) {
            LOG_WARN("Skipping missed trigger line: line=%d, lineY=%.4f, currentY=%.4f, direction=%d",
                     triggerLine->lineIndex,
                     triggerLine->y,
                     currentY,
                     direction);
        }

        if (m_predictedTriggerVectorIndex == m_nextTriggerVectorIndex) {
            clearTriggerPrediction();
        }

        m_nextTriggerVectorIndex += direction;
        m_inFineMode = false;
    }
}

bool StateEstimator::interpolateCrossing(const LocalPoint& p1,
                                         const LocalPoint& p2,
                                         double targetY,
                                         double& triggerX,
                                         qint64& triggerTimeUs) const
{
    const double dy1 = p1.y - targetY;
    const double dy2 = p2.y - targetY;
    if ((dy1 < 0.0 && dy2 < 0.0) || (dy1 > 0.0 && dy2 > 0.0)) {
        return false;
    }

    const double deltaY = p2.y - p1.y;
    if (std::abs(deltaY) < kEpsilon) {
        return false;
    }

    const double alpha = (targetY - p1.y) / deltaY;
    if (alpha < -kEpsilon || alpha > 1.0 + kEpsilon) {
        return false;
    }

    const double clampedAlpha = std::max(0.0, std::min(1.0, alpha));
    triggerX = p1.x + clampedAlpha * (p2.x - p1.x);
    const double triggerTimeSec = p1.timestamp + clampedAlpha * (p2.timestamp - p1.timestamp);
    triggerTimeUs = static_cast<qint64>(std::llround(triggerTimeSec * 1e6));
    return true;
}

void StateEstimator::commitTrigger(double triggerX, double triggerY, qint64 triggerTimeUs, const char* source)
{
    m_hasLastTriggerEvent = true;
    m_lastTriggerX = triggerX;
    m_lastTriggerY = triggerY;
    m_lastTriggerTimeUs = triggerTimeUs;
    ++m_triggerSequence;

    clearTriggerPrediction();

    LOG_INFO("Trigger committed [%s]: seq=%d, line=%d, point=(%.4f, %.4f), timeUs=%lld",
             source ? source : "unknown",
             m_triggerSequence,
             currentTriggerLine() ? currentTriggerLine()->lineIndex : 0,
             triggerX,
             triggerY,
             static_cast<long long>(triggerTimeUs));
}

EstimatedPose StateEstimator::getPose() const
{
    return m_currentPose;
}

bool StateEstimator::commitPredictedTrigger()
{
    if (!m_hasPrediction || m_predictedTriggerVectorIndex != m_nextTriggerVectorIndex || !hasActiveTrigger()) {
        return false;
    }

    const int direction = m_lastTravelDirection != 0 ? m_lastTravelDirection : 1;
    commitTrigger(m_predictedTriggerX,
                  m_predictedTriggerY,
                  m_predictedTriggerTimeUs,
                  "predicted-timer");
    m_nextTriggerVectorIndex += direction;
    m_inFineMode = false;
    updateTriggerPoseFields();
    return true;
}

void StateEstimator::reset()
{
    m_currentPose = EstimatedPose();
    m_lastTimestamp = 0.0;
    m_lastLat = 0.0;
    m_lastLon = 0.0;
    m_lastSpeed = 0.0;
    m_lastHeading = 0.0;
    m_buffer.clear();

    m_nextTriggerVectorIndex = m_triggerLines.empty() ? -1 : 0;
    m_inFineMode = false;
    m_lastCoarseEvalTimestamp = 0.0;
    m_lastTravelDirection = 0;
    m_averageSpeed = 0.0;
    m_averageDirectionDeg = 0.0;
    m_averagePointSpacing = 0.0;
    m_windowVx = 0.0;
    m_windowVy = 0.0;
    clearTriggerPrediction();
    m_hasLastTriggerEvent = false;
    m_lastTriggerX = 0.0;
    m_lastTriggerY = 0.0;
    m_lastTriggerTimeUs = 0;
    m_triggerSequence = 0;
    updateTriggerPoseFields();

    LOG_INFO("State estimator reset completed");
}

void StateEstimator::calibrate()
{
    m_currentPose.heading = 0.0;
    m_currentPose.magneticHeading = 0.0;
    m_lastHeading = 0.0;
    LOG_INFO("State estimator calibrated: current heading set to 0 degrees");
}
