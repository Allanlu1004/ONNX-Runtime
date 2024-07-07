#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>
#include <QDate>
#include <QDir>

#include "MvCamera.h"
#include "mythread.h"
#include "mainwidget.h"

#define MAX_DEVICE_NUM     2
#define TRIGGER_SOURCE     7
#define EXPOSURE_TIME      40000
#define FRAME              5
#define TRIGGER_ON         1
#define TRIGGER_OFF        0
#define START_GRABBING_ON  1
#define START_GRABBING_OFF 0
#define IMAGE_NAME_LEN     64


QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class MainWidget;
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
signals:
    void closedWid();
    void back();

public:
    CMvCamera *m_pcMyCamera[MAX_DEVICE_NUM]; // 相机指针对象
    MV_CC_DEVICE_INFO_LIST m_stDevList;      // 存储设备列表
    cv::Mat *myImage_L = new cv::Mat(); //保存左相机图像的图像指针对象
    cv::Mat *myImage_R = new cv::Mat(); //保存右相机有图像的图像指针对象
    int devices_num;                    // 设备数量

    MainWidget *mainWid = NULL;           //创建一个存储主界面窗体的指针
public:
    MyThread *myThread_Camera_show = NULL; //相机实时显示线程对象

private slots:
    void on_pbn_enum_camera_clicked();
    void on_pbn_open_camera_clicked();
    void on_rdo_continue_mode_clicked();
    void on_rdo_softigger_mode_clicked();
    void on_pbn_start_grabbing_clicked();
    void on_pbn_stop_grabbing_clicked();
    void on_pbn_software_once_clicked();
    void display_myImage_L(const cv::Mat *imagePrt);
    void display_myImage_Main(const cv::Mat *imagePrt);
    void on_pbn_close_camera_clicked();
    void on_pbn_save_BMP_clicked();
    void on_pbn_save_JPG_clicked();
    void on_le_set_exposure_textChanged(const QString &arg1);
    void on_le_set_gain_textChanged(const QString &arg1);



    void on_pbn_return_main_clicked();

public:
    // 状态
    bool m_bOpenDevice;                  // 是否打开设备
    bool m_bStartGrabbing;               // 是否开始抓图
    int m_nTriggerMode;                  // 触发模式
    int m_bContinueStarted;              // 开启过连续采集图像
    MV_SAVE_IAMGE_TYPE m_nSaveImageType; // 保存图像格式

private:
    QString PrintDeviceInfo(MV_CC_DEVICE_INFO *pstMVDevInfo, int num_index);
    QString m_SaveImagePath;
    void OpenDevices();
    void CloseDevices();
    void SaveImage();
    void saveImage(QString format,int index);

private:

    Ui::Widget *ui;

protected:
    void closeEvent(QCloseEvent *event) override;     //重写关闭事件处理函数
};
#endif // WIDGET_H

