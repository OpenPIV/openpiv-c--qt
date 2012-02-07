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

    //std::ofstream cmapFile;
    //if (output) cmapFile.open("D:/test/cmap.dat");

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
            //if (output) cmapFile << cmap[_intLengthX*2*i + j] << "\t";
        }
        //if (output) cmapFile << "\n";
    }
    //std::cout << "maxI: " << maxI << " maxJ: " << maxJ << std::endl;

    //if (output) cmapFile.close();

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
    disp.snr = max / mean;

    f0 = log(fabs(cmap[_intLengthX*2*maxI + maxJ]));

    f1 = log(fabs(cmap[_intLengthX*2*(maxI-1) + maxJ]));
    f2 = log(fabs(cmap[_intLengthX*2*(maxI+1) + maxJ]));
    yDisp = double(_intLengthY) - (double(maxI+1) + (f1 - f2) / (2*f1 - 4*f0 + 2*f2));
    disp.v = yDisp;

    f1 = log(fabs(cmap[_intLengthX*2*maxI + maxJ - 1]));
    f2 = log(fabs(cmap[_intLengthX*2*maxI + maxJ + 1]));
    xDisp = double(_intLengthX) - (double(maxJ+1) + (f1 - f2) / (2*f1 - 4*f0 + 2*f2));
    disp.u = xDisp;

    return disp;
}
