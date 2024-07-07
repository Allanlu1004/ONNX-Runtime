#ifndef YOLOTHREAD_H
#define YOLOTHREAD_H

#include <QObject>
#include <QThread>
#include "yolo.h"

class yoloThread : public QObject
{
    Q_OBJECT
public:
    explicit yoloThread(QObject *parent = nullptr);
    Yolo myYolo;
    void detect(cv::Mat frame);
    //QTimer          *timer;

signals:
    void image(cv::Mat img);
    void imageBoxInfo(std::vector<BoxInfo>);
    void BoxInfoMessage(QString message);
};

#endif // YOLOTHREAD_H
