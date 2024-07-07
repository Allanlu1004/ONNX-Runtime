#ifndef MYTHREAD_H
#define MYTHREAD_H

#include "QThread"
#include "MvCamera.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <QTimer>

class MyThread :public QThread
{
        Q_OBJECT

public:
        MyThread();
        ~MyThread();

        void run();
        void getCameraPtr(CMvCamera* camera);
        void getImagePtr(cv::Mat* image);
        void getCameraIndex(int index);

signals:
        void mess();
        void Display(const cv::Mat* image);

        //发送获取图像处理的信号
        void GetPtr(cv::Mat* image2);

private:
        CMvCamera* cameraPtr = NULL;
        cv::Mat* imagePtr = NULL;
        int cameraIndex ;
        int TriggerMode;

        QTimer *time;

};

#endif // MYTHREAD_H
