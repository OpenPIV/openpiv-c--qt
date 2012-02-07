!macx:!win32 {
    LIBS += -lfftw3
}

macx {
    LIBS += -L/opt/local/lib -lfftw3
    INCLUDEPATH += /opt/local/include
}

win32 {
    LIBS += -lfftw3-3
}
