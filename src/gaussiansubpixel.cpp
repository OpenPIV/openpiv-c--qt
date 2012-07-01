/*
====================================================================================

File: gaussiansubpixel.cpp
Description: Algorithm to provide subpixel estimation of the displacement.
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

#include <cmath>
#include <iostream>
#include <fstream>
#include "pivdata.h"
#include "gaussiansubpixel.h"

PivPointData gaussianSubPixel(double *cmap, int intLengthX, int intLengthY)
{
    int _intLengthX = intLengthX;
    int _intLengthY = intLengthY;
    int i, j;
    int maxI, maxJ;
    double f0, f1, f2;
    double xDisp, yDisp;
    PivPointData disp;

    // First, find the maximum value in the correlation map
    double max = -1000.0;
    for (i = 0; i < _intLengthY*2; i++)
    {
        for (j = 0; j < _intLengthX*2; j++)
        {
            if (cmap[_intLengthX*2*i + j] > max)
            {
                max = cmap[_intLengthX*2*i + j];
                maxI = i;
                maxJ = j;
            }
        }
    }

    // Need to calculate the average to which to compare the signal
    double mean = 0.0;
    int count = 0;
    for (i = 0; i < _intLengthY*2; i++)
    {
        for (j = 0; j < _intLengthX*2; j++)
        {
            if (!(i < (maxI+1) && i > (maxI-1) && j < (maxJ+1) && j < (maxJ-1)))
            {
                mean += fabs(cmap[_intLengthX*2*i + j]);
                count++;
            }
        }
    }
    mean = mean / double(count);
    // Assign the signal-to-noise (snr) value
    disp.snr = max / mean;

    // Gaussian fitting to the peak in the correlation map
    f0 = log(fabs(cmap[_intLengthX*2*maxI + maxJ]));

    f1 = log(fabs(cmap[_intLengthX*2*(maxI-1) + maxJ]));
    f2 = log(fabs(cmap[_intLengthX*2*(maxI+1) + maxJ]));
    yDisp = double(_intLengthY) - (double(maxI+1) + (f1 - f2) / (2*f1 - 4*f0 + 2*f2));
    // Assign the vertical displacment
    disp.v = yDisp;

    f1 = log(fabs(cmap[_intLengthX*2*maxI + maxJ - 1]));
    f2 = log(fabs(cmap[_intLengthX*2*maxI + maxJ + 1]));
    xDisp = double(_intLengthX) - (double(maxJ+1) + (f1 - f2) / (2*f1 - 4*f0 + 2*f2));
    // Assign the horizontal displacement
    disp.u = xDisp;

    // Note that at this point we know nothing about the location (x,y) of the data point.
    // That value is assigned elsewhere.
    return disp;
}
