#include "pathgenerator.h"
#include <cmath>
#include <algorithm>
#include <QDebug>
#include <stdexcept>

/**
 * @brief 构造函数，初始化默认参数
 */
PathGenerator::PathGenerator() 
    : fieldManager(nullptr),
      rowSpacing(0.6), // 垄距，默认0.6米
      headlandWidth(1.0), // 过道宽度，默认1米
      blockLength(5.0), // 小区长度，默认5米
      rowsPerBlock(2), // 每小区行数，默认2垄
      minSeedingLength(0.2),
      gridStep(0.1),
      gridMinY(0.0),
      gridMaxY(0.0)
{
}

/**
 * @brief 析构函数
 */
PathGenerator::~PathGenerator()
{
}

/**
 * @brief 设置田块管理器
 * @param fieldManager 田块管理器指针
 */
void PathGenerator::setFieldManager(FieldManager* fieldManager)
{
    this->fieldManager = fieldManager;
}

/**
 * @brief 设置行距
 * @param rowSpacing 行距（米），最小0.1米
 */
void PathGenerator::setRowSpacing(double rowSpacing)
{
    // 确保行距不小于0.1米，并对齐到0.1米的倍数
    this->rowSpacing = std::max(0.1, std::round(rowSpacing * 10.0) / 10.0);
}

/**
 * @brief 设置过道宽度
 * @param headlandWidth 过道宽度（米），最小1.0米
 */
void PathGenerator::setHeadlandWidth(double headlandWidth)
{
    // 确保过道宽度不小于1.0米，并对齐到0.1米的倍数
    this->headlandWidth = std::max(1.0, std::round(headlandWidth * 10.0) / 10.0);
}

/**
 * @brief 设置每块行数
 * @param rowsPerBlock 每块行数
 */
void PathGenerator::setRowsPerBlock(int rowsPerBlock)
{
    // 确保每块行数至少为1
    this->rowsPerBlock = std::max(1, rowsPerBlock);
}

/**
 * @brief 设置小区长度
 * @param blockLength 小区长度（米），最小1.0米
 */
void PathGenerator::setBlockLength(double blockLength)
{
    // 确保小区长度不小于1.0米
    this->blockLength = std::max(1.0, blockLength);
}

/**
 * @brief 设置最小播种段长度
 * @param minSeedingLength 最小播种段长度（米），默认0.2米
 */
void PathGenerator::setMinSeedingLength(double minSeedingLength)
{
    // 确保最小播种段长度不小于0.2米
    this->minSeedingLength = std::max(0.2, minSeedingLength);
}

/**
 * @brief 生成播种块和过道块
 */
void PathGenerator::generateBlocks()
{
    if (!fieldManager) {
        return;
    }
    
    // 清空之前的路径段
    pathSegments.clear();
    
    // 获取田块的Y轴范围
    double minY, maxY;
    fieldManager->getYRange(minY, maxY);
    
    // 计算Y轴覆盖范围
    double currentY = minY;
    
    // 生成播种块和过道块
    while (currentY < maxY) {
        // 计算播种块的结束位置
        double blockEnd = currentY + blockLength;
        if (blockEnd > maxY) {
            blockEnd = maxY;
        }
        
        // 添加播种块
        pathSegments.emplace_back(currentY, blockEnd, true);
        
        // 计算下一个过道的结束位置
        currentY = blockEnd + headlandWidth;
        if (currentY >= maxY) {
            break;
        }
        
        // 添加过道块
        double headlandEnd = currentY;
        pathSegments.emplace_back(blockEnd, headlandEnd, false);
    }
}

/**
 * @brief 生成路径序列
 */
void PathGenerator::generatePathSequence()
{
    if (!fieldManager) {
        return;
    }
    
    // 清空之前的路径点
    pathPoints.clear();
    
    // 获取田块的Y轴范围
    double minY, maxY;
    fieldManager->getYRange(minY, maxY);
    
    // 获取田块的X轴范围
    double minX, maxX;
    fieldManager->getXRange(minX, maxX);
    
    // 计算X方向需要的趟数
    const int rowsPerPass = 4; // 每趟播种4垄
    double passSpacing = rowsPerPass * rowSpacing;
    int numPasses = static_cast<int>(std::ceil((maxX - minX) / passSpacing)) + 1;
    
    // 生成路径点
    for (int pass = 0; pass < numPasses; ++pass) {
        // 计算当前趟的起始X坐标
        double passStartX = minX + pass * passSpacing;
        
        // 生成当前趟的路径
        double currentY = minY;
        int segmentIndex = 0;
        
        while (currentY < maxY) {
            // 检查当前位置是否在播种区
            bool isSeedingZone = false;
            for (const auto& segment : pathSegments) {
                double segMinY = std::get<0>(segment);
                double segMaxY = std::get<1>(segment);
                bool segSeeding = std::get<2>(segment);
                
                if (currentY >= segMinY && currentY < segMaxY && segSeeding) {
                    isSeedingZone = true;
                    break;
                }
            }
            
            if (isSeedingZone) {
                // 计算当前块的结束Y坐标
                double blockEndY = currentY + blockLength;
                if (blockEndY > maxY) {
                    blockEndY = maxY;
                }
                
                // 生成当前块的路径
                for (int row = 0; row < rowsPerPass; ++row) {
                    double rowY = currentY + (row + 0.5) * rowSpacing;
                    
                    // 根据趟索引和块索引决定行驶方向
                    if ((pass + segmentIndex) % 2 == 0) {
                        // 偶数趟或偶数块：从左到右
                        pathPoints.emplace_back(passStartX, rowY);
                        pathPoints.emplace_back(passStartX + (rowsPerPass - 1) * rowSpacing, rowY);
                    } else {
                        // 奇数趟或奇数块：从右到左
                        pathPoints.emplace_back(passStartX + (rowsPerPass - 1) * rowSpacing, rowY);
                        pathPoints.emplace_back(passStartX, rowY);
                    }
                }
                
                // 移动到下一个块
                currentY = blockEndY + headlandWidth;
                segmentIndex++;
            } else {
                // 跳过过道
                for (const auto& segment : pathSegments) {
                    double segMinY = std::get<0>(segment);
                    double segMaxY = std::get<1>(segment);
                    bool segSeeding = std::get<2>(segment);
                    
                    if (currentY >= segMinY && currentY < segMaxY && !segSeeding) {
                        currentY = segMaxY;
                        break;
                    }
                }
            }
        }
    }
}

/**
 * @brief 生成弓型路径
 */
void PathGenerator::generateShuttlePath()
{
    try {
        if (!fieldManager) {
            throw std::runtime_error("Field manager not set");
        }
        
        qDebug() << "Starting to generate shuttle path";
        
        // 生成播种块和过道块
        generateBlocks();
        qDebug() << "Generated" << pathSegments.size() << "path segments";
        
        // 生成路径序列
        generatePathSequence();
        qDebug() << "Generated" << pathPoints.size() << "path points";
        
        // 生成播种/过道网格
        generateSeedingGrid();
        qDebug() << "Generated seeding grid with" << seedingGrid.size() << "cells";
        
        qDebug() << "Shuttle path generation completed successfully";
    } catch (const std::exception& e) {
        qDebug() << "Error generating shuttle path:" << e.what();
        // 清空结果，确保系统不会使用无效数据
        pathPoints.clear();
        pathSegments.clear();
        seedingGrid.clear();
    }
}

/**
 * @brief 生成播种/过道网格
 */
void PathGenerator::generateSeedingGrid()
{
    if (!fieldManager) {
        return;
    }
    
    // 获取田块的Y轴范围
    fieldManager->getYRange(gridMinY, gridMaxY);
    
    // 计算网格大小
    int gridSize = static_cast<int>(std::ceil((gridMaxY - gridMinY) / gridStep)) + 1;
    seedingGrid.resize(gridSize, false);
    
    // 填充网格
    for (int i = 0; i < gridSize; ++i) {
        double y = gridMinY + i * gridStep;
        
        // 检查当前Y坐标是否在播种区
        bool isSeeding = false;
        for (const auto& segment : pathSegments) {
            double yStart = std::get<0>(segment);
            double yEnd = std::get<1>(segment);
            bool segmentSeeding = std::get<2>(segment);
            
            if (y >= yStart - 1e-6 && y <= yEnd + 1e-6) {
                isSeeding = segmentSeeding;
                break;
            }
        }
        
        seedingGrid[i] = isSeeding;
    }
}

/**
 * @brief 获取生成的路径点
 * @return 路径点列表
 */
std::vector<QPointF> PathGenerator::getPathPoints() const
{
    return pathPoints;
}

/**
 * @brief 获取播种/过道区间
 * @return 区间列表，每个区间为(y_start, y_end, is_seeding)
 */
std::vector<std::tuple<double, double, bool>> PathGenerator::getPathSegments() const
{
    return pathSegments;
}

/**
 * @brief 获取当前的行距
 * @return 行距（米）
 */
double PathGenerator::getRowSpacing() const
{
    return rowSpacing;
}

/**
 * @brief 获取当前的过道宽度
 * @return 过道宽度（米）
 */
double PathGenerator::getHeadlandWidth() const
{
    return headlandWidth;
}

/**
 * @brief 获取当前的每块行数
 * @return 每块行数
 */
int PathGenerator::getRowsPerBlock() const
{
    return rowsPerBlock;
}

/**
 * @brief 获取当前的小区长度
 * @return 小区长度（米）
 */
double PathGenerator::getBlockLength() const
{
    return blockLength;
}

/**
 * @brief 检查指定Y坐标是否在播种区
 * @param y Y坐标
 * @return 是否在播种区
 */
bool PathGenerator::isSeedingAtY(double y) const
{
    // 检查Y坐标是否在网格范围内
    if (y < gridMinY - 1e-6 || y > gridMaxY + 1e-6) {
        return false;
    }
    
    // 计算网格索引
    int index = static_cast<int>((y - gridMinY) / gridStep);
    index = std::max(0, std::min(index, static_cast<int>(seedingGrid.size()) - 1));
    
    // 返回网格状态
    return seedingGrid[index];
}

/**
 * @brief 获取播种网格
 * @return 播种网格
 */
std::vector<bool> PathGenerator::getSeedingGrid() const
{
    return seedingGrid;
}

/**
 * @brief 获取网格步长
 * @return 网格步长（米）
 */
double PathGenerator::getGridStep() const
{
    return gridStep;
}

/**
 * @brief 获取网格Y轴范围
 * @param minY 最小Y坐标
 * @param maxY 最大Y坐标
 */
void PathGenerator::getGridYRange(double& minY, double& maxY) const
{
    minY = gridMinY;
    maxY = gridMaxY;
}