#include"yolov5.h"
using namespace std;
using namespace cv;
using namespace cv::dnn;

void YoloV5::LetterBox(const cv::Mat& image, cv::Mat& outImage, cv::Vec4d& params, const cv::Size& newShape,
    bool autoShape, bool scaleFill, bool scaleUp, int stride, const cv::Scalar& color)
{
    Size shape = image.size();
    float r = std::min((float)newShape.width / (float)shape.width,
                       (float)newShape.height / (float)shape.height); // 选出较小的缩放比，否则会超过

    float ratio[2]{r, r};

    int new_up_pad[2] ={(int)round((float)shape.width * r),
                        (int)round((float)shape.height * r)}; // 缩放后与目标长宽可能还差一点

    auto dw = (float)(newShape.width - new_up_pad[0]);// 算出与目标长宽差多少
    auto dh = (float)(newShape.height - new_up_pad[1]);
    dw /= 2.0f;
    dh /= 2.0f;

    if (shape.width != new_up_pad[0] && shape.height != new_up_pad[1])//等比例缩放
    {
        resize(image, outImage, Size(new_up_pad[0], new_up_pad[1]));
    }
    else {
        outImage = image.clone();
    }

    int top = int(round(dh - 0.1f)); // 四周用0来填充
    int bottom = int(round(dh + 0.1f));
    int left = int(round(dw - 0.1f));
    int right = int(round(dw + 0.1f));
    params[0] = ratio[0];
    params[1] = ratio[1];
    params[2] = left;
    params[3] = top;
    copyMakeBorder(outImage, outImage, top, bottom, left, right,BORDER_CONSTANT,color);
}



bool YoloV5::readModel(Net &net, string &netPath,bool isCuda = false) {
    try {
        net = readNetFromONNX(netPath);
    }
    catch (const std::exception&) {
        return false;
    }
    //cuda
    if (isCuda) {
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
    }
    //cpu
    else {
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        qDebug() << "use cpu";
    }
    return true;
}

//bool YoloV5::Detect(Mat &SrcImg,Net &net,vector<Output> &output) {
//    Mat blob;
//    int col = SrcImg.cols;
//    int row = SrcImg.rows;
//    int maxLen = MAX(col, row);
//    Mat netInputImg = SrcImg.clone();
////    imshow("netInputImg",netInputImg);
////    cv::waitKey(0);
//    if (maxLen > 1.2*col || maxLen > 1.2*row) {
//        Mat resizeImg = Mat::zeros(maxLen, maxLen, CV_8UC3);
//        SrcImg.copyTo(resizeImg(Rect(0, 0, col, row)));
//        netInputImg = resizeImg;
//    }
//    //blobFromImage(netInputImg, blob, 1 / 255.0, cv::Size(netWidth, netHeight), cv::Scalar(104, 117,123), true, false);
//    blobFromImage(netInputImg, blob, 1 / 255.0, cv::Size(320, 320), cv::Scalar(0, 0,0), true, false);//如果训练集未对图片进行减去均值操作，则需要设置为这句
//    //blobFromImage(netInputImg, blob, 1 / 255.0, cv::Size(netWidth, netHeight), cv::Scalar(114, 114,114), true, false);
//    net.setInput(blob);
//    std::vector<cv::Mat> netOutputImg;
//    //模型推理，模型推理时间
//    auto start = std::chrono::high_resolution_clock::now();
////    vector<string> outputLayerName{"647","669", "691","output" };
////    net.forward(netOutputImg, outputLayerName[3]); //获取output的输出
//    net.forward(netOutputImg, net.getUnconnectedOutLayersNames());
//    auto end = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
//    qDebug() << "单张图片模型推理时间：" << duration << "ms";


//    //接上面
//    std::vector<int> classIds;//结果id数组
//    std::vector<float> confidences;//结果每个id对应置信度数组
//    std::vector<cv::Rect> boxes;//每个id矩形框
//    float ratio_h = (float)netInputImg.rows / netHeight;
//    float ratio_w = (float)netInputImg.cols / netWidth;
//    int net_width = className.size() + 5;  //输出的网络宽度是类别数+5
//    float* pdata = (float*)netOutputImg[0].data;
//    for (int stride = 0; stride < 3; stride++) {    //stride
//        int grid_x = (int)(netWidth / netStride[stride]);
//        int grid_y = (int)(netHeight / netStride[stride]);
//        for (int anchor = 0; anchor < 3; anchor++) { //anchors
//            const float anchor_w = netAnchors[stride][anchor * 2];
//            const float anchor_h = netAnchors[stride][anchor * 2 + 1];
//            for (int i = 0; i < grid_y; i++) {
//                for (int j = 0; j < grid_y; j++) {
//                    float box_score = Sigmoid(pdata[4]);//获取每一行的box框中含有某个物体的概率
//                    if (box_score > boxThreshold) {
//                        //为了使用minMaxLoc(),将85长度数组变成Mat对象
//                        cv::Mat scores(1,className.size(), CV_32FC1, pdata+5);
//                        Point classIdPoint;
//                        double max_class_socre;
//                        minMaxLoc(scores, 0, &max_class_socre, 0, &classIdPoint);
//                        max_class_socre = Sigmoid((float)max_class_socre);
//                        if (max_class_socre > classThreshold) {
//                            //rect [x,y,w,h]
//                            float x = (Sigmoid(pdata[0]) * 2.f - 0.5f + j) * netStride[stride];  //x
//                            float y = (Sigmoid(pdata[1]) * 2.f - 0.5f + i) * netStride[stride];   //y
//                            float w = powf(Sigmoid(pdata[2]) * 2.f, 2.f) * anchor_w;   //w
//                            float h = powf(Sigmoid(pdata[3]) * 2.f, 2.f) * anchor_h;  //h
//                            int left = (x - 0.5*w)*ratio_w;
//                            int top = (y - 0.5*h)*ratio_h;
//                            classIds.push_back(classIdPoint.x);
//                            confidences.push_back(max_class_socre*box_score);
//                            boxes.push_back(Rect(left, top, int(w*ratio_w), int(h*ratio_h)));
//                        }
//                    }
//                    pdata += net_width;//指针移到下一行
//                }
//            }
//        }
//    }
//    vector<int> nms_result;
//    NMSBoxes(boxes, confidences, classThreshold, nmsThreshold, nms_result);
//    for (int i = 0; i < nms_result.size(); i++) {
//        int idx = nms_result[i];
//        Output result;
//        result.id = classIds[idx];
//        result.confidence = confidences[idx];
//        result.box = boxes[idx];
//        output.push_back(result);
//    }

//    if (output.size())
//        return true;
//    else
//        return false;
//}

bool YoloV5::Detect(Mat& SrcImg, Net& net, vector<Output>& output) {
    qDebug() << "in detect";
    Mat blob;
    int col = SrcImg.cols;
    int row = SrcImg.rows;
    int maxLen = MAX(col, row);
    Mat netInputImg = SrcImg.clone();
    Vec4d params;
    LetterBox(SrcImg, netInputImg, params, cv::Size(netWidth, netHeight));
    imshow("netInputImg",netInputImg);
    waitKey(0);
    image_test = netInputImg.clone();
    qDebug() << "netInputImg.width" << netInputImg.cols;
    qDebug() << "netInputImg.height" << netInputImg.rows;
    //图像归一化处理
    blobFromImage(netInputImg, blob, 1 / 255.0, cv::Size(netWidth, netHeight), cv::Scalar(0, 0, 0), true, false);
//    cv::imshow("netInputImg",netInputImg);
//    cv::waitKey(0);
    qDebug() << "1";
    net.setInput(blob);
    qDebug() << "2";
    std::vector<cv::Mat> netOutputImg;
    std::vector<cv::String> names=net.getUnconnectedOutLayersNames();	//输出节点name={"345","403", "461","output" }
    qDebug() << "================================" << QString::fromStdString(names[0]);
    //模型推理，模型推理时间
    auto start = std::chrono::high_resolution_clock::now();
    //net.forward(netOutputImg, net.getUnconnectedOutLayersNames());
    net.forward(netOutputImg, names[0]);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    qDebug() << "单张图片模型推理时间：" << duration << "ms";

//    std::vector<int> classIds;//目标类别数组
//    std::vector<float> confidences;//结果每个目标类别对应置信度数组
//    std::vector<cv::Rect> boxes;//每个目标的矩形框位置信息[x,y,w,h]
//    int net_width = className.size() + 5; //输出维度[25200，85]
//    float* pdata = (float*)netOutputImg[0].data;
//    for (int r = 0; r < netOutputImg[0].size[1]; ++r) { //25200
//        float box_score = pdata[4]; ;//获取每一行的box框中含有某个物体的概率
//        if (box_score >= classThreshold) {
//            cv::Mat scores(1, className.size(), CV_32FC1, pdata + 5);
//            Point classIdPoint;
//            double max_class_socre;
//            minMaxLoc(scores, 0, &max_class_socre, 0, &classIdPoint);
//            max_class_socre = max_class_socre * box_score;
//            if (max_class_socre >= classThreshold) {
//                //坐标还原
//                float x = (pdata[0] - params[2]) / params[0];
//                float y = (pdata[1] - params[3]) / params[1];
//                float w = pdata[2] / params[0];
//                float h = pdata[3] / params[1];
//                qDebug() << "x=" << x;
//                qDebug() << "y=" << y;
//                qDebug() << "w=" << w;
//                qDebug() << "h=" << h;
//                int left = MAX(round(x - 0.5 * w + 0.5), 0);
//                int top = MAX(round(y - 0.5 * h + 0.5), 0);
//                classIds.push_back(classIdPoint.x);
//                confidences.push_back(max_class_socre);
//                boxes.push_back(Rect(left, top, int(w + 0.5), int(h + 0.5)));
//            }
//        }
//        pdata += net_width;

//    }

    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;

    float ratio_h = (float)netInputImg.rows / netWidth; // 此时为1
    float ratio_w = (float)netInputImg.cols / netWidth;
    int net_width = className.size() + 5;
    int net_out_width = netOutputImg[0].size[2];
    CV_Assert(net_out_width == net_width);

    float* pdata = (float*)netOutputImg[0].data;
    int net_height = netOutputImg[0].size[1];
    for (int r = 0; r < net_height; ++r) // 下一个框)
    {
        float box_score = pdata[4];
        if (box_score >= classThreshold)
        {
            Mat scores(1, className.size(), CV_32FC1, pdata+5);
            Point classIdPoint;
            double max_class_score;

            minMaxLoc(scores, 0, &max_class_score,0, &classIdPoint);
            max_class_score = max_class_score * box_score;
            if(max_class_score > classThreshold)
            {
                float norm_x = pdata[0];
                float norm_y = pdata[1];
                float norm_w = pdata[2];
                float norm_h = pdata[3];
                float x = norm_x * 320;
                float y = norm_y * 320;
                float w = norm_w * 320;
                float h = norm_h * 320;
//                float x = ((pdata[0]*netWidth - pdata[2]*netWidth/2) - params[2]) / params[0]; //  缩放、padding后，-》原图
//                float y = ((pdata[1]*netHeight - pdata[3]*netHeight/2) - params[3]) / params[1]; //  params: out // in
//                float w = pdata[2]*netWidth / params[0];
//                float h = pdata[3]*netHeight / params[1];
                qDebug() << "norm_x" << pdata[0];
                qDebug() << "norm_y" << pdata[1];
                qDebug() << "norm_w" << pdata[2];
                qDebug() << "norm_h" << pdata[3];

                qDebug() << "params[0]" << params[0];
                qDebug() << "params[1]" << params[1];
                qDebug() << "params[2]" << params[2];
                qDebug() << "params[3]" << params[3];

                qDebug() << "x=" << x;
                qDebug() << "y=" << y;
                qDebug() << "w=" << w;
                qDebug() << "h=" << h;
//                int left = MAX(round(x - 0.5 * w), 0);
//                int top = MAX(round(y - 0.5*h), 0);
                int left = MAX(round(x), 0);
                int top = MAX(round(y), 0);


                classIds.push_back(classIdPoint.x);
                confidences.push_back(max_class_score);
                boxes.push_back(Rect(left, top, round( w * ratio_w), round(h * ratio_h)));// ??
            }
        }
        pdata += net_width;
    }

    //非最大抑制（NMS）
    vector<int> nms_result;
    NMSBoxes(boxes, confidences, classThreshold, nmsThreshold, nms_result);
    for (int i = 0; i < nms_result.size(); i++) {
        int idx = nms_result[i];
        Output result;
        result.id = classIds[idx];
        result.confidence = confidences[idx];
        result.box = boxes[idx];
        output.push_back(result);
    }
    if (output.size())
        return true;
    else
        return false;

}



//这里的color是颜色数组，对没一个id随机分配一种颜色
void YoloV5::drawPred(Mat &img, vector<Output> result, vector<Scalar> color) {
    qDebug() << result.size();
    for (int i = 0; i < result.size(); i++) {
        int left, top;
        left = result[i].box.x;
        qDebug() << "left" << left;
        top = result[i].box.y;
        int color_num = i;
        //rectangle(img, result[i].box, color[result[i].id], 2, 8);
        rectangle(image_test, result[i].box, Scalar(255, 0, 0), 2, 8);
        //condifence
        std::ostringstream out;
        out.precision(2);
        out << std::fixed << result[i].confidence;
        QString str = QString::fromStdString(out.str());
        string label = className[result[i].id] +":" + str.toStdString();

        int baseLine;
        Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
        top = max(top, labelSize.height);
        //rectangle(frame, Point(left, top - int(1.5 * labelSize.height)), Point(left + int(1.5 * labelSize.width), top + baseLine), Scalar(0, 255, 0), FILLED);
        //putText(img, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 1, color[result[i].id], 2);
        putText(image_test, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 1);
    }
    imshow("resule1",image_test);
    waitKey(0);
}


//void YoloV5::drawPred(Mat& img, vector<Output> result, vector<Scalar> color) {
//    for (int i = 0; i < result.size(); i++) {
//        int left, top;
//        left = result[i].box.x;
//        top = result[i].box.y;
//        int color_num = i;
//        rectangle(img, result[i].box, Scalar(255, 0, 0), 2, 8);
//        //conf保留两位小数显示
//        int conf = (int)std::round(result[i].confidence * 100);
//        string label = className[result[i].id] + ":0." + to_string(conf);

//        int baseLine;
//        Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
//        //top = max(top, labelSize.height);
//        putText(img, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 2);
//    }
//    imshow("detect_img", img);
//    waitKey(0);
//}



void getTime(QString msg)
{
    auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    QString output = msg.arg(elapsed.count());
    string out = output.toStdString();
    cout<<out<<endl;
}
