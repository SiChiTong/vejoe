#-------------------------------------------------
#
# Project created by QtCreator 2016-07-21T18:16:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MachineVision
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    humanfollow.cpp

HEADERS  += mainwindow.h \
    hummanfollow.h

FORMS    += mainwindow.ui

INCLUDEPATH +=  D:\vejoe\opencv\2.4.9_build\install\include \
                D:\vejoe\opencv\2.4.9_build\install\include\opencv \
                D:\vejoe\opencv\2.4.9_build\install\include\opencv2

LIBS += D:\vejoe\opencv\2.4.9_build\lib\libopencv_calib3d249.dll.a \
  D:\vejoe\opencv\2.4.9_build\lib\libopencv_contrib249.dll.a \
  D:\vejoe\opencv\2.4.9_build\lib\libopencv_core249.dll.a \
  D:\vejoe\opencv\2.4.9_build\lib\libopencv_features2d249.dll.a \
  D:\vejoe\opencv\2.4.9_build\lib\libopencv_flann249.dll.a \
  D:\vejoe\opencv\2.4.9_build\lib\libopencv_gpu249.dll.a \
  D:\vejoe\opencv\2.4.9_build\lib\libopencv_highgui249.dll.a \
  D:\vejoe\opencv\2.4.9_build\lib\libopencv_imgproc249.dll.a \
  D:\vejoe\opencv\2.4.9_build\lib\libopencv_legacy249.dll.a \
  D:\vejoe\opencv\2.4.9_build\lib\libopencv_ml249.dll.a \
  D:\vejoe\opencv\2.4.9_build\lib\libopencv_objdetect249.dll.a \
  D:\vejoe\opencv\2.4.9_build\lib\libopencv_video249.dll.a
