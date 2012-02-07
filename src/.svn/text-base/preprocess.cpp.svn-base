/*
====================================================================================

File: preprocess.cpp
Description: This class is used to preprocess the data.  The main functionality
    is to provide a list (using QList) of points based on the image and the
    mask image.  If an area of the mask image contains a colour that is not
    alpha it is not included in the list.
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

#include "preprocess.h"
#include <QImage>
#include <QList>
#include <QPoint>
#include "imagedata.h"

PreProcess::PreProcess()
{
}

PreProcess::~PreProcess()
{
}

QList<QPoint> PreProcess::generateGrid(ImageData *image, QImage *mask)
{
    int i, j, m, n, count;

    int cAlpha;

    QColor color;

    int xSpacing = 16;
    int ySpacing = 16;

    int intLength = 32;

    int imageWidth = image->width();
    int imageHeight = image->height();

    // For now work with assumption that they are the same resolution
    for (i = ySpacing; i < imageHeight; i = i + ySpacing)
    {
        for (j = xSpacing; j < imageWidth; j = j + xSpacing)
        {
            count = 0;
            for (m = (i-intLength/2); m < (i+intLength/2); m++)
            {
                for (n = (j-intLength/2); n < (j+intLength/2); n++)
                {
                    cAlpha = color.fromRgba(mask->pixel(n,m)).alpha();
                    if (cAlpha != 0) count++;
                }
            }
            if (count == 0)
            {
                pointList.append(QPoint(j,i));
            }
        }
    }

    return pointList;
}

QList<QPoint> PreProcess::getPointList()
{
    return pointList;
}

