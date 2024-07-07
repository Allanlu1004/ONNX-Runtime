#include "camerathread.h"

CameraThread::CameraThread(QObject *parent)
    : QObject{parent}
{
    timerCam   = new QTimer(this);
    connect(timerCam, SIGNAL(timeout()), this, SLOT(readFarme()));  // 时间到，读取当前摄像头信息

    timerDetect = new QTimer(this);
    connect(timerDetect, SIGNAL(timeout()), this, SLOT(readFrameforDetect()));

}

void CameraThread::openCamara()
{
    // 设置原始图像的长宽
    int originalWidth = 640;
    int originalHeight = 480;
    capture.open(0);
    qDebug() << "open";
    if (!capture.isOpened()) //先判断是否打开摄像头
    {
         qDebug("err");
    }

    // 设置摄像头的原始分辨率
    capture.set(cv::CAP_PROP_FRAME_WIDTH, originalWidth);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, originalHeight);
    capture.set(cv::CAP_PROP_FPS, 30);
    capture.set(cv::CAP_PROP_AUTO_EXPOSURE, 0.75);

    timerCam->start(33);   // 开始计时，33ms获取一帧传给主线程显示函数
}

void CameraThread::startDetect()
{
    timerDetect->start(50);
}

void CameraThread::closeCamera()
{
    timerCam->stop();
}
QMutex CameraThread::mutex;
void CameraThread::readFarme()
{
    mutex.lock();
    capture.read(cap);
    if (!cap.empty()) //判断当前帧是否捕捉成功
    {
        emit image(cap);
    }
    else
    {
        qDebug() << "can not ";
        LogWrite::instance().addEntry("message","can not");
    }
    mutex.unlock();
}

void CameraThread::readFrameforDetect()
{
    capture.read(cap);
    if (!cap.empty()) //判断当前帧是否捕捉成功
    {
        emit imageDetect(cap);
    }
    else
    {
        qDebug() << "can not ";
        LogWrite::instance().addEntry("message","can not");
    }
}
