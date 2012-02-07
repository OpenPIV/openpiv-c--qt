/*
====================================================================================

File: maskpaint.h
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

#ifndef MASKPAINT_H
#define MASKPAINT_H

#include <QGraphicsItem>
#include <QGraphicsView>
#include <QObject>

class MaskPaint : public QObject, public QGraphicsItem
{
    Q_OBJECT

    public:
        MaskPaint();
        virtual ~MaskPaint();

        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
        QRectF boundingRect() const;
        void addPoint(QPointF point);
        void setDevice(QGraphicsView* device);

    private:
        QRectF total;
        QList<QPointF> *pointList;
        double squareSize;
        QGraphicsView *display;
};

#endif // MASKPAINT_H
