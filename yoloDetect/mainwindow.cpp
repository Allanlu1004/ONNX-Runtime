#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qdebug.h>
#include <QMetaType>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qRegisterMetaType<BoxInfo_v5lite>("BoxInfo_v5lite");
    qRegisterMetaType< cv::Mat >("cv::Mat");
    qRegisterMetaType<std::vector<BoxInfo_v5lite>>("std::vector<BoxInfo_v5lite>");
    ui->setupUi(this);
    this->setWindowTitle("OnnxRuntime推理框架测试工具");

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
    //connect(camerathread,&CameraThread::image,this,&MainWindow::showImg);
    connect(camerathread,&CameraThread::imageDisplay,this,&MainWindow::showImg_1);
    connect(ui->takeBtn, &QPushButton::clicked, camerathread, &CameraThread::startDetect);
    connect(camerathread,&CameraThread::imageDetect, yolothread, &yoloThread::detect);
    connect(yolothread, &yoloThread::image, this, &MainWindow::showImgDetected);
    connect(yolothread, &yoloThread::BoxInfoMessage, this, &MainWindow::showBoxInfo_1);
    connect(yolothread, &yoloThread::boxinfo, this, &MainWindow::showBoxInfo_2);
    //timerYolo = new QTimer(this);
    //connect(timerYolo, SIGNAL(timeout()), this, SLOT(yoloDetect()));  // 时间到，进行推理

    //connect(yolothread, &yoloThread::boxInfoUpdated,this,&MainWindow::processBoxInfo);

    // 连接初始化yolo模型信号槽
    connect(this,&MainWindow::signalinitYolov5,yolothread,&yoloThread::slotinitYolov5);
    connect(this,&MainWindow::signalinitYolov5lite,yolothread,&yoloThread::slotinitYolov5lite);


    initUI();

}

void MainWindow::initUI()
{
    // 设置默认模型路径以及图片路径
    ui->modelPath_lineEdit->setText("/home/allan/Code/cpp/onnxRuntime+usbCam/yoloDetect/onnxmodels/yolov5s.onnx");
    ui->imagePath_lineEdit->setText("/home/allan/Code/cpp/onnxRuntime+usbCam/yoloDetect/sample/bus.jpg");

    modelPath = "/home/allan/Code/cpp/onnxRuntime+usbCam/yoloDetect/onnxmodels/yolov5s.onnx";
    imagePath = "/home/allan/Code/cpp/onnxRuntime+usbCam/yoloDetect/sample/bus.jpg";

    // 默认使用yolov5s模型进行推理
    ui->chooseModel_checkBox->setChecked(false);
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

void MainWindow::showImg_1(const cv::Mat* imgPtr)
{
    cv::Mat showImage = *imgPtr;
    QImage showImageQ;
    showImageQ = Mat2QImage(showImage);
    ui->camera->setPixmap(QPixmap::fromImage(showImageQ));  // 将图片显示到label上
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

void MainWindow::showBoxInfo_1(QString boxinfo)
{
    //qDebug() << boxinfo;
    ui->BoxInfo->setText(boxinfo);

}

void MainWindow::showBoxInfo_2(QVariant info)
{
    std::vector<BoxInfo_v5lite> box = info.value<std::vector<BoxInfo_v5lite>>();
    qDebug() << "box = ";
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

void MainWindow::processBoxInfo(const std::vector<BoxInfo_v5lite> &boxInfoList)
{
    qDebug() << "111";
    qDebug() << "box = " << boxInfoList[0].score;
}


void MainWindow::on_closeBtn_clicked()
{
    qApp->quit();
}


void MainWindow::on_detect_v5lite_Button_clicked()
{
    Net_config_v5lite yolo_nets = { 0.5, 0.6, 0.5 ,"/home/allan/Code/cpp/onnxRuntime+usbCam/yoloDetect/onnxmodels/v5lite-s.onnx", "/home/allan/Code/cpp/onnxRuntime+usbCam/yoloDetect/sample/coco.names",true};
    Yolo yolo_model(yolo_nets);
    cv::Mat srcimg = cv::imread("/home/allan/Code/cpp/onnxRuntime+usbCam/yoloDetect/sample/bus.jpg");
    yolo_model.detect_v5lite(srcimg);
    cv::imshow("result_v5lite", srcimg);
    cv::waitKey(0);
}


void MainWindow::on_detect_v5_Button_clicked()
{
    clock_t startTime,endTime; //计算时间
    Configuration yolo_nets = { 0.5, 0.6, 0.5,"/home/allan/Code/cpp/onnxRuntime+usbCam/yoloDetect/onnxmodels/yolov5s.onnx" };
    Yolov5s yolo_model(yolo_nets);

    Mat srcimg = imread("/home/allan/Code/cpp/onnxRuntime+usbCam/yoloDetect/sample/bus.jpg");

    double timeStart = (double)getTickCount();
    startTime = clock();//计时开始
    yolo_model.detect_v5(srcimg);
    endTime = clock();//计时结束
    double nTime = ((double)getTickCount() - timeStart) / getTickFrequency();
    cout << "clock_running time is:" <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
    cout << "The run time is:" << (double)clock() /CLOCKS_PER_SEC<< "s" << endl;
    cout << "getTickCount_running time :" << nTime << "sec\n" << endl;
    cv::imshow("result_v5",srcimg);
    waitKey(0);
}


void MainWindow::on_set_modelPath_Button_clicked()
{
    QString file_path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("请选择文件路径..."), "./",
                                                          "模型文件(*.onnx)");
    if(file_path.isEmpty())
    {
        return;
    }
    ui->modelPath_lineEdit->setText(file_path);
    modelPath = ui->modelPath_lineEdit->text();

}


void MainWindow::on_set_imagePath_Button_clicked()
{
    QString file_path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("请选择文件路径..."), "./",
                                                          "图片文件(*.jpg *.png)");
    if(file_path.isEmpty())
    {
        return;
    }
    ui->imagePath_lineEdit->setText(file_path);
    imagePath = ui->imagePath_lineEdit->text();
}


void MainWindow::on_set_videoPath_Button_clicked()
{
    QString file_path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("请选择文件路径..."), "./",
                                                          "照片文件(*.mp4)");
    if(file_path.isEmpty())
    {
        return;
    }
    ui->videoPath_lineEdit->setText(file_path);
    videoPath = ui->videoPath_lineEdit->text();
}


void MainWindow::on_chooseModel_checkBox_stateChanged(int arg1)
{
    if(arg1 == 2)
    {
        v5liteFlag = true;
    }
    else
    {
        v5liteFlag = false;
    }
}


void MainWindow::on_initModel_Button_clicked()
{
    if(v5liteFlag)
    {
        emit signalinitYolov5lite(modelPath);
    }
    else
    {
        emit signalinitYolov5(modelPath);
    }
}

