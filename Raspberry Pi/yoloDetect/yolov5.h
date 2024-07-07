#pragma once
#include<iostream>
#include<opencv2/opencv.hpp>
using namespace std;
#include <QString>
#define YOLO_P6 false //是否使用P6模型
#include<math.h>
#include <qdebug.h>

struct Output {
    int id;             //结果类别id
    float confidence;   //结果置信度
    cv::Rect box;       //矩形框
};

class YoloV5 {
public:
    YoloV5() {
    }
    ~YoloV5() {}
    void LetterBox(const cv::Mat& image, cv::Mat& outImage,
        cv::Vec4d& params, //[ratiox,ratioy,dw,dh]
        const cv::Size& newShape = cv::Size(640, 640),
        bool autoShape = false,
        bool scaleFill = false,
        bool scaleUp = true,
        int stride = 32,
        const cv::Scalar& color = cv::Scalar(114, 114, 114));
    bool readModel(cv::dnn::Net& net, std::string& netPath, bool isCuda);
    bool Detect(cv::Mat& SrcImg, cv::dnn::Net& net, std::vector<Output>& output);
    void drawPred(cv::Mat& img, std::vector<Output> result, std::vector<cv::Scalar> color);
    void getTime(QString msg);
    cv::Mat image_test;
private:
    float Sigmoid(float x) {
            return static_cast<float>(1.f / (1.f + exp(-x)));
        }

    const float netAnchors[3][6] = { { 10.0, 13.0, 16.0, 30.0, 33.0, 23.0 },{ 30.0, 61.0, 62.0, 45.0, 59.0, 119.0 },{ 116.0, 90.0, 156.0, 198.0, 373.0, 326.0 } };
        //stride
        const float netStride[3] = { 8.0, 16.0, 32.0 };
        const int netWidth = 320; //网络模型输入大小
        const int netHeight = 320;
        float nmsThreshold = 0.2;
        float boxThreshold = 0.5;
        float classThreshold = 0.5;
public:
    std::vector<std::string> className = { "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
        "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
        "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
        "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
        "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
        "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
        "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone",
        "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear",
        "hair drier", "toothbrush"};
};
