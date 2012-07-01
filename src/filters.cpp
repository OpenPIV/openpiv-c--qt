/*
====================================================================================

File: filters.cpp
Description: File containing the source code of the various filtering algorithms.
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

#include <iostream>
#include <cmath>
#include <QList>
#include "filters.h"

#include "pivdata.h"
#include "filteroptions.h"

const float pi = 3.14159265359;

/*----------SNR-----------------------*/
void snr(PivData *pivData, FilterOptions filterOptions)
{
    double snr_i;

    int width = pivData->width();
    int height = pivData->height();

    double snrThresh = filterOptions.snrThresh();

    /*  Rejecting any point in the PivData where the signal-
        to-noise ratio is below the user defined value.
    */
    int i, j;
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            if (pivData->isValid(i,j))
            {
                snr_i = pivData->data(i,j).snr;
                if (snr_i < snrThresh)
                {
                    pivData->setFilter(i,j,true);
                }
            }
        }
    }
}

/*----------Image Intensity--------------------*/
void imageIntensity(PivData *pivData, FilterOptions filterOptions)
{
    double imageIntensity;

    int width = pivData->width();
    int height = pivData->height();

    double imageThresh = filterOptions.imageThresh();

    /*  Rejecting any point in the PivData where the image intensity
        is below the user defined value.
    */
    int i, j;
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            if (pivData->isValid(i,j))
            {
                imageIntensity = pivData->data(i,j).intensity;
                if (imageIntensity < imageThresh)
                {
                    pivData->setFilter(i,j,true);
                }
            }
        }
    }
}

/*----------Global--------------------*/
void globalRange(PivData *pivData, FilterOptions filterOptions)
{
    double u,v;

    int width = pivData->width();
    int height = pivData->height();

    double umin = filterOptions.umin();
    double umax = filterOptions.umax();
    double vmin = filterOptions.vmin();
    double vmax = filterOptions.vmax();

    /*  Rejecting any point in the PivData where the displacement
        exceeds any of the user-defined global min/max values
    */
    int i, j;
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            if (pivData->isValid(i,j))
            {
                u = pivData->data(i,j).u;
                v = pivData->data(i,j).v;
                if (u > umax || u < umin || v > vmax || v < vmin )
                {
                    pivData->setFilter(i,j,true);
                }
            }
        }
    }
}

void globalStd(PivData *pivData, FilterOptions filterOptions)
{
    double u,v;
    double mxU = 0.0;
    double mxV = 0.0;
    double sxU = 0.0;
    double sxV = 0.0;

    int width = pivData->width();
    int height = pivData->height();

    double nSigma = filterOptions.nStd();

    // First, compute the global average
    int i, j;
    int count = 0;
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            if (pivData->isValid(i,j))
            {
                u = pivData->data(i,j).u;
                v = pivData->data(i,j).v;

                mxU += u;
                mxV += v;

                count++;
            }
        }
    }

    mxU = mxU / double(count);
    mxV = mxV / double(count);

    // Next, compute the standard deviation
    count = 0;
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            if (pivData->isValid(i,j))
            {
                u = pivData->data(i,j).u;
                v = pivData->data(i,j).v;

                sxU += (u - mxU) * (u - mxU);
                sxV += (v - mxV) * (v - mxV);

                count++;
            }
        }
    }

    sxU = sqrt(sxU / double(count));
    sxV = sqrt(sxV / double(count));

    /*  Rejecting any point in the PivData where the displacment
        is farther away than nSigma times the global standard deviation
        from the global average.
    */

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            if (pivData->isValid(i,j))
            {
                u = pivData->data(i,j).u;
                v = pivData->data(i,j).v;

                if (u > (mxU + nSigma*sxU) || u < (mxU - nSigma*sxU) || v > (mxV + nSigma*sxV) || v < (mxV - nSigma*sxV))
                {
                    pivData->setFilter(i,j,true);
                }
            }
        }
    }
}

/*------------Local-------------------*/
void localDetect(PivData *pivData, FilterOptions filterOptions)
{
    double u, v;

    int width = pivData->width();
    int height = pivData->height();

    int i, j, m, n, count;

    int N = filterOptions.localNxN();
    int halfN = floor(double(N - 1) / 2.0);

    double uTol = filterOptions.uTol();
    double vTol = filterOptions.vTol();

    // First need to calculate the central estimator of the local neighbourhood
    switch(filterOptions.localMethod())
    {
    // Calculation of the local mean
    case OpenPIV::LocalMean:
        double mxU, mxV;
        for (i = 0; i < height; i++)
        {
            for (j = 0; j < width; j++)
            {
                u = pivData->data(i,j).u;
                v = pivData->data(i,j).v;

                mxU = 0.0; mxV = 0.0; count = 0;
                for (m = -halfN; m <= halfN; m++)
                {
                    for (n = -halfN; n <= halfN; n++)
                    {
                        if (!pivData->filtered(i+m,j+n))
                        {
                            mxU += pivData->data(i+m,j+n).u;
                            mxV += pivData->data(i+m,j+n).v;
                            count++;
                        }
                    }
                }

                mxU = mxU / double(count);
                mxV = mxV / double(count);

                /*  Rejecting any point in the PivData where the displacment
                    exceeds the tolerance of the local mean.
                */
                if (u > (mxU + uTol) || u < (mxU - uTol) || v > (mxV + vTol) || v < (mxV - vTol))
                {
                    pivData->setFilter(i,j,true);
                }

            }
        }
        break;

    // Calculation of the local median
    case OpenPIV::LocalMedian:
        double medU, medV;
        QList<double> listU;
        QList<double> listV;
        for (i = 0; i < height; i++)
        {
            for (j = 0; j < width; j++)
            {
                listU.clear();
                listV.clear();
                for (m = -halfN; m <= halfN; m++)
                {
                    for (n = -halfN; n <= halfN; n++)
                    {
                        if (!pivData->filtered(i+m,j+n))
                        {
                            listU << pivData->data(i+m,j+n).u;
                            listV << pivData->data(i+m,j+n).v;
                        }
                    }
                }
                qSort(listU.begin(), listU.end());
                qSort(listV.begin(), listV.end());

                medU = listU.value(int(ceil(double(listU.size())/2.0)));
                medV = listV.value(int(ceil(double(listV.size())/2.0)));

                /*  Rejecting any point in the PivData where the displacment
                    exceeds the tolerance of the local median.
                */
                if (u > (medU + uTol) || u < (medU - uTol) || v > (medV + vTol) || v < (medV - vTol))
                {
                    pivData->setFilter(i,j,true);
                }
            }
        }
        break;
    }

}

/*---------Interpolate---------------*/
void meanInterpolate(PivData *pivData, FilterOptions filterOptions)
{
    int width = pivData->width();
    int height = pivData->height();

    double *uTemp, *vTemp;
    uTemp = new double [width*height + 1];
    vTemp = new double [width*height + 1];

    int N = filterOptions.intNxN();
    int halfN = floor(double(N - 1) / 2.0);

    int i, j, m, n;
    double mxU, mxV;
    int count;
    PivPointData pointData;

    // Creating temporary velocity field from original PivData object
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            uTemp[i*width+j] = pivData->data(i,j).u;
            vTemp[i*width+j] = pivData->data(i,j).v;
        }
    }

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            // If the current vector is flagged as erroneous
            if (pivData->filtered(i,j))
            {
                count = 0; mxU = 0.0; mxV = 0.0;
                for (m = -halfN; m <= halfN; m++)
                {
                    for (n = -halfN; n <= halfN; n++)
                    {
                        // Calculate the local average from valid points
                        if (!pivData->filtered(i+m,j+n) && pivData->isValid(i+m,j+n))
                        {
                            mxU += pivData->data(i+m,j+n).u;
                            mxV += pivData->data(i+m,j+n).v;
                            count++;
                        }
                    }
                }
                // Assign the current vector the value of the local mean
                if (count != 0)
                {
                    uTemp[i*width + j] = mxU / double(count);
                    vTemp[i*width + j] = mxV / double(count);
                }
            }
        }
    }


    // Reassign the temporary (new) data to the original object
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            pointData = pivData->data(i,j);
            pointData.u = uTemp[i*width+j];
            pointData.v = vTemp[i*width+j];
            pivData->setData(i,j,pointData);
        }
    }

    // Cleaning up
    delete [] uTemp;
    delete [] vTemp;
}


/*---------- Smoothing --------------*/
void gaussianBlur(PivData *pivData, FilterOptions filterOptions)
{
    int width = pivData->width();
    int height = pivData->height();

    int i, j, k, k1, k2;
    double x, y;

    double *uTemp, *vTemp;

    uTemp = new double [width*height + 1];
    vTemp = new double [width*height + 1];

    double radius = filterOptions.smoothRadius();

    int N = filterOptions.smoothNxN();
    int halfN = floor(double(N - 1) / 2.0);

    double *kernel = new double [N*N+1];
    double sumU, sumV;
    PivPointData pointData;

    // Creation of the kernel
    k = 0; x = 0.0;
    for (y = -double(halfN); y <= double(halfN); y = y + 1.0)
    {
        for (x = -double(halfN); x <= double(halfN); x = x + 1.0)
        {
            kernel[k] = 0.5 / pi / radius / radius * exp(-(x*x + y*y) / 2.0 / radius / radius);
            k++;
        }
    }

    // Convolution
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            sumU = 0.0; sumV = 0.0;
            for (k1 = -halfN; k1 <= halfN; k1++)
            {
                for (k2 = -halfN; k2 <= halfN; k2++)
                {
                    sumU += pivData->data(i-k1,j-k2).u * kernel[(halfN+k1)*N+(halfN+k2)];
                    sumV += pivData->data(i-k1,j-k2).v * kernel[(halfN+k1)*N+(halfN+k2)];
                }
            }
            // Store values to temporary arrays to avoid propogation effects
            uTemp[i*width+j] = sumU;
            vTemp[i*width+j] = sumV;
        }
    }

    // Reassign temporary arrays to original PivData object
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            pointData = pivData->data(i,j);
            pointData.u = uTemp[i*width+j];
            pointData.v = vTemp[i*width+j];
            pivData->setData(i,j,pointData);
        }
    }

    delete [] kernel;
    delete [] uTemp;
    delete [] vTemp;

}
