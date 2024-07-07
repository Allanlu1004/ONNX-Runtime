#include "yolothread.h"

yoloThread::yoloThread(QObject *parent)
    : QObject{parent}
{

}


void yoloThread::detect(cv::Mat frame)
{
    std::vector<BoxInfo> imagebox;
    imagebox = myYolo.detect(frame);
    emit image(frame);

    QString boxinfoToString;
    for(int i = 0; i < imagebox.size(); i++)
    {
        BoxInfo boxinfo = imagebox[i];
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
    //emit imageBoxInfo(imagebox);
}
