#ifndef YOLOTHREAD_H
#define YOLOTHREAD_H

#include <QObject>
#include <QThread>
#include <QString>
#include "yolo.h"
#include "yolov5s.h"

extern QString modelPath; // 模型路径
extern QString imagePath; // 图片路径
extern QString videoPath; // 视频路径
extern bool v5liteFlag; // 是否使用v5lite模型

class yoloThread : public QObject
{
    Q_OBJECT
public:
    explicit yoloThread(QObject *parent = nullptr);
    void detect(const cv::Mat* framePtr, cv::Mat frame);
    //QTimer          *timer;

private:
    Yolo *yolov5lite = nullptr; // 初始化v5lite模型
    Yolov5s *yolov5 = nullptr; // 初始化v5模型

signals:
    void image(cv::Mat img);
    void imageBoxInfo(std::vector<BoxInfo_v5lite>);
    void BoxInfoMessage(QString message);
    void boxinfo(QVariant info);
    void boxInfoUpdated(const std::vector<BoxInfo_v5lite>& boxInfoList);

public slots:
    void slotinitYolov5();
    void slotinitYolov5lite();
};

#endif // YOLOTHREAD_H
