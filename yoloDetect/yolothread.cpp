#include "yolothread.h"

QString modelPath;
QString imagePath;
QString videoPath;
bool v5liteFlag = false;

yoloThread::yoloThread(QObject *parent)
    : QObject{parent}
{
//    Net_config_v5lite v5lite_nets = { 0.5, 0.6, 0.5 ,"/home/allan/Code/cpp/onnxRuntime+usbCam/yoloDetect/onnxmodels/v5lite-s.onnx", "/home/allan/Code/cpp/onnxRuntime+usbCam/yoloDetect/sample/coco.names",false};
//    //Net_config_v5lite v5lite_nets = {0.5,0.6,0.5,modelPath,"/home/allan/Code/cpp/onnxRuntime+usbCam/yoloDetect/sample/coco.names",v5liteFlag};
//    yolov5lite = new Yolo(v5lite_nets);

//    Configuration v5_nets = { 0.5, 0.6, 0.5,"/home/allan/Code/cpp/onnxRuntime+usbCam/yoloDetect/onnxmodels/yolov5s.onnx" };
//    yolov5 = new Yolov5s(v5_nets);
}


void yoloThread::detect(const cv::Mat* framePtr, cv::Mat frame)
{
    cv::Mat img = *framePtr;

    std::vector<BoxInfo_v5lite> imagebox;
    std::vector<BoxInfo> imageboxs_;

    if(v5liteFlag)
    {
        imagebox = yolov5lite->detect_v5lite(img);

        emit image(img);
    }
    else
    {
        yolov5->detect_v5(img);

        emit image(img);
    }



    QString boxinfoToString;
    for(int i = 0; i < imagebox.size(); i++)
    {
        BoxInfo_v5lite boxinfo = imagebox[i];
        boxinfoToString +=  QString("Box Information num%7:\nLeftTop(x1=%1, y1=%2)\nrightBottom(x2=%3, y2=%4)\nScore=%5, Label=%6")
        .arg(boxinfo.x1)
        .arg(boxinfo.y1)
        .arg(boxinfo.x2)
        .arg(boxinfo.y2)
        .arg(boxinfo.score)
        .arg(boxinfo.label)
        .arg(i);
        boxinfoToString += "\n";
    }
    emit BoxInfoMessage(boxinfoToString);
    emit imageBoxInfo(imagebox);
}

void yoloThread::slotinitYolov5()
{
    Configuration v5_nets = { 0.5, 0.6, 0.5, modelPath.toStdString()};
    yolov5 = new Yolov5s(v5_nets);
    qDebug() << "模型初始化成功";
}

void yoloThread::slotinitYolov5lite()
{
    Net_config_v5lite v5lite_nets = { 0.5, 0.6, 0.5 , modelPath.toStdString(), "/home/allan/Code/cpp/onnxRuntime+usbCam/yoloDetect/sample/coco.names",true};
    yolov5lite = new Yolo(v5lite_nets);
    qDebug() << "模型初始化成功";
}
