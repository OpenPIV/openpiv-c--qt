/*
====================================================================================

File: pivengine.cpp
Description: The base class for current and future correlation methods.  The class
    is designed to be thread safe and future developers are encouraged to build on
    this class by inheritance and by reimplementing the crossCorrelate function.
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


#include "pivengine.h"
#include <iostream>

#include <QList>
#include <QPoint>
#include <QMutex>

// Subpixel interpolators
#include "gaussiansubpixel.h"

#include "settings.h"

PivEngine::PivEngine(Settings *settingsPass, QList<QPoint> gridPass)
{
    // Initialization of local private variables
    _intLengthX = settingsPass->intLengthX();
    _intLengthY = settingsPass->intLengthY();
    _detector = settingsPass->detector();
    _cmap = new double [_intLengthX*2*_intLengthY*2+1];
    cmapCreated = true;
    _grid = gridPass;
}

PivEngine::~PivEngine()
{
    // If the correlation map has been created, need to delete it
    if (cmapCreated)
    {
        delete [] _cmap;
        cmapCreated = false;
    }
    // If the images have been created, need to delete them
    if (imagesCreated)
    {
        delete _imageA;
        delete _imageB;
        imagesCreated = false;
    }
}

/* --------- Access functions ----------*/
MetaData PivEngine::data() { return _data; }
int PivEngine::intLengthX() { return _intLengthX; }
int PivEngine::intLengthY() { return _intLengthY; }

QList<QPoint> PivEngine::grid() { return _grid; }

bool PivEngine::imagesAvailable() { return imagesCreated; }
ImageData* PivEngine::imageA() { return _imageA; }
ImageData* PivEngine::imageB() { return _imageB; }
double PivEngine::meanImageA() { return _meanImageA; }
double PivEngine::meanImageB() { return _meanImageB; }

double* PivEngine::cmap() { return _cmap; }
/* --------- Access functions ----------*/

PivData* PivEngine::operator ()(MetaData dataPass)
{
    PivData *pivData;
    pivData = new PivData;

    // First step is to load the images into memory
    _data = dataPass;
    loadImages();

    PivPointData pointData;
    QList<PivPointData> _dataList;
    for (int i = 0; i < _grid.size(); i++)
    {
        // Second step is to find the mean of each interrogation window
        _meanImageA = this->imageMean(_imageA,_grid.value(i).ry(),_grid.value(i).rx());
        _meanImageB = this->imageMean(_imageB,_grid.value(i).ry(),_grid.value(i).rx());

        // Third step is to calculate the displacement (velocity) between the two windows
        pointData = this->velocity(_grid.value(i).ry(),_grid.value(i).rx());

        // The minimum average intensity between the two images in the image pair is recorded
        if (_meanImageA > _meanImageB) pointData.intensity = _meanImageB;
        else pointData.intensity = _meanImageA;

        // Finally the data is appended to the list of data
        _dataList.append(pointData);
    }
    // Cleaning up
    pivData->setList(_dataList);
    delete _imageA;
    delete _imageB;
    imagesCreated = false;

    // Returning the data list
    return pivData;
}

PivPointData PivEngine::velocity(int topLeftRow, int topLeftColumn)
{
    PivPointData pointData;

    // Performing the cross-correlation.  If the operation is unsuccessful the data are set to zeros.
    if (crossCorrelate(topLeftRow, topLeftColumn))
    {
        // Once the correlation map is computed, we estimate the displacement to sub-pixel accuracy
        switch (_detector)
        {
        // Gaussian sub-pixel estimation is currently the most widely used and robust estimator
        case OpenPIV::GaussianSubPixel:
            pointData = gaussianSubPixel(_cmap,_intLengthX,_intLengthY);
            break;
        default:
            break;
        }
    }
    else
    {
        pointData.u = 0.0;
        pointData.v = 0.0;
        pointData.snr = 0.0;
    }

    // Data have not been filtered by this point
    pointData.filtered = false;
    pointData.x = topLeftColumn;
    pointData.y = topLeftRow;

    return pointData;
}

void PivEngine::loadImages()
{
    _imageA = new ImageData;
    _imageB = new ImageData;

    // if (_imageA->read(_data.imageA()) && _imageB->read(_data.imageB())) imagesCreated = true;
    // else
    // {
    //     delete _imageA;
    //     delete _imageB;
    //     imagesCreated = false;
    // }
}

bool PivEngine::crossCorrelate(int topLeftRow, int topLeftColumn)
{
    /*  This is the only necssary function to change when this class is inherited.
        It is called from the PivEngine class and is expected to calculate the correlation
        map by access through this->cmap().
    */
    return false;
}

double PivEngine::imageMean(ImageData *image, int topLeftRow, int topLeftColumn)
{
    // Straightforward two-dimensional mean of the interrogation window beginning at
    // (x,y) = (topLeftColumn, topLeftRow)
    double mean = 0.0;
    int count = 0;
    double* data = image->buffer();
    int imageWidth = image->width();

    int imageI, imageJ;

    for (imageI = topLeftRow; imageI < (topLeftRow + _intLengthY); imageI++)
    {
        for (imageJ = topLeftColumn; imageJ < (topLeftColumn + _intLengthX); imageJ++)
        {
            mean += data[imageWidth*imageI + imageJ];
            count++;
        }
    }
    if (count > 0) mean = mean / count;
    else mean = 0.0;

    return mean;
}
