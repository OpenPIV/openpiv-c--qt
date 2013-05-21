/*
====================================================================================

File: maskdisplay.h
Description: This class inherits QGraphicsView and is used to display the image
    and the masking of that image.
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

#ifndef MASKDISPLAY_H
#define MASKDISPLAY_H

#include <QGraphicsView>
#include <QStyleOptionGraphicsItem>

#include "imagepaint.h"
#include "imagedata.h"
#include "maskpaint.h"

class MaskDisplay : public QGraphicsView
{
    Q_OBJECT

    public:
        MaskDisplay(QWidget* parent = 0);
        virtual ~MaskDisplay();

        bool isDrawn();
        QImage* maskDisplayed();

    public slots:
        void importMask();

    signals:
        void imDrawn();

    protected:
        void wheelEvent(QWheelEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);
        void mouseDoubleClickEvent(QMouseEvent* event);
        void resizeEvent(QResizeEvent* event);

    protected slots:
        void drawImage(QImage imagePass);
        void display(QString fileName);

    private:
        QPointF point1;
        QPointF point2;

        bool mouseIsPressed;

        QImage *image;
        QGraphicsScene *scene;
        ImagePaint *imPaint;
        bool imPaintAdded;
        QPainter *painter;
        QPainter *painter2;
        QStyleOptionGraphicsItem *styleItem;
        QRect *rect;
        QRectF *rectF;
        QRectF *viewRectF;
        QPointF centerPoint;

        ImagePaint *maskPaint;
        QPainter *maskPainter;
        QStyleOptionGraphicsItem *maskStyleItem;

        ImageData *currentImage;
        bool currentImageCreated;

        QString maskName;
        QImage *maskImage;
        QRect *maskRect;
        QRectF *maskRectF;
        //ImagePaint *maskPaint;
};

#endif // MASKDISPLAY_H
