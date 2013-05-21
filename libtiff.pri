!macx:!win32 {
    LIBS += -ltiff
} 

macx {
    LIBS += -L/opt/local/lib -ltiff
    INCLUDEPATH += /opt/local/include
}

win32 {
    LIBS += -ltiff
}
