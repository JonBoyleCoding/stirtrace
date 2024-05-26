#-------------------------------------------------
#
# Project created by QtCreator 2013-11-08T09:58:29
#
#-------------------------------------------------

QT       += core gui

TARGET = QT_StirTrace
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imgproc.cpp \
    filehandler.cpp \
    ddrawreader.cpp \
    ddrawwriter.cpp \
    preprocessing.cpp \
    evaluation.cpp

HEADERS  += mainwindow.h \
    imgproc.h \
    filehandler.h \
    ddrawreader.h \
    ddrawwriter.h \
    IBinaryFileReader.h \
    IBinaryFileWriter.h \
    preprocessing.h \
    evaluation.h

FORMS    += mainwindow.ui


win32 {
    INCLUDEPATH += "C:/opencv_mingw/include"
        LIBS += "C:/opencv_mingw/bin/libopencv_calib3d231.dll"
    LIBS += "C:/opencv_mingw/bin/libopencv_contrib231.dll"
    LIBS += "C:/opencv_mingw/bin/libopencv_core231.dll"
    LIBS += "C:/opencv_mingw/bin/libopencv_features2d231.dll"
    LIBS += "C:/opencv_mingw/bin/libopencv_flann231.dll"
    LIBS += "C:/opencv_mingw/bin/libopencv_gpu231.dll"
    LIBS += "C:/opencv_mingw/bin/libopencv_highgui231.dll"
    LIBS += "C:/opencv_mingw/bin/libopencv_imgproc231.dll"
    LIBS += "C:/opencv_mingw/bin/libopencv_legacy231.dll"
    LIBS += "C:/opencv_mingw/bin/libopencv_ml231.dll"
    LIBS += "C:/opencv_mingw/bin/libopencv_objdetect231.dll"
    LIBS += "C:/opencv_mingw/bin/libopencv_video231.dll"
}

macx {
    INCLUDEPATH += "/usr/local/include/opencv2/"
    LIBS += "/usr/local/lib/libopencv_contrib.dylib"
    LIBS += "/usr/local/lib/libopencv_core.dylib"
    LIBS += "/usr/local/lib/libopencv_features2d.dylib"
    LIBS += "/usr/local/lib/libopencv_flann.dylib"
    LIBS += "/usr/local/lib/libopencv_core.dylib"
    LIBS += "/usr/local/lib/libopencv_highgui.dylib"
    LIBS += "/usr/local/lib/libopencv_imgproc.dylib"
    LIBS += "/usr/local/lib/libopencv_legacy.dylib"
    LIBS += "/usr/local/lib/libopencv_ml.dylib"
    LIBS += "/usr/local/lib/libopencv_objdetect.dylib"
    LIBS += "/usr/local/lib/libopencv_ts.dylib"
    LIBS += "/usr/local/lib/libopencv_video.dylib"
}

unix:!macx {
    INCLUDEPATH += "/usr/include/opencv2/"
    LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_contrib.so"
    LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_core.so"
    LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_features2d.so"
    LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_flann.so"
    LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_core.so"
    LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_highgui.so"
    LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_imgproc.so"
    LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_legacy.so"
    LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_ml.so"
    LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_objdetect.so"
#    LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_ts.so"
    LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_video.so"
    LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_calib3d.so"
}
