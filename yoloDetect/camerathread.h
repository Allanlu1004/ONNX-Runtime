#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QImage>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <QThread>
#include "LogWrite.h"
#include <QMutex>

class CameraThread : public QObject
{
    Q_OBJECT
public:
    explicit CameraThread(QObject *parent = nullptr);

private:
    QTimer          *timerCam;
    QTimer          *timerDetect;

    //定义一个Mat变量，用于存储每一帧的图像
    cv::Mat cap;
    cv::Mat cap_detect;
    //声明视频读入类
    cv::VideoCapture capture;
    // 相机读入的图像地址
    cv::Mat* imagePtr = NULL;
    cv::Mat* detectImagePtr = NULL;
public:
    void openCamara();
    void startDetect();
    void closeCamera();
    static QMutex mutex;

    void getImagePtr(cv::Mat* image);//获取用于显示图像的指针
    void getDetectImagePtr(cv::Mat* image);

private slots:
    void readFarme();       // 读取当前帧信息
    void readFrameforDetect();

signals:
    void image(cv::Mat img);
    void success(QString str);
    void imageDetect(const cv::Mat* image, cv::Mat frame);
    void imageDisplay(const cv::Mat* image);


};

#endif // CAMERATHREAD_H
