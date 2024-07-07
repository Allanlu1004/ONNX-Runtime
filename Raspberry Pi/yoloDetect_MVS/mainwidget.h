#ifndef MAINWIDGET_H
#define MAINWIDGET_H
#define MAX_MAINDEVICE_NUM 255
#include "ui_widget.h"
#include "ui_mainwidget.h"
#include "widget.h"
#include <QFileDialog>
#include <QFile>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <QMainWindow>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>
#include <QMimeDatabase>
#include <iostream>
#include "MvCamera.h"
#include <chrono>
#include <math.h>
#include <opencv2/highgui/highgui.hpp>
#include "mythread.h"
#include "yolo.h"
#include <QLineEdit>
#include <QTextCursor>


QPixmap MatImage(cv::Mat src);

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}

class Widget;              //前向声明，在qt中要用另一个类时
class CMvCamera;
class Yolo;
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    //存储相机配置界面的指针
    Widget *wid=NULL;

    CMvCamera *m_pcMyMainCamera[MAX_MAINDEVICE_NUM]; // 相机指针对象

    cv::Mat *myImage_Main = new cv::Mat(); //保存相机图像的图像指针对象

    MyThread *myThread_Camera_Mainshow = NULL;      //相机画面实时显示线程对象

    std::vector<std::string> className1 = { /*"person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
        "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
        "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
        "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
        "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
        "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
        "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone",
        "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear",
        "hair drier", "toothbrush" */"OK","NG"};

    Ui::MainWidget *ui;
    ~MainWidget();
public:
    // 状态
    bool m_bOpenDevice;                  // 是否打开设备
    bool m_bStartGrabbing;               // 是否开始抓图
    int m_nTriggerMode;                  // 触发模式
    int m_bContinueStarted;
    int num;
    // 开启过连续采集图像
    MV_SAVE_IAMGE_TYPE m_nSaveImageType; // 保存图像格式

    Yolo yolo_model;

private slots:

    void closeW();               //关闭子窗体的槽函数

    void readFrame();

    void on_openfile_clicked();

    void on_pbn_load_model_clicked();

    void display_myImage_Main(const cv::Mat *imagePrt);

    void getPtr();

    void yoloDetect( cv::Mat *imagePrt);
    //void showCamera();
    void on_checkBox_stateChanged(int arg1);


private:

    QTimer *timer;
    cv::VideoCapture *capture;

    std::vector<cv::Rect> bboxes;
    int IsDetect_ok =0;

};

#endif // MAINWIDGET_H
