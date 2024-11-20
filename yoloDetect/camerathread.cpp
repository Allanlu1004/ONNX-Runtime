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
    timerDetect->start(33);
}

void CameraThread::closeCamera()
{
    timerCam->stop();
}

void CameraThread::getImagePtr(cv::Mat *image)
{
    imagePtr = image;
}

void CameraThread::getDetectImagePtr(cv::Mat *image)
{
    detectImagePtr = image;
}

QMutex CameraThread::mutex;
void CameraThread::readFarme()
{
    mutex.lock();
    capture.read(cap);
    cv::Mat* matPtr = &cap;
    getImagePtr(matPtr);
    if (!cap.empty()) //判断当前帧是否捕捉成功
    {
        //emit image(cap);
        emit imageDisplay(imagePtr);
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
    capture.read(cap_detect);
    cv::Mat* matPtr = &cap_detect;
    getDetectImagePtr(matPtr);
    if (!cap_detect.empty()) //判断当前帧是否捕捉成功
    {
        emit imageDetect(detectImagePtr, cap_detect);
    }
    else
    {
        qDebug() << "can not ";
        LogWrite::instance().addEntry("message","can not");
    }
}
