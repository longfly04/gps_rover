#ifndef BLOCKGENERATOR_H
#define BLOCKGENERATOR_H

#include <QPointF>
#include <vector>
#include <string>

/**
 * @brief 小区规格
 */
struct BlockSpec {
    int blockId;              ///< 小区编号（从1开始）
    int startRow;             ///< 起始垄号
    int endRow;               ///< 结束垄号
    double yStart;            ///< Y轴起始位置（米）
    double yEnd;              ///< Y轴结束位置（米）
    double width;             ///< 小区宽度（米）
    double length;            ///< 小区长度（米）
};

/**
 * @brief 过道规格
 */
struct HeadlandSpec {
    int headlandId;           ///< 过道编号（从1开始）
    double yStart;            ///< Y轴起始位置（米）
    double yEnd;              ///< Y轴结束位置（米）
    double width;             ///< 过道宽度（米）
};

/**
 * @brief 边界顶点
 */
struct BoundaryVertex {
    double x;                 ///< X坐标（米）
    double y;                 ///< Y坐标（米）
};

/**
 * @brief 小区边界
 */
struct BlockBoundary {
    int blockId;              ///< 小区编号
    std::vector<BoundaryVertex> forwardBoundary;  ///< 正向边界顶点（从下往上）
    std::vector<BoundaryVertex> reverseBoundary;  ///< 反向边界顶点（从上往下）
    std::string forwardBoundaryStr;  ///< 正向边界字符串（用于显示）
    std::string reverseBoundaryStr;  ///< 反向边界字符串（用于显示）
};

/**
 * @brief 控制点
 */
struct BlockControlPoint {
    int blockId;              ///< 所属小区编号
    int rowNumber;            ///< 垄号
    double x;                 ///< X坐标（米）
    double y;                 ///< Y坐标（米）
    bool isStart;             ///< 是否为起始点
};

/**
 * @brief 小区规划结果
 */
struct BlockPlanResult {
    std::vector<BlockSpec> blocks;           ///< 小区列表
    std::vector<HeadlandSpec> headlands;     ///< 过道列表
    std::vector<BlockBoundary> boundaries;   ///< 边界列表
    std::vector<BlockControlPoint> controlPoints; ///< 控制点列表
    
    double totalLength;        ///< 总长度（米）
    double totalWidth;         ///< 总宽度（米）
    int totalBlocks;           ///< 小区总数
    int totalRows;             ///< 总垄数
    
    bool isValid;              ///< 结果是否有效
    std::string errorMessage;  ///< 错误信息
};

/**
 * @brief 小区生成器，负责根据区域参数生成虚拟小区、过道、边界和控制点
 */
class BlockGenerator
{
public:
    /**
     * @brief 构造函数
     */
    BlockGenerator();
    
    /**
     * @brief 析构函数
     */
    ~BlockGenerator();
    
    /**
     * @brief 设置垄距
     * @param rowSpacing 垄距（米），最小0.1米
     */
    void setRowSpacing(double rowSpacing);
    
    /**
     * @brief 设置过道宽度
     * @param headlandWidth 过道宽度（米），最小1.0米
     */
    void setHeadlandWidth(double headlandWidth);
    
    /**
     * @brief 设置每小区垄数
     * @param rowsPerBlock 每小区垄数
     */
    void setRowsPerBlock(int rowsPerBlock);
    
    /**
     * @brief 设置小区长度
     * @param fieldLength 小区长度（米），最小1.0米
     */
    void setFieldLength(double fieldLength);
    
    /**
     * @brief 设置区域范围
     * @param areaLength 区域长度（米）
     * @param areaWidth 区域宽度（米）
     */
    void setAreaRange(double areaLength, double areaWidth);
    
    /**
     * @brief 设置开始触发距离
     * @param distance 触发距离（米）
     */
    void setTriggerDistance(double distance);
    
    /**
     * @brief 设置中断触发距离
     * @param distance 触发距离（米）
     */
    void setStopTriggerDistance(double distance);
    
    /**
     * @brief 生成小区规划
     * @return 小区规划结果
     */
    BlockPlanResult generate();
    
    /**
     * @brief 获取当前的小区规划结果
     * @return 小区规划结果
     */
    BlockPlanResult getResult() const;
    
    /**
     * @brief 获取垄距
     * @return 垄距（米）
     */
    double getRowSpacing() const;
    
    /**
     * @brief 获取过道宽度
     * @return 过道宽度（米）
     */
    double getHeadlandWidth() const;
    
    /**
     * @brief 获取每小区垄数
     * @return 每小区垄数
     */
    int getRowsPerBlock() const;
    
    /**
     * @brief 获取小区长度
     * @return 小区长度（米）
     */
    double getFieldLength() const;
    
    /**
     * @brief 获取区域长度
     * @return 区域长度（米）
     */
    double getAreaLength() const;
    
    /**
     * @brief 获取区域宽度
     * @return 区域宽度（米）
     */
    double getAreaWidth() const;
    
    /**
     * @brief 检查指定Y坐标是否在播种区
     * @param y Y坐标
     * @return 是否在播种区
     */
    bool isSeedingAtY(double y) const;
    
    /**
     * @brief 获取指定Y坐标所属的小区编号
     * @param y Y坐标
     * @return 小区编号（0表示不在任何小区）
     */
    int getBlockIdAtY(double y) const;

private:
    /**
     * @brief 生成小区和过道
     */
    void generateBlocksAndHeadlands();
    
    /**
     * @brief 生成边界
     */
    void generateBoundaries();
    
    /**
     * @brief 生成控制点
     */
    void generateControlPoints();
    
    /**
     * @brief 生成播种网格
     */
    void generateSeedingGrid();
    
    /**
     * @brief 格式化边界顶点为字符串
     * @param vertices 顶点列表
     * @return 格式化字符串
     */
    std::string formatBoundaryVertices(const std::vector<BoundaryVertex>& vertices) const;
    
    double m_rowSpacing;       ///< 垄距（米）
    double m_headlandWidth;    ///< 过道宽度（米）
    double m_fieldLength;      ///< 小区长度（米）
    int m_rowsPerBlock;        ///< 每小区垄数
    double m_areaLength;       ///< 区域长度（米）
    double m_areaWidth;        ///< 区域宽度（米）
    double m_triggerDistance;  ///< 开始触发距离（米）
    double m_stopTriggerDistance; ///< 中断触发距离（米）
    
    double m_gridStep;         ///< 网格步长（米）
    
    BlockPlanResult m_result;  ///< 当前规划结果
    
    std::vector<bool> m_seedingGrid; ///< 播种网格
    double m_gridMinY;         ///< 网格最小Y坐标
    double m_gridMaxY;         ///< 网格最大Y坐标
};

#endif // BLOCKGENERATOR_H
