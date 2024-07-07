#ifndef YOLO_H
#define YOLO_H
#include <fstream>
#include <sstream>
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
//#include <cuda_provider_factory.h>
#include <onnxruntime_cxx_api.h>
#include <qdebug.h>
#include "LogWrite.h"
#include <QMetaType>

struct Net_config
{
    float confThreshold; // Confidence threshold
    float nmsThreshold;  // Non-maximum suppression threshold
    float objThreshold;  //Object Confidence threshold
    std::string model_path;
    std::string classesFile;
};

typedef struct BoxInfo
{
    float x1;
    float y1;
    float x2;
    float y2;
    float score;
    int label;
} BoxInfo;
Q_DECLARE_METATYPE(BoxInfo)




class Yolo
{
public:
    Yolo();
    std::vector<BoxInfo> detect(cv::Mat& frame);
    void inityolo(Net_config config);

private:
    const float anchors[3][6] = { {10.0, 13.0, 16.0, 30.0, 33.0, 23.0}, {30.0, 61.0, 62.0, 45.0, 59.0, 119.0},{116.0, 90.0, 156.0, 198.0, 373.0, 326.0} };
    const float stride[3] = { 8.0, 16.0, 32.0 };
    int inpWidth;
    int inpHeight;
    std::vector<std::string> class_names;
    int num_class;
    float confThreshold;
    float nmsThreshold;
    float objThreshold;

    cv::Mat resize_image(cv::Mat srcimg, int *newh, int *neww, int *top, int *left);
    std::vector<float> input_image_;
    void normalize_(cv::Mat img);
    void nms(std::vector<BoxInfo>& input_boxes);
    const bool keep_ratio = true;
    Ort::Env env = Ort::Env(ORT_LOGGING_LEVEL_ERROR, "yolov5-lite");
    Ort::Session *ort_session = nullptr;
    Ort::SessionOptions sessionOptions = Ort::SessionOptions();
    std::vector<std::string> input_names;
    std::vector<std::string> output_names;
    std::vector<std::vector<int64_t>> input_node_dims; // >=1 outputs
    std::vector<std::vector<int64_t>> output_node_dims; // >=1 outputs
};

#endif // YOLO_H
