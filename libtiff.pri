
LIBS *= -ltiffxx -ltiff

macx {
    LIBS *= -L/opt/local/lib
    INCLUDEPATH *= /opt/local/include
}
