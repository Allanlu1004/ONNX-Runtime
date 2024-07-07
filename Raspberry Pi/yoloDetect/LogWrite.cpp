#include "LogWrite.h"
#include <QCoreApplication>
#include <QDir>

LogWrite& LogWrite::instance()
{
    static LogWrite instance;
    return instance;
}

LogWrite::LogWrite()
{
    // 确保logs文件夹存在
    QString logDirPath = QCoreApplication::applicationDirPath() + "/logs";
    QDir dir(logDirPath);
    if (!dir.exists())
    {
        dir.mkpath("."); // 如果不存在，就创建logs文件夹
    }

    openLogFile();
}

LogWrite::~LogWrite()
{
    if (logFile.isOpen())
    {
        logFile.close();
    }
}

void LogWrite::addEntry(const QString& status, const QString& info)
{
    QMutexLocker locker(&mutex); // 确保此区块的线程安全
    QString currentTime = getCurrentTime();
    out << "[" << currentTime << "][" << status.toUpper() << "][" << info << "]" << endl;
}

QString LogWrite::getCurrentTime() const
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
}

void LogWrite::openLogFile()
{
    QString fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss") + ".txt";
    QString filePath = QCoreApplication::applicationDirPath() + "/logs/" + fileName;

    logFile.setFileName(filePath);

    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        return; // Handle error appropriately, maybe log to console or throw
    }

    out.setDevice(&logFile);
}
