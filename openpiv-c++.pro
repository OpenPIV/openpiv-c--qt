
TEMPLATE = app
TARGET = openpiv-c++

CONFIG *= warn_on
CONFIG *= silent

unix {
  MOC_DIR     = .moc
  OBJECTS_DIR = .obj
  UI_DIR      = .ui
  RCC_DIR     = .rcc
}

win32 {
  MOC_DIR     = _moc
  OBJECTS_DIR = _obj
  UI_DIR      = _ui
  RCC_DIR     = _rcc
}

QMAKE_CXXFLAGS += -std=c++11
QT += xml

DEPENDPATH += . \
    ui \
    share/icons \
    share/images

include( libtiff.pri )
include( fftw.pri )
include( src/src.pri )

FORMS += \
    ui/mainwindow.ui \
    ui/importimages.ui \
    ui/batchwindow.ui
    
RESOURCES += \
    share/icons/icons.qrc \
    share/images/images.qrc




