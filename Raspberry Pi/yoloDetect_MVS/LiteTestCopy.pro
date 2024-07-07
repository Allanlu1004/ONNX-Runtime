QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    LogWrite.cpp \
    MvCamera.cpp \
    main.cpp \
    mainwidget.cpp \
    mythread.cpp \
    widget.cpp \
    yolo.cpp

HEADERS += \
    LogWrite.h \
    MvCamera.h \
    mainwidget.h \
    mythread.h \
    widget.h \
    yolo.h

FORMS += \
    mainwidget.ui \
    widget.ui
QMAKE_LFLAGS += -no-pie
###################################对opencv的支持
INCLUDEPATH += /usr/local/include \
/usr/local/include/opencv4 \
/usr/local/include/opencv4/opencv2\

LIBS += /usr/local/lib/libopencv_calib3d.so \
/usr/local/lib/libopencv_core.so \
/usr/local/lib/libopencv_features2d.so \
/usr/local/lib/libopencv_flann.so \
/usr/local/lib/libopencv_highgui.so \
/usr/local/lib/libopencv_imgcodecs.so \
/usr/local/lib/libopencv_imgproc.so \
/usr/local/lib/libopencv_ml.so \
/usr/local/lib/libopencv_objdetect.so \
/usr/local/lib/libopencv_photo.so \
/usr/local/lib/libopencv_stitching.so \
/usr/local/lib/libopencv_videoio.so \
/usr/local/lib/libopencv_video.so\
/usr/local/lib/libopencv_dnn.so \
###################################

#####海康SDK
INCLUDEPATH += $$PWD/include
LIBS += /usr/local/lib/libMvCameraControl.so

#######onnx
INCLUDEPATH += /usr/local/include/onnxruntime/include/
LIBS += -L/usr/local/lib \
    -lonnxruntime

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    test2.pro.user

RESOURCES += \
    res.qrc
