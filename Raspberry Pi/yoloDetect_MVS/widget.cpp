#include "widget.h"
#include "./ui_widget.h"
#include "ui_mainwidget.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <QDebug>

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->lbl_camera_L->setPixmap(QPixmap(":/icon/MVS.png"));
    ui->lbl_camera_L->setScaledContents(true);
    //mainWid = static_cast<MainWidget*>(parent);

    //ui->lbl_camera_R->setPixmap(QPixmap("back_img.jpg"));
    //ui->lbl_camera_R->setScaledContents(true);

    // 相机初始化控件
    ui->pbn_enum_camera->setEnabled(true);
    ui->pbn_open_camera->setEnabled(false);
    ui->pbn_close_camera->setEnabled(false);
    ui->cmb_camera_index->setEnabled(false);

    // 图像采集控件
    ui->rdo_continue_mode->setEnabled(false);
    ui->rdo_softigger_mode->setEnabled(false);
    ui->pbn_start_grabbing->setEnabled(false);
    ui->pbn_stop_grabbing->setEnabled(false);
    ui->pbn_software_once->setEnabled(false);

    // 参数控件
    ui->le_set_exposure->setEnabled(false);
    ui->le_set_gain->setEnabled(false);

    // 保存图像控件
    ui->pbn_save_BMP->setEnabled(false);
    ui->pbn_save_JPG->setEnabled(false);

    // 线程对象实例化
    myThread_Camera_show = new MyThread; //相机线程对象


    //发送信号实现页面切换
    connect(ui->pbn_return_main,SIGNAL(clicked()),this,SIGNAL(back()));

    connect(myThread_Camera_show, SIGNAL(Display(const cv::Mat *)), this,
            SLOT(display_myImage_L(const cv::Mat *)));


    // 曝光和增益的输入控件限制值
    QRegExp int_rx("100000|([0-9]{0,5})");
    ui->le_set_exposure->setValidator(new QRegExpValidator(int_rx, this));
    QRegExp double_rx("15|([0-9]{0,1}[0-5]{1,2}[\.][0-9]{0,1})");
    ui->le_set_gain->setValidator(new QRegExpValidator(double_rx, this));


}

Widget::~Widget()
{
    delete ui;
    delete mainWid;
    delete myThread_Camera_show;
    delete myImage_L;
}

//创建关闭子窗体事件，显示主窗体
void Widget::closeEvent(QCloseEvent *event)
{
    emit closedWid();     //发射closed信号
    event->accept();

}


// 枚举相机
void Widget::on_pbn_enum_camera_clicked()
{
    memset(&m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
    int nRet = MV_OK;
    nRet = CMvCamera::EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &m_stDevList);
    if(nRet != MV_OK){
        qDebug() << "Failed to find devices, error code:" << nRet;
    }

    devices_num = m_stDevList.nDeviceNum;
    if (devices_num == 0) {
        QString cameraInfo;
        cameraInfo =
            QString::fromLocal8Bit("暂无设备可连接，请检查设备是否连接正确！");
        ui->lbl_camera_messagee->setText(cameraInfo);
    }
    if (devices_num > 0) {
        QString cameraInfo;
        for (int i = 0; i < devices_num; i++) {
            MV_CC_DEVICE_INFO *pDeviceInfo = m_stDevList.pDeviceInfo[i];
            QString cameraInfo_i = PrintDeviceInfo(pDeviceInfo, i);
            cameraInfo.append(cameraInfo_i);
            cameraInfo.append("\n");
            ui->cmb_camera_index->addItem(QString::number(i+1));
        }
        ui->lbl_camera_messagee->setText(cameraInfo);
        ui->pbn_open_camera->setEnabled(true);
        ui->cmb_camera_index->setEnabled(true);
    }
}

//打印相机的型号、ip等信息
QString Widget::PrintDeviceInfo(MV_CC_DEVICE_INFO *pstMVDevInfo, int num_index)
{
    QString cameraInfo_index;
    cameraInfo_index = QString::fromLocal8Bit("相机序号：");
    cameraInfo_index.append(QString::number(num_index+1));
    cameraInfo_index.append("\t\t");
    // 海康GIGE协议的相机
    if (pstMVDevInfo->nTLayerType == MV_GIGE_DEVICE) {
        int nIp1 =
            ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >>
             24);
        int nIp2 =
            ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >>
             16);
        int nIp3 =
            ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >>
             8);
        int nIp4 =
            (pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

        cameraInfo_index.append(QString::fromLocal8Bit("相机型号："));
        std::string str_name =
            (char *)pstMVDevInfo->SpecialInfo.stGigEInfo.chModelName;
        cameraInfo_index.append(QString::fromStdString(str_name));
        cameraInfo_index.append("\n");
        cameraInfo_index.append(QString::fromLocal8Bit("当前相机IP地址："));
        cameraInfo_index.append(QString::number(nIp1));
        cameraInfo_index.append(".");
        cameraInfo_index.append(QString::number(nIp2));
        cameraInfo_index.append(".");
        cameraInfo_index.append(QString::number(nIp3));
        cameraInfo_index.append(".");
        cameraInfo_index.append(QString::number(nIp4));
        cameraInfo_index.append("\t");
    } else if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE) {
        cameraInfo_index.append(QString::fromLocal8Bit("相机型号："));
        std::string str_name =
            (char *)pstMVDevInfo->SpecialInfo.stUsb3VInfo.chModelName;
        cameraInfo_index.append(QString::fromStdString(str_name));
        cameraInfo_index.append("\n");
    } else {
        cameraInfo_index.append(QString::fromLocal8Bit("相机型号：未知"));
    }
    cameraInfo_index.append(QString::fromLocal8Bit("相机品牌：海康威视"));
    return cameraInfo_index;
}

void Widget::on_pbn_open_camera_clicked()
{
    ui->pbn_open_camera->setEnabled(false);
    ui->pbn_close_camera->setEnabled(true);
    ui->rdo_continue_mode->setEnabled(true);
    ui->rdo_softigger_mode->setEnabled(true);

    ui->rdo_continue_mode->setCheckable(true);
    // 参数据控件
    ui->le_set_exposure->setEnabled(true);
    ui->le_set_gain->setEnabled(true);


    OpenDevices();
}

void Widget::OpenDevices()
{
    int nRet = MV_OK;
    // 创建相机指针对象
    for (unsigned int i = 0, j = 0; j < m_stDevList.nDeviceNum; j++, i++) {
        m_pcMyCamera[i] = new CMvCamera;
        // 相机对象初始化
        m_pcMyCamera[i]->m_pBufForDriver = NULL;
        m_pcMyCamera[i]->m_pBufForSaveImage = NULL;
        m_pcMyCamera[i]->m_nBufSizeForDriver = 0;
        m_pcMyCamera[i]->m_nBufSizeForSaveImage = 0;
        m_pcMyCamera[i]->m_nTLayerType = m_stDevList.pDeviceInfo[j]->nTLayerType;

        unsigned int width = 0;
        unsigned int height = 0;

        nRet = m_pcMyCamera[i]->Open(m_stDevList.pDeviceInfo[j]); //打开相机
        if(nRet != MV_OK){
            qDebug() << "Failed to open camera, error code:" << nRet;
        }
        // 设置图像宽度和高度640×480 设置之前需要将OffsetX OffsetY置零
        nRet = m_pcMyCamera[i]->SetIntValue("OffsetX",0);
        if(nRet != MV_OK){
            qDebug() << "Failed to set OffsetX, error code:" << nRet;
        }

        nRet = m_pcMyCamera[i]->SetIntValue("OffsetY",0);
        if(nRet != MV_OK){
            qDebug() << "Failed to set OffsetY, error code:" << nRet;
        }

        nRet = m_pcMyCamera[i]->SetIntValue("Width", 640);
        if (nRet != MV_OK) {
            qDebug() << "Failed to set Width, error code:" << nRet;
        }

        nRet = m_pcMyCamera[i]->SetIntValue("Height", 480);
        if (nRet != MV_OK) {
            qDebug() << "Failed to set Height, error code:" << nRet;
        }

        // 设置采集帧率 5fps
        nRet = m_pcMyCamera[i]->SetFloatValue("AcquisitionFrameRate", FRAME);
        if (nRet != MV_OK) {
            qDebug() << "Failed to set AcquisitionFrameRate, error code:" << nRet;
        }

        // 设置触发模式
        m_pcMyCamera[i]->setTriggerMode(TRIGGER_ON);
        // 设置触发源为软触发
        m_pcMyCamera[i]->setTriggerSource(TRIGGER_SOURCE);
        // 设置曝光时间,初始为40000，并开启自动曝光模式
        m_pcMyCamera[i]->setExposureTime(EXPOSURE_TIME);
        m_pcMyCamera[i]->SetEnumValue("ExposureAuto",
                                      MV_EXPOSURE_AUTO_MODE_OFF);

        nRet = m_pcMyCamera[i]->GetIntValue("Width", &width);
        if (nRet == MV_OK) {
            qDebug() << "Width:" << width;
        }

        nRet = m_pcMyCamera[i]->GetIntValue("Height", &height);
        if (nRet == MV_OK) {
            qDebug() << "height:" << height;
        }
    }
}

// 连续模式
void Widget::on_rdo_continue_mode_clicked()
{
    ui->pbn_start_grabbing->setEnabled(true);
    m_nTriggerMode = TRIGGER_ON;
}

// 触发模式
void Widget::on_rdo_softigger_mode_clicked()
{
    // 如果开始选择的连续模式，切换到触发模式之前，需要先停止采集
    if (m_bContinueStarted == 1) {
        on_pbn_stop_grabbing_clicked(); //先执行停止采集
    }

    ui->pbn_start_grabbing->setEnabled(false);
    ui->pbn_software_once->setEnabled(true);

    m_nTriggerMode = TRIGGER_OFF;
    for (unsigned int i = 0; i < m_stDevList.nDeviceNum; i++) {
        m_pcMyCamera[i]->setTriggerMode(m_nTriggerMode);
    }
}


void Widget::on_pbn_start_grabbing_clicked()
{
    // 触发模式标记一下，切换触发模式时先执行停止采集图像函数
    m_bContinueStarted = 1;

    ui->pbn_start_grabbing->setEnabled(false);
    ui->pbn_stop_grabbing->setEnabled(true);
    // 保存图像控件
    ui->pbn_save_BMP->setEnabled(true);
    // 图像采集控件
    ui->pbn_save_JPG->setEnabled(true);

    int camera_Index = 0;

    // 先判断什么模式，再判断是否正在采集
    if (m_nTriggerMode == TRIGGER_ON) {
        // 开始采集之后才创建workthread线程
        for (unsigned int i = 0; i < m_stDevList.nDeviceNum; i++) {
            //开启相机采集
            m_pcMyCamera[i]->StartGrabbing();
            camera_Index = i;
            if (camera_Index == 0) {
                myThread_Camera_show->getCameraPtr(
                    m_pcMyCamera[0]); //线程获取左相机指针
                myThread_Camera_show->getImagePtr(
                    myImage_L); //线程获取图像指针
                myThread_Camera_show->getCameraIndex(0); //相机 Index==0

                if (!myThread_Camera_show->isRunning()) {
                    myThread_Camera_show->start();
                    m_pcMyCamera[0]->softTrigger();
                    m_pcMyCamera[0]->ReadBuffer(*myImage_L); //读取Mat格式的图像
                }
            }
        }
    }
}

void Widget::on_pbn_stop_grabbing_clicked()
{
    ui->pbn_start_grabbing->setEnabled(true);
    ui->pbn_stop_grabbing->setEnabled(false);

    for (unsigned int i = 0; i < m_stDevList.nDeviceNum; i++) {
        //关闭相机
        if (myThread_Camera_show->isRunning()) {
            m_pcMyCamera[0]->StopGrabbing();
            myThread_Camera_show->requestInterruption();
            myThread_Camera_show->wait();
        }
    }
}

void Widget::on_pbn_software_once_clicked()
{
    // 保存图像控件
    ui->pbn_save_BMP->setEnabled(true);
    ui->pbn_save_JPG->setEnabled(true);

    if (m_nTriggerMode == TRIGGER_OFF) {
        int nRet = MV_OK;
        for (unsigned int i = 0; i < m_stDevList.nDeviceNum; i++) {
            //开启相机采集
            m_pcMyCamera[i]->StartGrabbing();

            if (i == 0) {
                nRet = m_pcMyCamera[i]->CommandExecute("TriggerSoftware");
                m_pcMyCamera[i]->ReadBuffer(*myImage_L);
                display_myImage_L(myImage_L);       //左相机图像
                display_myImage_Main(myImage_L);    //主界面显示
            }
        }
    }
}

//在相机配置界面显示
void Widget::display_myImage_L(const cv::Mat *imagePrt)
{

    cv::Mat rgb;

    //判断是黑白、彩色图像
    QImage QmyImage_L;
    if (myImage_L->channels() > 1) {
        cv::cvtColor(*imagePrt, rgb, CV_BGR2RGB);
        QmyImage_L = QImage((const unsigned char *)(rgb.data), rgb.cols,
                            rgb.rows, QImage::Format_RGB888);
    } else {
        cv::cvtColor(*imagePrt, rgb, CV_GRAY2RGB);
        QmyImage_L = QImage((const unsigned char *)(rgb.data), rgb.cols,
                            rgb.rows, QImage::Format_RGB888);
    }

    QmyImage_L = (QmyImage_L)
            .scaled(ui->lbl_camera_L->size(), Qt::IgnoreAspectRatio,
                    Qt::SmoothTransformation); //饱满填充

    ui->lbl_camera_L->setPixmap(QPixmap::fromImage(QmyImage_L));

}

//在主界面显示
void Widget::display_myImage_Main(const cv::Mat *imagePrt)
{
    cv::Mat rgb;
    cv::cvtColor(*imagePrt, rgb, CV_BGR2RGB);

    QImage QmyImage_L;
    QmyImage_L = QImage((const unsigned char *)(rgb.data), rgb.cols,
                            rgb.rows, QImage::Format_RGB888);

    QmyImage_L = (QmyImage_L)
                     .scaled(ui->lbl_camera_L->size(), Qt::IgnoreAspectRatio,
                             Qt::SmoothTransformation); //饱满填充
    QDateTime curdatetime;
    if(!QmyImage_L.isNull()){
            curdatetime=QDateTime::currentDateTime();
            QString str=curdatetime.toString("yyyyMMddhhmmss");
            QString filename="/home/pi/Desktop/softward/code/test_opencv/LiteTest/res_img/"+str+".png";
            qDebug()<<"正在保存"<<filename<<"图片，请稍候...";
            /* save(arg1，arg2，arg3)重载函数，arg1 代表路径文件名，
             * arg2 保存的类型，arg3 代表保存的质量等级 */
            QmyImage_L.save(filename,"PNG",-1);
            qDebug()<<"保存完成！";
        }

//    //显示图像
//    this->mainWid=new MainWidget();
//    //ui->lbl_camera_L->setPixmap(QPixmap::fromImage(QmyImage_L));
//    mainWid->ui->lbl_res_pic->setPixmap(QPixmap::fromImage(QmyImage_L));
}

void Widget::CloseDevices()
{
    for (unsigned int i = 0; i < m_stDevList.nDeviceNum; i++) {
        // 关闭线程、相机
        if (myThread_Camera_show->isRunning()) {
            myThread_Camera_show->requestInterruption();
            myThread_Camera_show->wait();
            m_pcMyCamera[0]->StopGrabbing();
        }

        m_pcMyCamera[i]->Close();
    }

    // 关闭之后再枚举一遍
    memset(&m_stDevList, 0,
           sizeof(MV_CC_DEVICE_INFO_LIST)); // 初始化设备信息列表
    int devices_num = MV_OK;
    devices_num =
        CMvCamera::EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE,
                               &m_stDevList); // 枚举子网内所有设备,相机设备数量
}

void Widget::on_pbn_close_camera_clicked()
{
    ui->pbn_open_camera->setEnabled(true);
    ui->pbn_close_camera->setEnabled(false);
    // 图像采集控件
    ui->rdo_continue_mode->setEnabled(false);
    ui->rdo_softigger_mode->setEnabled(false);
    ui->pbn_start_grabbing->setEnabled(false);
    ui->pbn_stop_grabbing->setEnabled(false);
    ui->pbn_software_once->setEnabled(false);
    // 参数控件
    ui->le_set_exposure->setEnabled(false);
    ui->le_set_gain->setEnabled(false);
    // 保存图像控件
    ui->pbn_save_BMP->setEnabled(false);
    ui->pbn_save_JPG->setEnabled(false);

    // 关闭设备，销毁线程
    CloseDevices();
    ui->lbl_camera_messagee->clear();
    ui->lbl_camera_L->clear();
    ui->lbl_camera_L->setPixmap(QPixmap(":/icon/MVS.png"));
    //ui->lbl_camera_R->clear();
}

void Widget::on_pbn_save_BMP_clicked()
{
    m_nSaveImageType = MV_Image_Bmp;
    SaveImage();

}

void Widget::on_pbn_save_JPG_clicked()
{
    m_nSaveImageType = MV_Image_Jpeg;
    SaveImage();
}

void Widget::SaveImage()
{
    // 获取1张图
    MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
    memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
    unsigned int nDataLen = 0;
    int nRet = MV_OK;

    //保存图片的路径
    QString saveDirPath="/home/pi/Desktop/softward/code/test_opencv/LiteTest/saveImage/";
    for (int i = 0; i < devices_num; i++) {

        //保存图像的缓存类指针
        const char *imageName_c_str=NULL;
        // 仅在第一次保存图像时申请缓存，在CloseDevice时释放
        if (NULL == m_pcMyCamera[i]->m_pBufForDriver) {
            unsigned int nRecvBufSize = 0;

            m_pcMyCamera[i]->GetIntValue("PayloadSize", &nRecvBufSize);

            m_pcMyCamera[i]->m_nBufSizeForDriver = nRecvBufSize; // 一帧数据大小

            m_pcMyCamera[i]->m_pBufForDriver =
                (unsigned char *)malloc(m_pcMyCamera[i]->m_nBufSizeForDriver);
        }

        nRet = m_pcMyCamera[i]->GetOneFrameTimeout(
            m_pcMyCamera[i]->m_pBufForDriver, &nDataLen,
            m_pcMyCamera[i]->m_nBufSizeForDriver, &stImageInfo, 1000);
        if (MV_OK == nRet) {
            // 仅在第一次保存图像时申请缓存，在 CloseDevice 时释放
            if (NULL == m_pcMyCamera[i]->m_pBufForSaveImage) {
                // BMP图片大小：width * height * 3 + 2048(预留BMP头大小)
                m_pcMyCamera[i]->m_nBufSizeForSaveImage =
                    stImageInfo.nWidth * stImageInfo.nHeight * 3 + 2048;

                m_pcMyCamera[i]->m_pBufForSaveImage = (unsigned char *)malloc(
                    m_pcMyCamera[i]->m_nBufSizeForSaveImage);
            }
            // 设置对应的相机参数
            MV_SAVE_IMAGE_PARAM_EX stParam = { 0 };
            stParam.enImageType = m_nSaveImageType; // 需要保存的图像类型
            stParam.enPixelType = stImageInfo.enPixelType; // 相机对应的像素格式
            stParam.nBufferSize =
                m_pcMyCamera[i]->m_nBufSizeForSaveImage; // 存储节点的大小
            stParam.nWidth = stImageInfo.nWidth;         // 相机对应的宽
            stParam.nHeight = stImageInfo.nHeight;       // 相机对应的高
            stParam.nDataLen = stImageInfo.nFrameLen;
            stParam.pData = m_pcMyCamera[i]->m_pBufForDriver;
            stParam.pImageBuffer = m_pcMyCamera[i]->m_pBufForSaveImage;
            stParam.nJpgQuality = 90; // jpg编码，仅在保存Jpg图像时有效

            nRet = m_pcMyCamera[i]->SaveImage(&stParam);


            QString image_name;
            //图像名称
            char chImageName[IMAGE_NAME_LEN] = {0};
            if (MV_Image_Bmp == stParam.enImageType) {
                if (i == 0) {
                    snprintf(chImageName, IMAGE_NAME_LEN,
                             "Image_w%d_h%d_fn%d_L.bmp", stImageInfo.nWidth,
                             stImageInfo.nHeight, stImageInfo.nFrameNum);
                    image_name = "Image_w";
                    image_name.append(QString::number(stImageInfo.nWidth));
                    image_name.append("_h");
                    image_name.append(QString::number(stImageInfo.nHeight));
                    image_name.append("_fn");
                    image_name.append(QString::number(stImageInfo.nFrameNum));
                    image_name.append("_L.bmp");
                }
                if (i == 1) {
                    snprintf(chImageName, IMAGE_NAME_LEN,
                             "Image_w%d_h%d_fn%03d_R.bmp", stImageInfo.nWidth,
                             stImageInfo.nHeight, stImageInfo.nFrameNum);
                }

            } else if (MV_Image_Jpeg == stParam.enImageType) {
                if (i == 0) {
                    snprintf(chImageName, IMAGE_NAME_LEN,
                             "Image_w%d_h%d_fn%d_L.jpg", stImageInfo.nWidth,
                             stImageInfo.nHeight, stImageInfo.nFrameNum);
                    image_name = "Image_w";
                    image_name.append(QString::number(stImageInfo.nWidth));
                    image_name.append("_h");
                    image_name.append(QString::number(stImageInfo.nHeight));
                    image_name.append("_fn");
                    image_name.append(QString::number(stImageInfo.nFrameNum));
                    image_name.append("_L.jpg");
                }
                if (i == 1) {
                    snprintf(chImageName, IMAGE_NAME_LEN,
                             "Image_w%d_h%d_fn%03d_R.jpg", stImageInfo.nWidth,
                             stImageInfo.nHeight, stImageInfo.nFrameNum);
                }

            }

            QString imagePath = saveDirPath + image_name;
            QByteArray ba = imagePath.toLatin1();
            imageName_c_str = ba.data();


            FILE *fp = fopen(imageName_c_str, "wb");

            fwrite(m_pcMyCamera[i]->m_pBufForSaveImage, 1, stParam.nImageLen,
                   fp);
            fclose(fp);


//            ui->lbl_camera_R->setPixmap(QPixmap(image_name));
//            ui->lbl_camera_R->setScaledContents(true);
        }
    }
}


void Widget::on_le_set_exposure_textChanged(const QString &arg1)
{
    //设置曝光时间
    QString str = ui->le_set_exposure->text(); // 读取
    int exposure_Time = str.toInt();

    for (int i = 0; i < devices_num; i++) {
        m_pcMyCamera[i]->SetEnumValue("ExposureAuto",
                                      MV_EXPOSURE_AUTO_MODE_OFF);
        m_pcMyCamera[i]->SetFloatValue("ExposureTime", exposure_Time);
    }
}

void Widget::on_le_set_gain_textChanged(const QString &arg1)
{
    QString str = ui->le_set_gain->text(); // 读取
    float gain = str.toFloat();

    for (int i = 0; i < devices_num; i++) {
        m_pcMyCamera[i]->SetEnumValue("GainAuto", 0);
        m_pcMyCamera[i]->SetFloatValue("Gain", gain);
    }
}


void Widget::on_pbn_return_main_clicked()
{
    //发送信号实现页面切换
    //connect(ui->pbn_return_main,SIGNAL(clicked()),this,SIGNAL(back()));
}
