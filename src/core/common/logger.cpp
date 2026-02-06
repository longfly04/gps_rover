#include "logger.h"

// 实现Logger类的方法

// 注意：Logger类的方法已经在头文件中实现，这里只需要包含头文件即可
// 但是为了支持信号功能，我们需要添加一些额外的代码

// 由于Logger类需要发送信号到主窗口，我们需要使用Qt的信号槽机制
// 但是Logger类本身不是QObject，所以我们需要创建一个辅助类来处理信号

#include <QObject>

class LoggerSignalEmitter : public QObject
{
    Q_OBJECT

public:
    static LoggerSignalEmitter& getInstance() {
        static LoggerSignalEmitter instance;
        return instance;
    }

signals:
    void logMessageGenerated(const QString& message);

public slots:
    void emitLogMessage(const QString& message) {
        emit logMessageGenerated(message);
    }
};

// 为了能够在Logger类中使用信号，我们需要修改log方法
// 但是由于Logger类的方法是模板方法，无法在cpp文件中实现
// 所以我们需要在头文件中直接修改

// 这里我们只需要确保LoggerSignalEmitter类的实现正确即可

// 注意：由于LoggerSignalEmitter类使用了Q_OBJECT宏，需要确保Qt能够正确处理
// 对于单独的.cpp文件中的Q_OBJECT宏，需要手动包含生成的moc文件
#include "logger.moc"
