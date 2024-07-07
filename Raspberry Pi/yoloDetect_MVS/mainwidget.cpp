#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "widget.h"
#include <QPushButton>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("YoloV5-lite"));
    ui->lbl_res_pic->setScaledContents(true);
    ui->txt_message->setLineWrapMode(QTextEdit::WidgetWidth);
    timer =new QTimer(this);
    timer->setInterval(200);                     //设置定时器触发的间隔

    num=1;
    //将定时器与读取数据流的函数连接起来，每当定时器触发，执行readFrame
    connect(timer,&QTimer::timeout,this,&MainWidget::readFrame);

    //将定时器与读取相机帧率以及检测的函数连接起来，每当定时器触发，执行yoloDetect

    //ui->startdetect->setEnabled(false);

    //
    wid=new Widget();
    //点击相机配置按钮，显示相机配置界面，隐藏主界面
    connect(ui->pbn_start_grab,&QPushButton::clicked,wid,[=]()
    {
        this->hide();
        wid->show();
    });

    //关闭相机配置界面后，显示主界面
    connect(this->wid, &Widget::closedWid, this,[=]()
    {
        this->show();
    });

    //主界面接受信号后，会显示主界面，隐藏相机界面
    connect(this->wid,&Widget::back,this,[=]()
    {
        this->wid->hide();
        this->show();

    });

    this->myThread_Camera_Mainshow=new MyThread;
    connect(wid->myThread_Camera_show, SIGNAL(GetPtr(cv::Mat *)), this,
            SLOT(yoloDetect(cv::Mat *)));

    //设置打开图片和摄像头button为disable，只有当加载模型后才会Enable
    ui->openfile->setEnabled(false);
    ui->pbn_start_grab->setEnabled(false);
}
MainWidget::~MainWidget()
{
    capture->release();
    delete capture;
    delete ui;
    delete wid;


}


//读取流函数
void MainWidget::readFrame()
{
    qDebug() << "read frame";
    cv::Mat frame;
    capture->read(frame);
    if (frame.empty())
        return;

    auto start = std::chrono::steady_clock::now();
   //yolov5->detect(frame);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    ui->txt_message->append(QString("cost_time: %1 ms").arg(elapsed.count()));
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    QImage rawImage = QImage((uchar*)(frame.data),frame.cols,frame.rows,frame.step,QImage::Format_RGB888);
    ui->lbl_res_pic->setPixmap(QPixmap::fromImage(rawImage));
}


//根据定时器触发实时检测
void MainWidget::yoloDetect(cv::Mat *myImage_Main)
{
    //读取相机流
    //m_pcMyMainCamera[0]->ReadBuffer(*myImage_Main);
    if(myImage_Main!=NULL)
    {
        // yolo detect
        cv::Mat temp;
        if(myImage_Main->channels()==4)
            cv::cvtColor(*myImage_Main,temp,cv::COLOR_BGRA2RGB);
        else if (myImage_Main->channels()==3)
            cv::cvtColor(*myImage_Main,temp,cv::COLOR_BGR2RGB);
        else
            cv::cvtColor(*myImage_Main,temp,cv::COLOR_GRAY2RGB);

        auto start = std::chrono::steady_clock::now();
        yolo_model.detect(temp);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        ui->txt_message->append(QString("dtetct time: %1 ms").arg(elapsed.count()));
        ui->time_tet->setOverwriteMode(true);
        ui->time_tet->setText(QString("%1 ms").arg(elapsed.count()));

        QImage img = QImage((uchar*)(temp.data),temp.cols,temp.rows,temp.step,QImage::Format_RGB888);
        ui->lbl_res_pic->setPixmap(QPixmap::fromImage(img));
        ui->lbl_res_pic->resize(ui->lbl_res_pic->pixmap()->size());

        if(ui->checkBox->isChecked())
        {
            QString savePath=QString("/home/pi/Desktop/softward/code/test_opencv/LiteTest/res_img/resImg_%1.png").arg(num);
            img.save(savePath);
        }
        num++;
    }
    else
    {
        ui->txt_message->append("CamearImage is empty!");
    }
}


//关闭子窗体  显示父窗体
void MainWidget::closeW()
{
    this->show();
}


//打开文件按钮的槽函数
void MainWidget::on_openfile_clicked()
{

    //过滤除了"*.mp4 *.avi;;*.png *.jpg *.jpeg *.bmp"以外的文件
    QString filename = QFileDialog::getOpenFileName(this,QStringLiteral("打开文件"),"/home/pi/Desktop/softward/code/test_opencv/LiteTest/onnx/sample",".*.png *.jpg *.jpeg *.bmp;;*.mp4 *.avi");
    if(!QFile::exists(filename)){
        return;
    }

    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(filename);
    if (mime.name().startsWith("image/")) {
        cv::Mat src = cv::imread(filename.toLatin1().data());
        if(src.empty()){
            return;
        }
        cv::Mat temp;
        if(src.channels()==4)
            cv::cvtColor(src,temp,cv::COLOR_BGRA2RGB);
        else if (src.channels()==3)
            cv::cvtColor(src,temp,cv::COLOR_BGR2RGB);
        else
            cv::cvtColor(src,temp,cv::COLOR_GRAY2RGB);
        QImage Img;
        if (src.channels() == 3)
        {
            Img = QImage((const uchar*)(temp.data), temp.cols, temp.rows, temp.cols * temp.channels(), QImage::Format_RGB888);
        }
        else//Gray Img
        {
            Img = QImage((const uchar*)(temp.data), temp.cols, temp.rows, temp.cols*temp.channels(), QImage::Format_Indexed8);
        }
        ui->lbl_res_pic->setPixmap(QPixmap::fromImage(Img));
        ui->lbl_res_pic->resize(ui->lbl_res_pic->pixmap()->size());


        // yolo detect
        auto start = std::chrono::steady_clock::now();
        yolo_model.detect(src);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        ui->txt_message->append(QString("const_time: %1 ms").arg(elapsed.count()));
        ui->time_tet->setOverwriteMode(true);
        ui->time_tet->setText(QString("%1 ms").arg(elapsed.count()));

        if(src.channels()==4)
            cv::cvtColor(src,src,cv::COLOR_BGRA2RGB);
        else if (src.channels()==3)
            cv::cvtColor(src,src,cv::COLOR_BGR2RGB);
        else
            cv::cvtColor(src,src,cv::COLOR_GRAY2RGB);
        QImage img = QImage((uchar*)(src.data),src.cols,src.rows,src.step,QImage::Format_RGB888);
        ui->lbl_res_pic->setPixmap(QPixmap::fromImage(img));
        ui->lbl_res_pic->resize(ui->lbl_res_pic->pixmap()->size());

        if(ui->checkBox->isChecked())
        {
            QString savePath=QString("/home/pi/Desktop/softward/code/test_opencv/LiteTest/res_img/resImg_%1.png").arg(num);
            img.save(savePath);
            qDebug() << savePath;
        }
        num++;
        filename.clear();
    }
//    }else if (mime.name().startsWith("video/")) {
//        capture->open(filename.toLatin1().data());
//        if (!capture->isOpened()){
//            ui->txt_message->append("fail to open MP4!");
//            return;
//        }
//        IsDetect_ok +=1;
////        if (IsDetect_ok ==2)
////            ui->startdetect->setEnabled(true);
//        ui->txt_message->append(QString::fromUtf8("Open video: %1 succesfully!").arg(filename));

//        //获取整个帧数QStringLiteral
//        long totalFrame = capture->get(cv::CAP_PROP_FRAME_COUNT);
//        int width = capture->get(cv::CAP_PROP_FRAME_WIDTH);
//        int height = capture->get(cv::CAP_PROP_FRAME_HEIGHT);
//        ui->txt_message->append(QStringLiteral("整个视频共 %1 帧, 宽=%2 高=%3 ").arg(totalFrame).arg(width).arg(height));
//        ui->lbl_res_pic->resize(QSize(width, height));

//        //设置开始帧()
//        long frameToStart = 0;
//        capture->set(cv::CAP_PROP_POS_FRAMES, frameToStart);
//        ui->txt_message->append(QStringLiteral("从第 %1 帧开始读").arg(frameToStart));

//        //获取帧率
//        double rate = capture->get(cv::CAP_PROP_FPS);
//        ui->txt_message->append(QStringLiteral("帧率为: %1 ").arg(rate));
//    }
}

void MainWidget::on_pbn_load_model_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,QStringLiteral("打开文件"),"/home/pi/Desktop/softward/code/test_opencv/LiteTest/onnx","*.onnx");
    if(!QFile::exists(filePath)){
        return;
    }
    std::string model_path=filePath.toStdString();

    if (yolo_model.inityolo(model_path)) {
            ui->txt_message->append("模型加载成功");
            //提取文件名并显示在TextEdit上
            QFileInfo fileInfo(filePath);
            QString fileName=fileInfo.fileName();
            ui->fileName_tet->setText(fileName);
            ui->fileName_tet->show();
            ui->openfile->setEnabled(true);
            ui->pbn_start_grab->setEnabled(true);

        }
    else
    {
        ui->txt_message->append("模型加载失败");
    }
}

//获取图像信息
void MainWidget::getPtr()
{
    // 触发模式标记一下，切换触发模式时先执行停止采集图像函数
    m_bContinueStarted = 1;

    if (m_nTriggerMode == TRIGGER_ON) {
        // 开始采集之后才创建workthread线程

            //开启相机采集
            m_pcMyMainCamera[0]->StartGrabbing();
            int camera_Index=0;
            if (camera_Index == 0) {
                myThread_Camera_Mainshow->getCameraPtr(
                    m_pcMyMainCamera[0]); //线程获取左相机指针
                myThread_Camera_Mainshow->getImagePtr(
                    myImage_Main); //线程获取图像指针
                myThread_Camera_Mainshow->getCameraIndex(0); //相机 Index==0

                if (!myThread_Camera_Mainshow->isRunning()) {
                    myThread_Camera_Mainshow->start();
                    m_pcMyMainCamera[0]->softTrigger();
                    m_pcMyMainCamera[0]->ReadBuffer(*myImage_Main); //读取Mat格式的图像
                }
            }
        }
}
//在主界面显示
void MainWidget::display_myImage_Main(const cv::Mat *imagePrt)
{


    cv::Mat rgb;
    cv::cvtColor(*imagePrt, rgb, CV_BGR2RGB);

    QImage QmyImage_L;
    QmyImage_L = QImage((const unsigned char *)(rgb.data), rgb.cols,
                            rgb.rows, QImage::Format_RGB888);

    QmyImage_L = (QmyImage_L)
                     .scaled(ui->lbl_res_pic->size(), Qt::IgnoreAspectRatio,
                             Qt::SmoothTransformation); //饱满填充
    //显示图像
    //ui->lbl_camera_L->setPixmap(QPixmap::fromImage(QmyImage_L));
    ui->lbl_res_pic->setPixmap(QPixmap::fromImage(QmyImage_L));
}

void MainWidget::on_checkBox_stateChanged(int arg1)
{
    if(ui->checkBox->isChecked())
    {
        ui->txt_message->append("您选择了保存结果，保存路径为/home/pi/Desktop/softward/code/test_opencv/LiteTest/res_img");
    }
}


