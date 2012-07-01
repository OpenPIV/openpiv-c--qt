/*
====================================================================================

File: imagepaint.cpp
Description: This class inherits QGraphicsItem and uses QPainter to draw the
    provided QImage.
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

#include <QObject>
#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>

#include "imagepaint.h"

ImagePaint::ImagePaint(QImage imagePass) : QGraphicsItem()
{
    image = imagePass;
    total = image.rect();
    viewRect = QRectF(total);
}

ImagePaint::~ImagePaint()
{
}

void ImagePaint::setImage(QImage imagePass)
{
    image = imagePass;
    total = image.rect();
    viewRect = QRectF(total);
}

QRectF ImagePaint::boundingRect() const
{
    return QRectF(total);
}

/* This commented out function was used in Version 0.  It remains so that when the 'LUT' function
   is re-implemented in Version 1.0 we don't have to re-invent the wheel. */

/*void ImagePaint::setupPixmap(QString filename, QRectF viewRectPass, SetProcess *settings)
{
    int i, j, imageWidth, imageHeight;
    int red, green, blue;
    int newRed, newGreen, newBlue;
    QRgb rgbValue;
    QColor color;

    image.load(filename);
    if (image.format() == 3) colors = image.colorTable();
    imageHeight = image.height();
    imageWidth = image.width();
    QImage tempImage(imageWidth, imageHeight, QImage::Format_ARGB32);
    for (i = 0; i < imageHeight; i++)
    {
        for (j = 0; j < imageWidth; j++)
        {
            if (image.format() == 3)
            {
                color = QColor(image.pixel(j,i));
                red = color.red();
                green = color.green();
                blue = color.blue();
                newRed = int(settings->getLUT(red));
                newGreen = int(settings->getLUT(blue));
                newBlue = int(settings->getLUT(green));
                rgbValue = qRgb(newRed, newBlue, newGreen);
                tempImage.setPixel(j,i,rgbValue);
            }
            else if (image.format() == 4 || image.format() == 5)
            {
                color = QColor(image.pixel(j,i));
                red = color.red();
                green = color.green();
                blue = color.blue();
                newRed = int(settings->getLUT(red));
                newGreen = int(settings->getLUT(blue));
                newBlue = int(settings->getLUT(green));
                rgbValue = qRgb(newRed, newBlue, newGreen);
                image.setPixel(j,i,rgbValue);
            }
        }
    }
    if (image.format() == 3) image = tempImage;
    viewRect = viewRectPass;
    total = image.rect();
} */

void ImagePaint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawImage(viewRect,image,QRectF(total));
}


