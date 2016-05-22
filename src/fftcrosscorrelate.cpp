/*
====================================================================================

File: fftcrosscorrelate.cpp
Description: Inheriting the PivEngine class the fast and robust FFT
    cross-correlation using the FFTW library is implemented.
Copyright (C) 2010  OpenPIV (http://www.openpiv.net)

Contributors to this code:
Zachary Taylor

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

====================================================================================
*/


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

    // Creation and destruction of FFT objects is not thread-safe
    mutex.lock();
    prepareFFT();
    mutex.unlock();
}

FFTCrossCorrelate::~FFTCrossCorrelate()
{
    // Destruction of FFT objects is not thread-safe
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
    // Initializing fftw variables.  Not thread-safe.
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
    // Checks to see if images are available (provided through the PivEngine class)
    if (this->imagesAvailable())
    {
        // Calling the core FFT algorithm
        fftCore(this->imageA(),this->imageB(),topLeftRow,topLeftColumn);
        return true;
    }
    else return false;
}

void FFTCrossCorrelate::fftCore(ImageData *_imageA, ImageData *_imageB, int topLeftRow, int topLeftColumn)
{
    // Note that the execution of FFTW objects/plans is thread safe (according to FFTW) but the creation
    // and destruction of FFTW objects/plans is not safe.
    int i, j;
    int imageI, imageJ;
    int windowI, windowJ;

    // Interrogation windows
    int n = _intLengthX * 2 * _intLengthY * 2;

    // Getting the average intensity of both interrogation windows
    double meanA;
    double meanB;

    int imageWidth = _imageA->width();
    double* aData = _imageA->buffer();
    double* bData = _imageB->buffer();

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
                in[_intLengthX*2*windowI + windowJ][0] = aData[imageWidth*imageI + imageJ] - meanA;
                in[_intLengthX*2*windowI + windowJ][1] = 0.0;
                // Rotation of the interrogation window B
                inRot[_intLengthX*2*windowI + windowJ][0] = bData[imageWidth*(topLeftRow + _intLengthY - windowI - 1) + (topLeftColumn + _intLengthX - windowJ - 1)] - meanB;
                inRot[_intLengthX*2*windowI + windowJ][1] = 0.0;
            }
        }
    }

    // Computing the 2D FFTs through executing the FFTW plans
    fftw_execute(goA);
    fftw_execute(goB);

    // Complex conjugate multiplication
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

    // Computing the 2D inverse FFT through executing the FFTW plan
    fftw_execute(goIfft);

    // Assigning the result to the cmap object (type double) through the access
    // function provided by the base PivEngine class
    for (i = 0; i < (2*_intLengthY); i++)
    {
        for (j = 0; j < (2*_intLengthX); j++)
        {
            this->cmap()[_intLengthX*2*i + j] = out[_intLengthX*2*i + j][0];
        }
    }
}
