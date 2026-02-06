#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QString>
#include <QPointF>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <vector>

/**
 * @brief 数据库管理类，负责处理sqlite数据库的操作
 */
class DBManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param dbPath 数据库文件路径
     */
    explicit DBManager(const QString& dbPath = "./gps_seeder.db", QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~DBManager();
    
    /**
     * @brief 初始化数据库
     * @return 是否初始化成功
     */
    bool initialize();
    
    /**
     * @brief 保存路径数据
     * @param pathId 路径ID
     * @param pathName 路径名称
     * @param points 路径点列表
     * @return 是否保存成功
     */
    bool savePathData(int pathId, const QString& pathName, const std::vector<QPointF>& points);
    
    /**
     * @brief 保存播种详情
     * @param pathId 路径ID
     * @param fieldId 小区ID
     * @param startTime 开始时间
     * @param endTime 结束时间
     * @param startPosition 开始位置
     * @param endPosition 结束位置
     * @param seedingStatus 播种状态
     * @return 是否保存成功
     */
    bool saveSeedingDetail(int pathId, int fieldId, const QString& startTime, const QString& endTime,
                          const QPointF& startPosition, const QPointF& endPosition, bool seedingStatus);
    
    /**
     * @brief 获取所有路径数据
     * @return 路径数据列表
     */
    std::vector<std::pair<int, QString>> getAllPaths();
    
    /**
     * @brief 获取路径点数据
     * @param pathId 路径ID
     * @return 路径点列表
     */
    std::vector<QPointF> getPathPoints(int pathId);
    
    /**
     * @brief 获取播种详情
     * @param pathId 路径ID
     * @return 播种详情列表
     */
    std::vector<std::tuple<int, int, QString, QString, QPointF, QPointF, bool>> getSeedingDetails(int pathId);

private:
    /**
     * @brief 创建数据库表
     * @return 是否创建成功
     */
    bool createTables();
    
    /**
     * @brief 打开数据库连接
     * @return 是否打开成功
     */
    bool openDatabase();
    
    /**
     * @brief 关闭数据库连接
     */
    void closeDatabase();
    
    QString dbPath; ///< 数据库文件路径
    QSqlDatabase db; ///< 数据库连接
};

#endif // DBMANAGER_H