/*
====================================================================================

File: fftcrosscorrelate.h
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

#ifndef FFTCROSSCORRELATE_H
#define FFTCROSSCORRELATE_H

#include <fftw3.h>
#include <QPoint>
#include <QList>
#include <QMutex>
#include "pivengine.h"
#include "imagedata.h"
#include "settings.h"

/*!
    Inheriting the PivEngine class the fast and robust FFT
    cross-correlation using the FFTW library is implemented.
*/

class FFTCrossCorrelate : public PivEngine
{

public:
    //! Constructor
    /*!
        The basic PivEngine class requires the global settings object and the list of
        grid points to be calculated.

        \param settingsPass is a pointer to the global Settings objeccct
        \param gridPass is a copy of the list of grid points to be computed in the image
    */
    FFTCrossCorrelate(Settings *settingsPass, QList<QPoint> gridPass);
    //! Destructor
    virtual ~FFTCrossCorrelate();   

protected:
    //! Reimplemented from the inherited PivEngine class to provide cross-correlation through FFT
    bool crossCorrelate(int topLeftRow, int topLeftColumn);
    //! Creating the objects necessary for the FFT calculations.
    void prepareFFT();
    //! The core algorithm to calculate the cross-correlation using FFT methods
    /*!
        The algorithm consists of subtracting the mean of each interrogation window, zero-padding
        the interrogation windows, rotation of interrogation window 'B', calculation of the two-
        dimensional FFT of each window, complex conjugate multiplication of the result and the
        inverse FFT.
    */
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
