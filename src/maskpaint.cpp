/*
====================================================================================

File: maskpaint.cpp
Description: This class inherits QGraphicsItem and is used to paint various features
    of the masking on the image.
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

#include "maskpaint.h"

#include <QGraphicsItem>
#include <QPainter>

MaskPaint::MaskPaint() : QGraphicsItem()
{
    total.setRect(0,0,0,0);
    squareSize = 10;
    pointList = new QList<QPointF>;
}

MaskPaint::~MaskPaint()
{
    delete pointList;
}

QRectF MaskPaint::boundingRect() const
{
    return total;
}

void MaskPaint::setDevice(QGraphicsView* device)
{
    display = device;
}

void MaskPaint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPainterPath path;
    QColor colour;
    double x,y;

    painter = new QPainter;
    painter->begin(display->viewport());
    colour = QColor(0,220,255);
    painter->setPen(colour);

    int i = 0;
    while (i < pointList->length())
    {
        x = pointList->value(i).x();
        y = pointList->value(i).y();
        painter->drawRect(x,y,squareSize,squareSize);
        i++;
    }
    delete painter;
}

void MaskPaint::addPoint(QPointF point)
{
    pointList->append(point);
}
