QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    LogWrite.cpp \
    camerathread.cpp \
    main.cpp \
    mainwindow.cpp \
    yolo.cpp \
    yolothread.cpp \
    yolov5s.cpp

HEADERS += \
    LogWrite.h \
    camerathread.h \
    mainwindow.h \
    yolo.h \
    yolothread.h \
    yolov5s.h

FORMS += \
    mainwindow.ui

#opencv
INCLUDEPATH += /usr/local/include/opencv4 \
    /usr/local/include/opencv4/opencv2

LIBS += -L/usr/local/lib \
    -lopencv_calib3d \
    -lopencv_core \
    -lopencv_features2d \
    -lopencv_flann \
    -lopencv_highgui \
    -lopencv_imgcodecs \
    -lopencv_imgproc \
    -lopencv_ml \
    -lopencv_objdetect \
    -lopencv_photo \
    -lopencv_stitching \
    -lopencv_videoio \
    -lopencv_video \

#######onnx
INCLUDEPATH += /usr/local/include/onnxruntime
LIBS += -L/usr/local/lib \
    -lonnxruntime



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
