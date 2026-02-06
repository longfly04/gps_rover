#ifndef PATHGENERATOR_H
#define PATHGENERATOR_H

#include <QPointF>
#include <vector>
#include "fieldmanager.h"

/**
 * @brief 路径生成模块，负责生成弓型覆盖路径
 */
class PathGenerator
{
public:
    /**
     * @brief 构造函数
     */
    PathGenerator();
    
    /**
     * @brief 析构函数
     */
    ~PathGenerator();
    
    /**
     * @brief 设置田块管理器
     * @param fieldManager 田块管理器指针
     */
    void setFieldManager(FieldManager* fieldManager);
    
    /**
     * @brief 设置行距
     * @param rowSpacing 行距（米），最小0.1米
     */
    void setRowSpacing(double rowSpacing);
    
    /**
     * @brief 设置过道宽度
     * @param headlandWidth 过道宽度（米），最小1.0米
     */
    void setHeadlandWidth(double headlandWidth);
    
    /**
     * @brief 设置每块行数
     * @param rowsPerBlock 每块行数
     */
    void setRowsPerBlock(int rowsPerBlock);
    
    /**
     * @brief 设置小区长度
     * @param blockLength 小区长度（米），最小1.0米
     */
    void setBlockLength(double blockLength);
    
    /**
     * @brief 设置最小播种段长度
     * @param minSeedingLength 最小播种段长度（米），默认0.2米
     */
    void setMinSeedingLength(double minSeedingLength);
    
    /**
     * @brief 生成弓型路径
     */
    void generateShuttlePath();
    
    /**
     * @brief 获取生成的路径点
     * @return 路径点列表
     */
    std::vector<QPointF> getPathPoints() const;
    
    /**
     * @brief 获取播种/过道区间
     * @return 区间列表，每个区间为(y_start, y_end, is_seeding)
     */
    std::vector<std::tuple<double, double, bool>> getPathSegments() const;
    
    /**
     * @brief 获取当前的行距
     * @return 行距（米）
     */
    double getRowSpacing() const;
    
    /**
     * @brief 获取当前的过道宽度
     * @return 过道宽度（米）
     */
    double getHeadlandWidth() const;
    
    /**
     * @brief 获取当前的每块行数
     * @return 每块行数
     */
    int getRowsPerBlock() const;
    
    /**
     * @brief 获取当前的小区长度
     * @return 小区长度（米）
     */
    double getBlockLength() const;
    
    /**
     * @brief 检查指定Y坐标是否在播种区
     * @param y Y坐标
     * @return 是否在播种区
     */
    bool isSeedingAtY(double y) const;
    
    /**
     * @brief 获取播种网格
     * @return 播种网格
     */
    std::vector<bool> getSeedingGrid() const;
    
    /**
     * @brief 获取网格步长
     * @return 网格步长（米）
     */
    double getGridStep() const;
    
    /**
     * @brief 获取网格Y轴范围
     * @param minY 最小Y坐标
     * @param maxY 最大Y坐标
     */
    void getGridYRange(double& minY, double& maxY) const;

private:
    /**
     * @brief 生成播种块和过道块
     */
    void generateBlocks();
    
    /**
     * @brief 生成路径序列
     */
    void generatePathSequence();
    
    /**
     * @brief 生成播种/过道网格
     */
    void generateSeedingGrid();
    
    FieldManager* fieldManager; ///< 田块管理器指针
    double rowSpacing; ///< 垄距（米），默认0.6米
    double headlandWidth; ///< 过道宽度（米），默认1米
    double blockLength; ///< 小区长度（米），默认5米
    int rowsPerBlock; ///< 每小区行数，默认2垄
    double minSeedingLength; ///< 最小播种段长度（米）
    double gridStep; ///< 网格步长（米），默认0.1米
    
    std::vector<QPointF> pathPoints; ///< 生成的路径点
    std::vector<std::tuple<double, double, bool>> pathSegments; ///< 路径段（y_start, y_end, is_seeding）
    std::vector<bool> seedingGrid; ///< 播种网格，标记每个Y坐标是否为播种区
    double gridMinY; ///< 网格最小Y坐标
    double gridMaxY; ///< 网格最大Y坐标
};

#endif // PATHGENERATOR_H