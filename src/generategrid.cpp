/*
====================================================================================

File: generategrid.cpp
Description: Algorithm to generate the list of points at which to calculate the
    velocity whether a mask has been loaded or not.
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

#include "generategrid.h"

#include <QColor>
#include <QImage>
#include <iostream>
#include <QPoint>
#include <QList>
#include <QRect>

#include "settings.h"

QList<QPoint> generateGrid(QImage *maskData, Settings *settings)
{
    QList<QPoint> grid;

    int i, j, m, n, count;

    int cAlpha;
    QColor color;
    QRect rect = settings->roi();

    int xSpacing = settings->deltaX();
    int ySpacing = settings->deltaY();

    int _intLengthX = settings->intLengthX();
    int _intLengthY = settings->intLengthY();

    // If any portion of any interrogation window has a value which is not transparent then
    // that interrogation window is not included in the list to be computed.
    for (i = rect.top(); i <= (rect.bottom() - _intLengthY); i = i + ySpacing)
    {
        for (j = rect.left(); j <= (rect.right() - _intLengthX); j = j + xSpacing)
        {
            if (!maskData->isNull())
            {
                count = 0;
                for (m = i; m < (i + _intLengthY); m++)
                {
                    for (n = j; n < (j + _intLengthX); n++)
                    {
                        cAlpha = color.fromRgba(maskData->pixel(n,m)).alpha();
                        if (cAlpha != 0) count++;
                    }
                }
                if (count == 0) grid.append(QPoint(j,i));
            }
            else
            {
                grid.append(QPoint(j,i));
            }
        }
    }

    return grid;
}

QList<QPoint> generateGrid(Settings *settings)
{
    QList<QPoint> grid;

    int i, j;

    QRect rect = settings->roi();

    int xSpacing = settings->deltaX();
    int ySpacing = settings->deltaY();

    int _intLengthX = settings->intLengthX();
    int _intLengthY = settings->intLengthY();

    // Assigning the entire image to be calculated assuming no mask
    for (i = rect.top(); i <= (rect.bottom() - _intLengthY); i = i + ySpacing)
    {
        for (j = rect.left(); j <= (rect.right() - _intLengthX); j = j + xSpacing)
        {
            grid.append(QPoint(j,i));
        }
    }

    return grid;
}
