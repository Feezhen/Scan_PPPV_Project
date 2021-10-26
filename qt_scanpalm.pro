QT       += core gui
QT       += sql


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG+=debug_and_release
CONFIG(debug, debug|release){
    TARGET = qt_scanpalm_debug
} else {
    TARGET = qt_scanpalm_release
}

#TARGET = qt_scanpalm_bak
TEMPLATE = app
QMAKE_CXXFLAGS += -fopenmp
INCLUDEPATH += /usr/local/include/opencv4 \
               /home/pi/Palm/qt_scanpalm_backup/NCNN/include/ncnn \
               /home/pi/Palm/qt_scanpalm_backup/include
               

LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_videoio -lopencv_imgproc -lopencv_objdetect
LIBS += -lwiringPi
LIBS += /home/pi/Palm/qt_scanpalm_backup/NCNN/lib/libncnn.a
LIBS += -fopenmp
LIBS += /lib/libMVSDK.so
SOURCES += main.cpp\
           Camera.cpp\
           dialog.cpp\
           utils.cpp\
           measure_dis.cpp\
           gabor.cpp\
           ncnn_recognition/recognition.cpp\
           MCP/commonFunc.cpp\
           MCP/FuzzyEnhancement.cpp\
           MCP/ImgPreprocess.cpp\
           MCP/ImgSegment.cpp\
           MCP/PointFeature.cpp

HEADERS  += dialog.h\
            Camera.h\
            utils.h\
            measure_dis.h\
            gabor.h\
            ncnn_recognition/recognition.h\
            MCP/commonFunc.h\
            MCP/FuzzyEnhancement.h\
            MCP/ImgPreprocess.h\
            MCP/ImgSegment.h\
            MCP/PointFeature.h

FORMS    += dialog.ui
