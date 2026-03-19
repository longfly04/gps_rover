#include "ui/mainwindow.h"
#include <QApplication>
#include <QIcon>
#include "core/common/logger.h"

/**
 * @brief 主函数，应用程序的入口点
 * @param argc 命令行参数数量
 * @param argv 命令行参数数组
 * @return 应用程序退出代码
 */
int main(int argc, char *argv[])
{
    // 初始化日志系统
    Logger::getInstance().setLevel(Logger::LogLevel::INFO);
    Logger::getInstance().setLogFile("gps_rover.log");
    
    LOG_INFO("GPS-Rover application started");
    LOG_INFO("Command line arguments: %d", argc);
    for (int i = 0; i < argc; i++) {
        LOG_INFO("Argument %d: %s", i, argv[i]);
    }
    
    QApplication a(argc, argv);
    // 设置应用程序图标
    a.setWindowIcon(QIcon("d:/GPS-Seeder/gps_rover/resource/icon.jpg"));
    LOG_INFO("QApplication initialized");
    
    MainWindow w;
    // 设置主窗口图标
    w.setWindowIcon(QIcon("d:/GPS-Seeder/gps_rover/resource/icon.jpg"));
    LOG_INFO("MainWindow created");
    
    w.show();
    LOG_INFO("MainWindow shown");
    
    int exitCode = a.exec();
    
    LOG_INFO("Application exiting with code: %d", exitCode);
    
    // 关闭日志文件
    Logger::getInstance().closeLogFile();
    
    return exitCode;
}