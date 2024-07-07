#include "mythread.h"

MyThread::MyThread()
{
}

MyThread::~MyThread()
{
    terminate();
    if (cameraPtr != NULL) {
        delete cameraPtr;
    }
    if (imagePtr != NULL) {
        delete imagePtr;
    }
}

void MyThread::getCameraPtr(CMvCamera *camera)
{
    cameraPtr = camera;
}

void MyThread::getImagePtr(cv::Mat *image)
{
    imagePtr = image;
}

void MyThread::getCameraIndex(int index)
{
    cameraIndex = index;
}

void MyThread::run()
{
    if (cameraPtr == NULL) {
        return;
    }

    if (imagePtr == NULL) {
        return;
    }


    while (!isInterruptionRequested()) {
        std::cout << "Thread_Trigger:" << cameraPtr->softTrigger() << std::endl;
        std::cout << "Thread_Readbuffer:" << cameraPtr->ReadBuffer(*imagePtr)
                  << std::endl;
        //cameraPtr->ReadBuffer(*imagePtr2);
        emit mess();
        emit Display(imagePtr); //发送信号lbl_camera_L接收并显示

        emit GetPtr(imagePtr); //发送信号yolo进行检测并显示


        msleep(30);
    }
}

