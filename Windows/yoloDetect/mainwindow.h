#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QFile>
#include <opencv2/opencv.hpp>
#include <QMainWindow>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>
#include <QMimeDatabase>
#include <iostream>
#include <chrono>
#include <math.h>
#include <opencv2/highgui/highgui.hpp>
#include "camerathread.h"
#include <QLineEdit>
#include <QTextCursor>
#include "yolo.h"
#include "LogWrite.h"
#include "yolothread.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//前向声明，在qt中要用另一个类时
class CameraThread;
class Yolo;
class yoloThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //显示画面
    void showImg(cv::Mat img);

    void showImgDetected(cv::Mat img);

    void showBoxInfo(std::vector<BoxInfo> imageboxinfo);

    void showBoxInfo_1(QString boxinfo);

    void on_closeBtn_clicked();

    void on_loadYoloBtn_clicked();

public:
    QImage  Mat2QImage(cv::Mat cvImg);

private:
    Ui::MainWindow *ui;
    QTimer *timerYolo;
    QImage imag;
    QImage imag_res;
    cv::Mat cap; //定义一个Mat变量，用于存储每一帧的图像
    int index = 0; //存储图像数量
    Yolo yolo_model;
    //CameraThread* video = nullptr;
    //yoloThread* yolothread = nullptr;

};
#endif // MAINWINDOW_H
