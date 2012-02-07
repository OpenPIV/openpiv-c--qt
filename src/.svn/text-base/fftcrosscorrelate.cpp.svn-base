#include <fftw3.h>
#include <iostream>
#include <fstream>
#include <QPoint>
#include <QList>
#include <QMutex>
#include "fftcrosscorrelate.h"
#include "imagedata.h"

#include "gaussiansubpixel.h"

FFTCrossCorrelate::FFTCrossCorrelate(Settings *settingsPass, QList<QPoint> gridPass)
    : PivEngine(settingsPass, gridPass)
{
    _intLengthX = PivEngine::intLengthX();
    _intLengthY = PivEngine::intLengthY();
    mutex.lock();
    prepareFFT();
    mutex.unlock();
}

FFTCrossCorrelate::~FFTCrossCorrelate()
{
    mutex.lock();
    fftw_free(in);
    fftw_free(inRot);
    fftw_free(out);
    fftw_free(outRot);

    fftw_destroy_plan(goA);
    fftw_destroy_plan(goB);
    fftw_destroy_plan(goIfft);
    mutex.unlock();
}

void FFTCrossCorrelate::prepareFFT()
{
    int n = _intLengthX * 2 * _intLengthY * 2;

    in = (fftw_complex*) fftw_malloc ( sizeof (fftw_complex) * n );
    inRot = (fftw_complex*) fftw_malloc ( sizeof (fftw_complex) * n );
    out = (fftw_complex*) fftw_malloc ( sizeof (fftw_complex) * n );
    outRot = (fftw_complex*) fftw_malloc ( sizeof (fftw_complex) * n );

    goA = fftw_plan_dft_2d((2*_intLengthX),(2*_intLengthY),in,out,FFTW_FORWARD,FFTW_ESTIMATE);
    goB = fftw_plan_dft_2d((2*_intLengthX),(2*_intLengthY),inRot,outRot,FFTW_FORWARD,FFTW_ESTIMATE);
    goIfft = fftw_plan_dft_2d((2*_intLengthX),(2*_intLengthY),in,out,FFTW_BACKWARD,FFTW_ESTIMATE);
}

bool FFTCrossCorrelate::crossCorrelate(int topLeftRow, int topLeftColumn)
{   
    if (this->imagesAvailable())
    {
        fftCore(this->imageA(),this->imageB(),topLeftRow,topLeftColumn);
        return true;
    }
    else return false;
}

void FFTCrossCorrelate::fftCore(ImageData *_imageA, ImageData *_imageB, int topLeftRow, int topLeftColumn)
{
    int i, j;
    int imageI, imageJ;
    int windowI, windowJ;

    // Interrogation windows
    int n = _intLengthX * 2 * _intLengthY * 2;

    // Getting the average intensity of both interrogation windows
    double meanA;
    double meanB;
//    int count = 0;

    int imageWidth = _imageA->width();
//    unsigned char* aData = _imageA->buffer();
//    unsigned char* bData = _imageB->buffer();
    double* aData = _imageA->buffer();
    double* bData = _imageB->buffer();

//    for (imageI = topLeftRow; imageI < (topLeftRow + _intLengthY); imageI++)
//    {
//        for (imageJ = topLeftColumn; imageJ < (topLeftColumn + _intLengthX); imageJ++)
//        {
//            meanA = meanA + double(aData[imageWidth*imageI + imageJ]);
//            meanB = meanB + double(bData[imageWidth*imageI + imageJ]);
//            count++;
//        }
//    }
//    meanA = meanA / count;
//    meanB = meanB / count;

    meanA = this->meanImageA();
    meanB = this->meanImageB();

    for (windowI = 0; windowI < (_intLengthY*2); windowI++)
    {
        imageI = topLeftRow + windowI;
        for (windowJ = 0; windowJ < (_intLengthX*2); windowJ++)
        {
            imageJ = topLeftColumn + windowJ;
            if ((windowI >= _intLengthY) || (windowJ >= _intLengthX))
            {
                in[_intLengthX*2*windowI + windowJ][0] = 0.0;
                in[_intLengthX*2*windowI + windowJ][1] = 0.0;
                inRot[_intLengthX*2*windowI + windowJ][0] = 0.0;
                inRot[_intLengthX*2*windowI + windowJ][1] = 0.0;
            }
            else
            {
                //in[_intLengthX*2*windowI + windowJ][0] = double(aData[imageWidth*imageI + imageJ]) - meanA;
                in[_intLengthX*2*windowI + windowJ][0] = aData[imageWidth*imageI + imageJ] - meanA;
                in[_intLengthX*2*windowI + windowJ][1] = 0.0;
                //inRot[_intLengthX*2*windowI + windowJ][0] = double(bData[imageWidth*(topLeftRow + _intLengthY - windowI - 1) + (topLeftColumn + _intLengthX - windowJ - 1)]) - meanB;
                inRot[_intLengthX*2*windowI + windowJ][0] = bData[imageWidth*(topLeftRow + _intLengthY - windowI - 1) + (topLeftColumn + _intLengthX - windowJ - 1)] - meanB;
                inRot[_intLengthX*2*windowI + windowJ][1] = 0.0;
            }
        }
    }

    fftw_execute(goA);
    fftw_execute(goB);

    double a, b, c, d;
    for (i = 0; i < n; i++)
    {
            a = out[i][0];
            b = out[i][1];
            c = outRot[i][0];
            d = outRot[i][1];

            in[i][0] = a * c - b * d;
            in[i][1] = b * c + a * d;
    }

    fftw_execute(goIfft);

    for (i = 0; i < (2*_intLengthY); i++)
    {
        for (j = 0; j < (2*_intLengthX); j++)
        {
            this->cmap()[_intLengthX*2*i + j] = out[_intLengthX*2*i + j][0];
        }
    }
}
