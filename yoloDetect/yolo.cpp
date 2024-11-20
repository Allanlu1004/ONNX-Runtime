#include "yolo.h"

Yolo::Yolo(Net_config_v5lite yolo_nets)
{
    //Net_config yolo_nets = { 0.5, 0.6, 0.5 ,"/home/allan/Code/cpp/onnxRuntime+usbCam/yoloDetect/onnxmodels/v5lite-s.onnx", "/home/allan/Code/cpp/onnxRuntime+usbCam/yoloDetect/sample/coco.names" };
    //Net_config yolo_nets = { 0.5, 0.6, 0.5 ,"/home/allan/Code/cpp/onnxRuntime+usbCam/yoloDetect/onnxmodels/yolov5s.onnx", "/home/allan/Code/cpp/onnxRuntime+usbCam/yoloDetect/sample/coco.names" };
    //LogWrite::instance().addEntry("message","load Net config success!");
    inityolo(yolo_nets);
}

std::vector<BoxInfo_v5lite> Yolo::detect_v5lite(cv::Mat &frame)
{
    // 记录开始时间
    int64 start = cv::getTickCount();
    // 图像预处理
    int newh = 0, neww = 0, padh = 0, padw = 0;
    cv::Mat dstimg = this->resize_image(frame, &newh, &neww, &padh, &padw);
    this->normalize_(dstimg);
    //LogWrite::instance().addEntry("message","image preprocessing success!");
    // 构建输入张量
    std::array<int64_t, 4> input_shape_{ 1, 3, this->inpHeight, this->inpWidth };
    auto allocator_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    Ort::Value input_tensor_ = Ort::Value::CreateTensor<float>(allocator_info, input_image_.data(), input_image_.size(), input_shape_.data(), input_shape_.size());

    // 准备输入输出名称
    int inputCount = input_names.size();
    int outputCount = output_names.size();
    std::vector<const char*> inputNamesCStr(inputCount);
    std::vector<const char*> outputNamesCStr(outputCount);
    for (size_t i = 0; i < input_names.size(); ++i) {
        inputNamesCStr[i] = input_names[i].c_str();
    }

    for (size_t i = 0; i < output_names.size(); ++i) {
        outputNamesCStr[i] = output_names[i].c_str();
    }


    // 有问题 需要注意 inputNamesCStr outputNamesCStr
    // 推理
    std::vector<Ort::Value> ort_outputs = ort_session->Run(Ort::RunOptions{ nullptr }, inputNamesCStr.data(), &input_tensor_, 1, outputNamesCStr.data(), output_names.size());
    const float* preds = ort_outputs[0].GetTensorMutableData<float>();


    // 生成检测框
    std::vector<BoxInfo_v5lite> generate_boxes;
    float ratioh = (float)frame.rows / newh, ratiow = (float)frame.cols / neww;
    int n = 0, q = 0, i = 0, j = 0, k = 0; ///xmin,ymin,xamx,ymax,box_score,class_score
    const int nout = this->num_class + 5;
    for (n = 0; n < 3; n++)
    {
        int num_grid_x = (int)(this->inpWidth / this->stride[n]);
        int num_grid_y = (int)(this->inpHeight / this->stride[n]);
        for (q = 0; q < 3; q++)    ///anchor
        {
            const float anchor_w = this->anchors[n][q * 2];
            const float anchor_h = this->anchors[n][q * 2 + 1];
            for (i = 0; i < num_grid_y; i++)
            {
                for (j = 0; j < num_grid_x; j++)
                {
                    float box_score = preds[4];
                    if (box_score > this->objThreshold)
                    {
                        float class_score = 0;
                        int class_ind = 0;
                        for (k = 0; k < this->num_class; k++)
                        {
                            if (preds[k + 5] > class_score)
                            {
                                class_score = preds[k + 5];
                                class_ind = k;
                            }
                        }
                        //if (class_score > this->confThreshold)
                        //{
                        float cx = (preds[0] * 2.f - 0.5f + j) * this->stride[n];  ///cx
                        float cy = (preds[1] * 2.f - 0.5f + i) * this->stride[n];   ///cy
                        float w = powf(preds[2] * 2.f, 2.f) * anchor_w;   ///w
                        float h = powf(preds[3] * 2.f, 2.f) * anchor_h;  ///h

                        float xmin = (cx - padw - 0.5 * w)*ratiow;
                        float ymin = (cy - padh - 0.5 * h)*ratioh;
                        float xmax = (cx - padw + 0.5 * w)*ratiow;
                        float ymax = (cy - padh + 0.5 * h)*ratioh;
                        //qDebug() << "classId" << class_ind;
                        generate_boxes.push_back(BoxInfo_v5lite{ xmin, ymin, xmax, ymax, class_score, class_ind });
                        //}
                    }
                    preds += nout;
                }
            }
        }
    }

    // 非极大抑制
    nms(generate_boxes);
    for (size_t i = 0; i < generate_boxes.size(); ++i)
    {
        int xmin = std::max(1,int(generate_boxes[i].x1));
        int ymin = std::max(1,int(generate_boxes[i].y1));
        int xmax = std::min(frame.cols-1,int(generate_boxes[i].x2));
        int ymax = std::min(frame.rows-1,int(generate_boxes[i].y2));
        cv::rectangle(frame, cv::Point(xmin, ymin), cv::Point(xmax, ymax), cv::Scalar(0, 0, 255), 2);
        std::string label = cv::format("%.2f", generate_boxes[i].score);
        label = this->class_names[generate_boxes[i].label] + ":" + label;
        if(ymin < 10){
            cv::putText(frame, label, cv::Point(xmin + 5, ymin + 20), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0, 255, 0), 1);
        }
        else{
            cv::putText(frame, label, cv::Point(xmin, ymin - 5), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0, 255, 0), 1);
        }

        BoxInfo_v5lite box = generate_boxes[i];
        //QString info1 = QString("pre: xmin=%1, ymin=%2, xmax=%3, ymax=%4, confidenceScore=%5, classID=%6").arg(box.x1).arg(box.y1).arg(box.x2).arg(box.y2).arg(box.score).arg(box.label);
        QString bboxinfo = QString("boxInfo: xmin=%1, ymin=%2, xmax=%3, ymax=%4").arg(xmin).arg(ymin).arg(xmax).arg(ymax);
        QString info = bboxinfo + ", class:" + QString::fromStdString(label);

        LogWrite::instance().addEntry("message",info);
        //LogWrite::instance().addEntry("message",info1);
    }
    
    // 记录结束时间
    int64 end = cv::getTickCount();
    double elapsed_secs = double(end - start) / cv::getTickFrequency();
    QString timeInfo = QString("Inference time=%1ms").arg(elapsed_secs*1000);
    qDebug() << "Inference time:" << elapsed_secs*1000 << "ms";
    LogWrite::instance().addEntry("message",timeInfo);
//    double fps = 1.0 / elapsed_secs;
//    std::string fps_text = "FPS: " + std::to_string(fps);
//    cv::putText(frame, fps_text, cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);
    return generate_boxes;
}

void Yolo::inityolo(Net_config_v5lite config)
{
    // 初始化阈值
    this->confThreshold = config.confThreshold;
    this->nmsThreshold = config.nmsThreshold;
    this->objThreshold = config.objThreshold;

    // 初始化使用哪个模型
    this->v5LiteFlag = config.v5LiteFlag;
    
    // 模型路径处理
    std::string model_path =config.model_path;
    std::wstring widestr = std::wstring(model_path.begin(), model_path.end());
    //OrtStatus* status = OrtSessionOptionsAppendExecutionProvider_CUDA(sessionOptions, 0);
    //Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "YOLOv5Inference");

    // 创建ONNX Runtime会话
    Ort::SessionOptions sessionOptions;
    sessionOptions.SetIntraOpNumThreads(1);
    ort_session = new Ort::Session(env, model_path.c_str(), sessionOptions);
    //Ort::Env env = Env(ORT_LOGGING_LEVEL_ERROR, "yolov5-lite");
    //Ort::SessionOptions sessionOptions;
    //sessionOptions.SetGraphOptimizationLevel(ORT_ENABLE_BASIC);
    //Ort::Session ort_session(env, widestr.c_str(), sessionOptions);

    // 获取输入输出节点信息
    size_t numInputNodes = ort_session->GetInputCount();
    size_t numOutputNodes = ort_session->GetOutputCount();
    Ort::AllocatorWithDefaultOptions allocator;

    // 处理输入节点
    // 迭代获取每个输入节点的名称，并将其存储在 input_names 向量中
    // 获取每个输入节点的类型信息和形状，并将形状存储在 input_node_dims 向量中
    for (int i = 0; i < numInputNodes; i++)
    {
        Ort::AllocatedStringPtr input_name = ort_session->GetInputNameAllocated(i, allocator);
        input_names.push_back(input_name.get());
        Ort::TypeInfo input_type_info = ort_session->GetInputTypeInfo(i);
        auto input_tensor_info = input_type_info.GetTensorTypeAndShapeInfo();
        auto input_dims = input_tensor_info.GetShape();
        input_node_dims.push_back(input_dims);
    }

    // 处理输出节点
    for (int i = 0; i < numOutputNodes; i++)
    {
        Ort::AllocatedStringPtr output_name = ort_session->GetOutputNameAllocated(i, allocator);
        output_names.push_back(output_name.get());
        Ort::TypeInfo output_type_info = ort_session->GetOutputTypeInfo(i);
        auto output_tensor_info = output_type_info.GetTensorTypeAndShapeInfo();
        auto output_dims = output_tensor_info.GetShape();
        output_node_dims.push_back(output_dims);
    }

    // 设置输入图片的宽高
    // 输入张量的形状是 [batch, channels, height, width]
    this->inpHeight = input_node_dims[0][2];
    this->inpWidth = input_node_dims[0][3];
    this->nout = output_node_dims[0][2];      // 5+classes
    this->num_proposal = output_node_dims[0][1];  // pre_box

    // 加载类别文件
    std::string classesFile = config.classesFile;
    std::ifstream ifs(classesFile.c_str());
    std::string line;
    while (getline(ifs, line)) this->class_names.push_back(line);
    this->num_class = class_names.size();

    // 日志文件并输出调试信息
    LogWrite::instance().addEntry("message","yolo init success!");
    qDebug() << "yolo init success!";
}

cv::Mat Yolo::resize_image(cv::Mat srcimg, int *newh, int *neww, int *top, int *left)
{
    int srch = srcimg.rows, srcw = srcimg.cols;
    *newh = this->inpHeight;
    *neww = this->inpWidth;
    cv::Mat dstimg;
    if (this->keep_ratio && srch != srcw) {
        float hw_scale = (float)srch / srcw;
        if (hw_scale > 1) {
            *newh = this->inpHeight;
            *neww = int(this->inpWidth / hw_scale);
            cv::resize(srcimg, dstimg, cv::Size(*neww, *newh), cv::INTER_AREA);
            *left = int((this->inpWidth - *neww) * 0.5);
            cv::copyMakeBorder(dstimg, dstimg, 0, 0, *left, this->inpWidth - *neww - *left, cv::BORDER_CONSTANT, 0);
        }
        else {
            *newh = (int)this->inpHeight * hw_scale;
            *neww = this->inpWidth;
            resize(srcimg, dstimg, cv::Size(*neww, *newh), cv::INTER_AREA);
            *top = (int)(this->inpHeight - *newh) * 0.5;
            copyMakeBorder(dstimg, dstimg, *top, this->inpHeight - *newh - *top, 0, 0, cv::BORDER_CONSTANT, 0);
        }
    }
    else {
        resize(srcimg, dstimg, cv::Size(*neww, *newh), cv::INTER_AREA);
    }
    return dstimg;
}

void Yolo::normalize_(cv::Mat img)
{
    //    img.convertTo(img, CV_32F);
    int row = img.rows;
    int col = img.cols;
    this->input_image_.resize(row * col * img.channels());
    for (int c = 0; c < 3; c++)
    {
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < col; j++)
            {
                float pix = img.ptr<uchar>(i)[j * 3 + 2 - c];
                this->input_image_[c * row * col + i * col + j] = pix / 255.0;
            }
        }
    }
}

void Yolo::nms(std::vector<BoxInfo_v5lite> &input_boxes)
{
    sort(input_boxes.begin(), input_boxes.end(), [](BoxInfo_v5lite a, BoxInfo_v5lite b) { return a.score > b.score; });
    std::vector<float> vArea(input_boxes.size());
    for (int i = 0; i < int(input_boxes.size()); ++i)
    {
        vArea[i] = (input_boxes.at(i).x2 - input_boxes.at(i).x1 + 1)
            * (input_boxes.at(i).y2 - input_boxes.at(i).y1 + 1);
    }

    std::vector<bool> isSuppressed(input_boxes.size(), false);
    for (int i = 0; i < int(input_boxes.size()); ++i)
    {
        if (isSuppressed[i]) { continue; }
        for (int j = i + 1; j < int(input_boxes.size()); ++j)
        {
            if (isSuppressed[j]) { continue; }
            float xx1 = (cv::max)(input_boxes[i].x1, input_boxes[j].x1);
            float yy1 = (cv::max)(input_boxes[i].y1, input_boxes[j].y1);
            float xx2 = (cv::min)(input_boxes[i].x2, input_boxes[j].x2);
            float yy2 = (cv::min)(input_boxes[i].y2, input_boxes[j].y2);

            float w = (cv::max)(float(0), xx2 - xx1 + 1);
            float h = (cv::max)(float(0), yy2 - yy1 + 1);
            float inter = w * h;
            float ovr = inter / (vArea[i] + vArea[j] - inter);

            if (ovr >= this->nmsThreshold)
            {
                isSuppressed[j] = true;
            }
        }
    }
    // return post_nms;
    int idx_t = 0;
    input_boxes.erase(remove_if(input_boxes.begin(), input_boxes.end(), [&idx_t, &isSuppressed](const BoxInfo_v5lite& f) { return isSuppressed[idx_t++]; }), input_boxes.end());
}
