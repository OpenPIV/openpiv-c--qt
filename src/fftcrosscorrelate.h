#ifndef FFTCROSSCORRELATE_H
#define FFTCROSSCORRELATE_H

#include <fftw3.h>
#include <QPoint>
#include <QList>
#include <QMutex>
#include "pivengine.h"
#include "imagedata.h"
#include "settings.h"

class FFTCrossCorrelate : public PivEngine
{

public:
    FFTCrossCorrelate(Settings *settingsPass, QList<QPoint> gridPass);
    virtual ~FFTCrossCorrelate();   

protected:
    bool crossCorrelate(int topLeftRow, int topLeftColumn);
    void prepareFFT();
    void fftCore(ImageData *_imageA, ImageData *_imageB, int topLeftRow, int topLeftColumn);

private:
    int _intLengthX, _intLengthY;

    fftw_plan goA;
    fftw_plan goB;
    fftw_plan goIfft;
    fftw_complex *in;
    fftw_complex *inRot;
    fftw_complex *out;
    fftw_complex *outRot;

    QMutex mutex;
};


#endif // FFTCROSSCORRELATE_H
