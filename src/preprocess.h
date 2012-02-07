/*
====================================================================================

File: preprocess.h
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

#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <QObject>
#include <QImage>
#include <QList>
#include <QPoint>
#include "imagedata.h"

class PreProcess : public QObject
{
    Q_OBJECT

    public:
        PreProcess();
        virtual ~PreProcess();

        QList<QPoint> getPointList();

    public slots:
        QList<QPoint> generateGrid(ImageData *image, QImage *mask);

    private:
        QList<QPoint> pointList;

};

#endif // PREPROCESS_H
