
TEMPLATE = app
TARGET = openpiv-c++

MOC_DIR =     _build
OBJECTS_DIR = _build
UI_DIR =      _build
RCC_DIR =     _build

DEPENDPATH += . \
    src \
    ui \
    share/icons \
    share/images
INCLUDEPATH += . \
    src

include( libtiff.pri )
include( fftw.pri )

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/imagedata.cpp \
    src/pivdisplay.cpp \
    src/imagepaint.cpp \
    src/imagelist.cpp \
    src/importimages.cpp \
    src/importimagelist.cpp \
    src/maskdropdown.cpp \
    src/itempaint.cpp \
    src/settings.cpp \
    src/processing.cpp \
    src/fftcrosscorrelate.cpp \
    src/pivdata.cpp \
    src/gaussiansubpixel.cpp \
    src/datacontainer.cpp \
    src/filters.cpp \
    src/analysis.cpp \
    src/filteroptions.cpp \
    src/pivengine.cpp \
    src/output.cpp \
    src/vectorlist.cpp \
    src/batchwindow.cpp \
    src/pivthread.cpp \
    src/outputthread.cpp \
    src/colourbar.cpp \
    src/session.cpp \
    src/generategrid.cpp
HEADERS += src/mainwindow.h \
    src/imagedata.h \
    src/pivdisplay.h \
    src/imagepaint.h \
    src/imagelist.h \
    src/importimages.h \
    src/importimagelist.h \
    src/maskdropdown.h \
    src/itempaint.h \
    src/settings.h \
    src/processing.h \
    src/fftcrosscorrelate.h \
    src/pivdata.h \
    src/gaussiansubpixel.h \
    src/datacontainer.h \
    src/filters.h \
    src/analysis.h \
    src/filteroptions.h \
    src/pivengine.h \
    src/output.h \
    src/vectorlist.h \
    src/batchwindow.h \
    src/pivthread.h \
    src/outputthread.h \
    src/colourbar.h \
    src/session.h \
    src/generategrid.h
FORMS += ui/mainwindow.ui \
    ui/importimages.ui \
    ui/batchwindow.ui
RESOURCES += share/icons/icons.qrc \
    share/images/images.qrc




