#include "dbmanager.h"
#include <QDebug>
#include <QDateTime>
#include <QSqlError>

/**
 * @brief 构造函数
 * @param dbPath 数据库文件路径
 */
DBManager::DBManager(const QString& dbPath, QObject *parent)
    : QObject(parent), dbPath(dbPath)
{
}

/**
 * @brief 析构函数
 */
DBManager::~DBManager()
{
    closeDatabase();
}

/**
 * @brief 初始化数据库
 * @return 是否初始化成功
 */
bool DBManager::initialize()
{
    if (!openDatabase()) {
        qDebug() << "Failed to open database";
        return false;
    }
    
    if (!createTables()) {
        qDebug() << "Failed to create tables";
        closeDatabase();
        return false;
    }
    
    return true;
}

/**
 * @brief 打开数据库连接
 * @return 是否打开成功
 */
bool DBManager::openDatabase()
{
    // 移除旧的连接
    if (db.isOpen()) {
        db.close();
    }
    
    // 添加SQLite数据库连接
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    
    if (!db.open()) {
        qDebug() << "Failed to open database:" << db.lastError().text();
        return false;
    }
    
    return true;
}

/**
 * @brief 关闭数据库连接
 */
void DBManager::closeDatabase()
{
    if (db.isOpen()) {
        db.close();
    }
}

/**
 * @brief 创建数据库表
 * @return 是否创建成功
 */
bool DBManager::createTables()
{
    // 创建路径表
    QString createPathTable = R"(
        CREATE TABLE IF NOT EXISTS paths (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP
        );
    )";
    
    // 创建路径点表
    QString createPathPointsTable = R"(
        CREATE TABLE IF NOT EXISTS path_points (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            path_id INTEGER NOT NULL,
            x REAL NOT NULL,
            y REAL NOT NULL,
            sequence INTEGER NOT NULL,
            FOREIGN KEY (path_id) REFERENCES paths(id)
        );
    )";
    
    // 创建播种详情表
    QString createSeedingDetailsTable = R"(
        CREATE TABLE IF NOT EXISTS seeding_details (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            path_id INTEGER NOT NULL,
            field_id INTEGER NOT NULL,
            start_time TEXT NOT NULL,
            end_time TEXT NOT NULL,
            start_x REAL NOT NULL,
            start_y REAL NOT NULL,
            end_x REAL NOT NULL,
            end_y REAL NOT NULL,
            seeding_status INTEGER NOT NULL,
            FOREIGN KEY (path_id) REFERENCES paths(id)
        );
    )";
    
    // 执行创建表的SQL语句
    QSqlQuery query;
    if (!query.exec(createPathTable)) {
        qDebug() << "Failed to create paths table:" << query.lastError().text();
        return false;
    }
    
    if (!query.exec(createPathPointsTable)) {
        qDebug() << "Failed to create path_points table:" << query.lastError().text();
        return false;
    }
    
    if (!query.exec(createSeedingDetailsTable)) {
        qDebug() << "Failed to create seeding_details table:" << query.lastError().text();
        return false;
    }
    
    return true;
}

/**
 * @brief 保存路径数据
 * @param pathId 路径ID
 * @param pathName 路径名称
 * @param points 路径点列表
 * @return 是否保存成功
 */
bool DBManager::savePathData(int pathId, const QString& pathName, const std::vector<QPointF>& points)
{
    if (!db.isOpen()) {
        if (!openDatabase()) {
            return false;
        }
    }
    
    // 开始事务
    if (!db.transaction()) {
        qDebug() << "Failed to begin transaction:" << db.lastError().text();
        return false;
    }
    
    // 插入路径记录
    QSqlQuery query;
    query.prepare("INSERT INTO paths (id, name) VALUES (:id, :name)");
    query.bindValue(":id", pathId);
    query.bindValue(":name", pathName);
    
    if (!query.exec()) {
        qDebug() << "Failed to insert path:" << query.lastError().text();
        db.rollback();
        return false;
    }
    
    // 插入路径点记录
    query.prepare("INSERT INTO path_points (path_id, x, y, sequence) VALUES (:path_id, :x, :y, :sequence)");
    for (size_t i = 0; i < points.size(); ++i) {
        query.bindValue(":path_id", pathId);
        query.bindValue(":x", points[i].x());
        query.bindValue(":y", points[i].y());
        query.bindValue(":sequence", static_cast<int>(i));
        
        if (!query.exec()) {
            qDebug() << "Failed to insert path point:" << query.lastError().text();
            db.rollback();
            return false;
        }
    }
    
    // 提交事务
    if (!db.commit()) {
        qDebug() << "Failed to commit transaction:" << db.lastError().text();
        db.rollback();
        return false;
    }
    
    return true;
}

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
bool DBManager::saveSeedingDetail(int pathId, int fieldId, const QString& startTime, const QString& endTime,
                                 const QPointF& startPosition, const QPointF& endPosition, bool seedingStatus)
{
    if (!db.isOpen()) {
        if (!openDatabase()) {
            return false;
        }
    }
    
    // 插入播种详情记录
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO seeding_details (path_id, field_id, start_time, end_time, start_x, start_y, end_x, end_y, seeding_status)
        VALUES (:path_id, :field_id, :start_time, :end_time, :start_x, :start_y, :end_x, :end_y, :seeding_status)
    )");
    
    query.bindValue(":path_id", pathId);
    query.bindValue(":field_id", fieldId);
    query.bindValue(":start_time", startTime);
    query.bindValue(":end_time", endTime);
    query.bindValue(":start_x", startPosition.x());
    query.bindValue(":start_y", startPosition.y());
    query.bindValue(":end_x", endPosition.x());
    query.bindValue(":end_y", endPosition.y());
    query.bindValue(":seeding_status", seedingStatus ? 1 : 0);
    
    if (!query.exec()) {
        qDebug() << "Failed to insert seeding detail:" << query.lastError().text();
        return false;
    }
    
    return true;
}

/**
 * @brief 获取所有路径数据
 * @return 路径数据列表
 */
std::vector<std::pair<int, QString>> DBManager::getAllPaths()
{
    std::vector<std::pair<int, QString>> paths;
    
    if (!db.isOpen()) {
        if (!openDatabase()) {
            return paths;
        }
    }
    
    QSqlQuery query("SELECT id, name FROM paths ORDER BY created_at DESC");
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        paths.emplace_back(id, name);
    }
    
    return paths;
}

/**
 * @brief 获取路径点数据
 * @param pathId 路径ID
 * @return 路径点列表
 */
std::vector<QPointF> DBManager::getPathPoints(int pathId)
{
    std::vector<QPointF> points;
    
    if (!db.isOpen()) {
        if (!openDatabase()) {
            return points;
        }
    }
    
    QSqlQuery query;
    query.prepare("SELECT x, y FROM path_points WHERE path_id = :path_id ORDER BY sequence");
    query.bindValue(":path_id", pathId);
    
    if (query.exec()) {
        while (query.next()) {
            double x = query.value(0).toDouble();
            double y = query.value(1).toDouble();
            points.emplace_back(x, y);
        }
    } else {
        qDebug() << "Failed to get path points:" << query.lastError().text();
    }
    
    return points;
}

/**
 * @brief 获取播种详情
 * @param pathId 路径ID
 * @return 播种详情列表
 */
std::vector<std::tuple<int, int, QString, QString, QPointF, QPointF, bool>> DBManager::getSeedingDetails(int pathId)
{
    std::vector<std::tuple<int, int, QString, QString, QPointF, QPointF, bool>> details;
    
    if (!db.isOpen()) {
        if (!openDatabase()) {
            return details;
        }
    }
    
    QSqlQuery query;
    query.prepare(R"(
        SELECT id, field_id, start_time, end_time, start_x, start_y, end_x, end_y, seeding_status
        FROM seeding_details
        WHERE path_id = :path_id
        ORDER BY start_time
    )");
    query.bindValue(":path_id", pathId);
    
    if (query.exec()) {
        while (query.next()) {
            int id = query.value(0).toInt();
            int fieldId = query.value(1).toInt();
            QString startTime = query.value(2).toString();
            QString endTime = query.value(3).toString();
            double startX = query.value(4).toDouble();
            double startY = query.value(5).toDouble();
            double endX = query.value(6).toDouble();
            double endY = query.value(7).toDouble();
            bool seedingStatus = query.value(8).toInt() == 1;
            
            details.emplace_back(
                id,
                fieldId,
                startTime,
                endTime,
                QPointF(startX, startY),
                QPointF(endX, endY),
                seedingStatus
            );
        }
    } else {
        qDebug() << "Failed to get seeding details:" << query.lastError().text();
    }
    
    return details;
}
