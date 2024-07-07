QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    LogWrite.cpp \
    camerathread.cpp \
    main.cpp \
    mainwindow.cpp \
    yolo.cpp \
    yolothread.cpp

HEADERS += \
    LogWrite.h \
    camerathread.h \
    mainwindow.h \
    yolo.h \
    yolothread.h

FORMS += \
    mainwindow.ui


###################################对opencv的支持
INCLUDEPATH += D:\OpenCv\opencv\opencv4.6.0\opencv\build\include
LIBS += -LD:\OpenCv\opencv\opencv4.6.0\opencv\build\x64\vc15\lib \
    -lopencv_world460 \
    -lopencv_world460d \
###################################

#######onnx
INCLUDEPATH += D:\ONNX_Runtime\onnxruntime-win-x64-1.18.0\include
DEPENDPATH += D:\ONNX_Runtime\onnxruntime-win-x64-1.18.0\include
LIBS += -L$$quote(D:\ONNX_Runtime\onnxruntime-win-x64-1.18.0\lib)\
        -lonnxruntime\
        -lonnxruntime_providers_shared



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



