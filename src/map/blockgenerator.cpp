#include "blockgenerator.h"
#include "../core/common/logger.h"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

/**
 * @brief 构造函数，初始化默认参数
 */
BlockGenerator::BlockGenerator()
    : m_rowSpacing(0.6),
      m_headlandWidth(1.0),
      m_fieldLength(5.0),
      m_rowsPerBlock(2),
      m_areaLength(100.0),
      m_areaWidth(100.0),
      m_triggerDistance(1.0),
      m_stopTriggerDistance(0.5),
      m_gridStep(0.1),
      m_gridMinY(0.0),
      m_gridMaxY(0.0)
{
    m_result.totalLength = m_areaLength;
    m_result.totalWidth = m_areaWidth;
    m_result.totalBlocks = 0;
    m_result.totalRows = 0;
    m_result.triggerStartY = 0.0;
    m_result.triggerStopY = 0.0;
    m_result.triggerInterval = 0.0;
    m_result.isValid = false;
}

/**
 * @brief 析构函数
 */
BlockGenerator::~BlockGenerator()
{
}

/**
 * @brief 设置垄距
 * @param rowSpacing 垄距（米），最小0.1米
 */
void BlockGenerator::setRowSpacing(double rowSpacing)
{
    m_rowSpacing = std::max(0.1, std::round(rowSpacing * 10.0) / 10.0);
}

/**
 * @brief 设置过道宽度
 * @param headlandWidth 过道宽度（米），最小1.0米
 */
void BlockGenerator::setHeadlandWidth(double headlandWidth)
{
    m_headlandWidth = std::max(1.0, std::round(headlandWidth * 10.0) / 10.0);
}

/**
 * @brief 设置每小区垄数
 * @param rowsPerBlock 每小区垄数
 */
void BlockGenerator::setRowsPerBlock(int rowsPerBlock)
{
    m_rowsPerBlock = std::max(1, rowsPerBlock);
}

/**
 * @brief 设置小区长度
 * @param fieldLength 小区长度（米），最小1.0米
 */
void BlockGenerator::setFieldLength(double fieldLength)
{
    m_fieldLength = std::max(1.0, fieldLength);
}

/**
 * @brief 设置区域范围
 * @param areaLength 区域长度（米）
 * @param areaWidth 区域宽度（米）
 */
void BlockGenerator::setAreaRange(double areaLength, double areaWidth)
{
    m_areaLength = std::max(1.0, areaLength);
    m_areaWidth = std::max(1.0, areaWidth);
}

/**
 * @brief 设置开始触发距离
 * @param distance 触发距离（米）
 */
void BlockGenerator::setTriggerDistance(double distance)
{
    m_triggerDistance = std::max(0.1, distance);
}

/**
 * @brief 设置中断触发距离
 * @param distance 触发距离（米）
 */
void BlockGenerator::setStopTriggerDistance(double distance)
{
    m_stopTriggerDistance = std::max(0.1, distance);
}

/**
 * @brief 生成小区规划
 * @return 小区规划结果
 */
BlockPlanResult BlockGenerator::generate()
{
    try {
        LOG_INFO("Starting block generation");
        
        m_result = BlockPlanResult();
        m_result.isValid = false;
        
        generateBlocksAndHeadlands();
        generateBoundaries();
        generateControlPoints();
        generateTriggerLines();
        generateSeedingGrid();

        m_result.totalLength = m_areaLength;
        m_result.totalWidth = m_areaWidth;
        m_result.totalBlocks = static_cast<int>(m_result.blocks.size());
        m_result.totalRows = static_cast<int>(std::ceil(m_areaWidth / m_rowSpacing));
        
        m_result.isValid = true;
        
        LOG_INFO("Block generation completed: %d blocks, %d headlands", 
                 m_result.totalBlocks, static_cast<int>(m_result.headlands.size()));
        
        return m_result;
    } catch (const std::exception& e) {
        LOG_Error("Error generating blocks: %s", e.what());
        m_result.isValid = false;
        m_result.errorMessage = e.what();
        return m_result;
    }
}

/**
 * @brief 获取当前的小区规划结果
 * @return 小区规划结果
 */
BlockPlanResult BlockGenerator::getResult() const
{
    return m_result;
}

/**
 * @brief 生成小区和过道
 */
void BlockGenerator::generateBlocksAndHeadlands()
{
    m_result.blocks.clear();
    m_result.headlands.clear();
    
    double currentY = 0.0;
    int blockId = 1;
    int headlandId = 1;
    int currentRow = 1;
    
    while (currentY < m_areaLength) {
        double blockEndY = currentY + m_fieldLength;
        if (blockEndY > m_areaLength) {
            blockEndY = m_areaLength;
        }
        
        BlockSpec block;
        block.blockId = blockId;
        block.startRow = currentRow;
        block.endRow = currentRow + m_rowsPerBlock - 1;
        block.yStart = currentY;
        block.yEnd = blockEndY;
        block.width = m_rowsPerBlock * m_rowSpacing;
        block.length = m_fieldLength;
        
        m_result.blocks.push_back(block);
        
        currentRow += m_rowsPerBlock;
        blockId++;
        currentY = blockEndY;
        
        if (currentY >= m_areaLength) {
            break;
        }
        
        double headlandEndY = currentY + m_headlandWidth;
        if (headlandEndY > m_areaLength) {
            headlandEndY = m_areaLength;
        }
        
        HeadlandSpec headland;
        headland.headlandId = headlandId;
        headland.yStart = currentY;
        headland.yEnd = headlandEndY;
        headland.width = m_headlandWidth;
        
        m_result.headlands.push_back(headland);
        
        headlandId++;
        currentY = headlandEndY;
    }
    
    LOG_INFO("Generated %d blocks and %d headlands", 
             static_cast<int>(m_result.blocks.size()),
             static_cast<int>(m_result.headlands.size()));
}

/**
 * @brief 生成边界
 */
void BlockGenerator::generateBoundaries()
{
    m_result.boundaries.clear();
    
    for (const auto& block : m_result.blocks) {
        BlockBoundary boundary;
        boundary.blockId = block.blockId;
        
        double leftX = 0.0;
        double rightX = block.width;
        
        boundary.forwardBoundary.clear();
        boundary.reverseBoundary.clear();
        
        boundary.forwardBoundary.push_back({leftX, block.yStart});
        boundary.forwardBoundary.push_back({rightX, block.yStart});
        boundary.forwardBoundary.push_back({rightX, block.yEnd});
        boundary.forwardBoundary.push_back({leftX, block.yEnd});
        
        boundary.reverseBoundary.push_back({leftX, block.yEnd});
        boundary.reverseBoundary.push_back({rightX, block.yEnd});
        boundary.reverseBoundary.push_back({rightX, block.yStart});
        boundary.reverseBoundary.push_back({leftX, block.yStart});
        
        boundary.forwardBoundaryStr = formatBoundaryVertices(boundary.forwardBoundary);
        boundary.reverseBoundaryStr = formatBoundaryVertices(boundary.reverseBoundary);
        
        m_result.boundaries.push_back(boundary);
    }
    
    LOG_INFO("Generated %d boundaries", static_cast<int>(m_result.boundaries.size()));
}

/**
 * @brief 生成控制点
 */
void BlockGenerator::generateControlPoints()
{
    m_result.controlPoints.clear();

    for (const auto& block : m_result.blocks) {
        for (int row = block.startRow; row <= block.endRow; ++row) {
            double rowX = (row - 1) * m_rowSpacing + m_rowSpacing / 2.0;

            double startY = block.yStart + m_triggerDistance;
            double endY = block.yEnd - m_stopTriggerDistance;

            BlockControlPoint startPt;
            startPt.blockId = block.blockId;
            startPt.rowNumber = row;
            startPt.x = rowX;
            startPt.y = startY;
            startPt.isStart = true;
            m_result.controlPoints.push_back(startPt);

            BlockControlPoint endPt;
            endPt.blockId = block.blockId;
            endPt.rowNumber = row;
            endPt.x = rowX;
            endPt.y = endY;
            endPt.isStart = false;
            m_result.controlPoints.push_back(endPt);
        }
    }

    LOG_INFO("Generated %d control points", static_cast<int>(m_result.controlPoints.size()));
}

/**
 * @brief 生成触发线
 */
void BlockGenerator::generateTriggerLines()
{
    m_result.triggerLines.clear();

    const double interval = m_fieldLength + m_headlandWidth;
    const double clampedStartInputY = std::clamp(m_triggerDistance, 0.0, m_areaLength);
    const double clampedStopInputY = std::clamp(m_stopTriggerDistance, 0.0, m_areaLength);
    const double startY = std::min(clampedStartInputY, clampedStopInputY);
    const double stopY = std::max(clampedStartInputY, clampedStopInputY);

    m_result.triggerStartY = startY;
    m_result.triggerStopY = stopY;
    m_result.triggerInterval = interval;

    if (interval <= 0.0 || startY > stopY + 1e-9) {
        LOG_WARN("Trigger line generation skipped: startY=%.4f, stopY=%.4f, interval=%.4f",
                 startY, stopY, interval);
        return;
    }

    if (clampedStartInputY != m_triggerDistance || clampedStopInputY != m_stopTriggerDistance) {
        LOG_WARN("Trigger range clamped to area bounds: inputStartY=%.4f, inputStopY=%.4f, areaLength=%.4f, usedStartY=%.4f, usedStopY=%.4f",
                 m_triggerDistance,
                 m_stopTriggerDistance,
                 m_areaLength,
                 startY,
                 stopY);
    } else if (m_triggerDistance > m_stopTriggerDistance) {
        LOG_WARN("Trigger range reordered by position: startInput=%.4f, stopInput=%.4f, usedStartY=%.4f, usedStopY=%.4f",
                 m_triggerDistance,
                 m_stopTriggerDistance,
                 startY,
                 stopY);
    }

    int lineIndex = 1;
    for (double y = startY; y <= stopY + 1e-9; y += interval) {
        TriggerLineSpec line;
        line.lineIndex = lineIndex++;
        line.y = y;
        line.xStart = 0.0;
        line.xEnd = m_areaWidth;
        m_result.triggerLines.push_back(line);
    }

    LOG_INFO("Generated %d trigger lines: startY=%.4f, stopY=%.4f, interval=%.4f",
             static_cast<int>(m_result.triggerLines.size()),
             m_result.triggerStartY,
             m_result.triggerStopY,
             m_result.triggerInterval);
}

/**
 * @brief 生成播种网格
 */
void BlockGenerator::generateSeedingGrid()
{
    m_gridMinY = 0.0;
    m_gridMaxY = m_areaLength;
    
    int gridSize = static_cast<int>(std::ceil((m_gridMaxY - m_gridMinY) / m_gridStep)) + 1;
    m_seedingGrid.resize(gridSize, false);
    
    for (int i = 0; i < gridSize; ++i) {
        double y = m_gridMinY + i * m_gridStep;
        m_seedingGrid[i] = isSeedingAtY(y);
    }
}

/**
 * @brief 格式化边界顶点为字符串
 * @param vertices 顶点列表
 * @return 格式化字符串
 */
std::string BlockGenerator::formatBoundaryVertices(const std::vector<BoundaryVertex>& vertices) const
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    
    for (size_t i = 0; i < vertices.size(); ++i) {
        if (i > 0) oss << ";";
        oss << "(" << vertices[i].x << "," << vertices[i].y << ")";
    }
    
    return oss.str();
}

/**
 * @brief 获取垄距
 * @return 垄距（米）
 */
double BlockGenerator::getRowSpacing() const
{
    return m_rowSpacing;
}

/**
 * @brief 获取过道宽度
 * @return 过道宽度（米）
 */
double BlockGenerator::getHeadlandWidth() const
{
    return m_headlandWidth;
}

/**
 * @brief 获取每小区垄数
 * @return 每小区垄数
 */
int BlockGenerator::getRowsPerBlock() const
{
    return m_rowsPerBlock;
}

/**
 * @brief 获取小区长度
 * @return 小区长度（米）
 */
double BlockGenerator::getFieldLength() const
{
    return m_fieldLength;
}

/**
 * @brief 获取区域长度
 * @return 区域长度（米）
 */
double BlockGenerator::getAreaLength() const
{
    return m_areaLength;
}

/**
 * @brief 获取区域宽度
 * @return 区域宽度（米）
 */
double BlockGenerator::getAreaWidth() const
{
    return m_areaWidth;
}

/**
 * @brief 检查指定Y坐标是否在播种区
 * @param y Y坐标
 * @return 是否在播种区
 */
bool BlockGenerator::isSeedingAtY(double y) const
{
    for (const auto& block : m_result.blocks) {
        if (y >= block.yStart - 1e-6 && y <= block.yEnd + 1e-6) {
            return true;
        }
    }
    return false;
}

/**
 * @brief 获取指定Y坐标所属的小区编号
 * @param y Y坐标
 * @return 小区编号（0表示不在任何小区）
 */
int BlockGenerator::getBlockIdAtY(double y) const
{
    for (const auto& block : m_result.blocks) {
        if (y >= block.yStart - 1e-6 && y <= block.yEnd + 1e-6) {
            return block.blockId;
        }
    }
    return 0;
}
