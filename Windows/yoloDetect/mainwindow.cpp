#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qdebug.h>
#include <QMetaType>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qRegisterMetaType<BoxInfo>("BoxInfo");
    qRegisterMetaType< cv::Mat >("cv::Mat");
    qRegisterMetaType<std::vector<QPointF>>("std::vector<BoxInfo>");
    ui->setupUi(this);

    // 创建线程对象
    QThread* subcam = new QThread;
    QThread* subYolo = new QThread;
    // 创建工作的类对象
    // 千万不要指定给创建的对象指定父对象
    // 如果指定了: QObject::moveToThread: Cannot move objects with a parent
    CameraThread* camerathread = new CameraThread;
    yoloThread* yolothread = new yoloThread;
    //yolothread = new yoloThread;
    // 将工作的类对象移动到创建的子线程对象中
    camerathread->moveToThread(subcam);
    yolothread->moveToThread(subYolo);
    // 启动线程
    subcam->start();
    subYolo->start();
    // 让工作的对象开始工作, 点击开始按钮, 开始工作
    connect(ui->startBtn, &QPushButton::clicked, camerathread, &CameraThread::openCamara);
    connect(camerathread,&CameraThread::image,this,&MainWindow::showImg);
    connect(ui->takeBtn, &QPushButton::clicked, camerathread, &CameraThread::startDetect);
    connect(camerathread,&CameraThread::imageDetect, yolothread, &yoloThread::detect);
    connect(yolothread, &yoloThread::image, this, &MainWindow::showImgDetected);
    //connect(yolothread, &yoloThread::imageBoxInfo, this, &MainWindow::showBoxInfo);
    connect(yolothread, &yoloThread::BoxInfoMessage, this, &MainWindow::showBoxInfo_1);
    //timerYolo = new QTimer(this);
    //connect(timerYolo, SIGNAL(timeout()), this, SLOT(yoloDetect()));  // 时间到，进行推理

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showImg(cv::Mat img)
{
    cv::Mat showimage = img.clone();
    QImage showImageQ;
    showImageQ = Mat2QImage(showimage);
    ui->camera->setPixmap(QPixmap::fromImage(showImageQ));  // 将图片显示到label上
    //LogWrite::instance().addEntry("message","show image success!");
}

void MainWindow::showImgDetected(cv::Mat img)
{
    cv::Mat showImg = img.clone();
    QImage showImageQ;
    showImageQ = Mat2QImage(showImg);
    showImageQ = showImageQ.scaled(ui->camera->width(), ui->camera->height(),
                                        Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//设置图片大小和label的长宽一致

    ui->picture->setPixmap(QPixmap::fromImage(showImageQ));  // 将图片显示到label上
}

void MainWindow::showBoxInfo(std::vector<BoxInfo> imageboxinfo)
{
    qDebug() << imageboxinfo.size();
//    int label = imageboxinfo[0].label;
//    qDebug() << label;
//    for(int i = 0; i < imageboxinfo.size(); i++)
//    {
//        BoxInfo boxinfo = imageboxinfo[i];
//        QString boxinfoToString;
//        boxinfoToString =  QString("Box Information:\n"
//                   "x1: %1\n"
//                   "y1: %2\n"
//                   "x2: %3\n"
//                   "y2: %4\n"
//                   "Score: %5\n"
//                   "Label: %6")
//        .arg(boxinfo.x1)
//        .arg(boxinfo.y1)
//        .arg(boxinfo.x2)
//        .arg(boxinfo.y2)
//        .arg(boxinfo.score)
//        .arg(boxinfo.label);
//        qDebug() << boxinfoToString;
//        //ui->BoxInfo->setText(boxinfoToString);
    //    }
}

void MainWindow::showBoxInfo_1(QString boxinfo)
{
    //qDebug() << boxinfo;
    ui->BoxInfo->setText(boxinfo);

}

// 图片转换
QImage  MainWindow::Mat2QImage(cv::Mat cvImg)
{
    QImage qImg;
    if(cvImg.channels()==3)     //3 channels color image
    {

        cv::cvtColor(cvImg,cvImg,cv::COLOR_BGR2RGB);
        qImg =QImage((const unsigned char*)(cvImg.data),
                    cvImg.cols, cvImg.rows,
                    cvImg.cols*cvImg.channels(),
                    QImage::Format_RGB888);
    }
    else if(cvImg.channels()==1)                    //grayscale image
    {
        qImg =QImage((const unsigned char*)(cvImg.data),
                    cvImg.cols,cvImg.rows,
                    cvImg.cols*cvImg.channels(),
                    QImage::Format_Indexed8);
    }
    else
    {
        qImg =QImage((const unsigned char*)(cvImg.data),
                    cvImg.cols,cvImg.rows,
                    cvImg.cols*cvImg.channels(),
                    QImage::Format_RGB888);
    }
    return qImg;
}


void MainWindow::on_closeBtn_clicked()
{
    qApp->quit();
}


void MainWindow::on_loadYoloBtn_clicked()
{
    LogWrite::instance().addEntry("message","11111");
    Yolo yolo_model;
    std::string imgpath = "D:/Qt/code/code7.6/yoloDetect/sample/person.jpg";
    cv::Mat srcimg = cv::imread(imgpath);
    yolo_model.detect(srcimg);
    cv::imshow("resultImage", srcimg);
    cv::waitKey(0);
}


