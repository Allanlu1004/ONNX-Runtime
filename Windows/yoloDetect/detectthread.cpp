#include "detectthread.h"

yoloThread::yoloThread(QObject *parent): QObject{parent}
{

}

void yoloThread::detect(cv::Mat frame)
{
    myYolo.detect(frame);
    qDebug() << "yoloThread::detect";
    emit image(frame);
}
