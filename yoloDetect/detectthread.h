#ifndef DETECTTHREAD_H
#define DETECTTHREAD_H
#include <QObject>
#include <QThread>
#include "yolo.h"



class yoloThread : public QObject
{
    Q_OBJECT
public:
    explicit yoloThread(QObject *parent = nullptr);
public:
    Yolo myYolo;
    void detect(cv::Mat frame);

//private slots:
//    void detect(cv::Mat &frame);

signals:
    void image(cv::Mat img);
};

#endif // DETECTTHREAD_H
