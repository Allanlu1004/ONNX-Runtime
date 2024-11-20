#ifndef LOGWRITE_H
#define LOGWRITE_H

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDateTime>
#include <QMutex>
#include <QDebug>

//Using Example
//
//LogWrite::instance().addEntry("message", "这是一个信息级别的日志条目");
//LogWrite::instance().addEntry("warning", "这是一个警告级别的日志条目");
//LogWrite::instance().addEntry("error", "这是一个错误级别的日志条目");
//

class LogWrite
{
public:
    static LogWrite& instance();
    void addEntry(const QString& status, const QString& info);

private:
    LogWrite();
    ~LogWrite();
    LogWrite(const LogWrite&) = delete;
    LogWrite& operator=(const LogWrite&) = delete;

    QFile logFile;
    QTextStream out;
    QMutex mutex;
    QString getCurrentTime() const;
    void openLogFile();
};

#endif // LOGWRITE_H
