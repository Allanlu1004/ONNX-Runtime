#include "yolo.h"

int main()
{
    cv::Mat img = cv::imread("D:/Qt/code/code7.7/testonnx/sample/bus.jpg");
    cv::imshow("11", img);
    cv::waitKey(0);
    Yolo myYolo;
    std::vector<BoxInfo> imagebox;
    imagebox = myYolo.detect(img);
    cv::imshow("22", img);
    cv::waitKey(0);
}