/*
====================================================================================

File: pivdisplay.h
Description: This class inherits QGraphicsView and is used to show the image and,
    eventully, the PIV data.
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

#ifndef PIVDISPLAY_H
#define PIVDISPLAY_H

#include <QGraphicsView>
#include <QStyleOptionGraphicsItem>

#include "imagepaint.h"
#include "imagedata.h"
#include "settings.h"
#include "itempaint.h"
#include "datacontainer.h"
#include "pivdata.h"

class PivDisplay : public QGraphicsView
{
    Q_OBJECT

    public:
        PivDisplay(QWidget* parent = 0);
        virtual ~PivDisplay();

        QPointF getPoint1();
        QPointF getPoint2();

        void setSettings(Settings *settingsPass);
        void setData(DataContainer *filedataPass);

    signals:
        void mouseMoved(QPointF pointPass);

    public slots:
        void refreshView();
        void displayCurrent();
        void maskToggled(bool isTrue);
        void vectorsToggled(bool isTrue);

        void zoomIn();
        void zoomOut();
        void zoomFit();
        void vectorsChanged();

    protected:
        void resize();

        void wheelEvent(QWheelEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);
        void resizeEvent(QResizeEvent* event);

        void drawLayers();
        void drawMaskImage();
        void drawImage(QImage imagePass);

    private:
        QPointF point1;
        QPointF point2;

        bool mouseIsPressed;

        QImage *image;

        QImage *maskImage;

        QGraphicsScene *_scene;
        ImagePaint *imPaint;
        ImagePaint *maskPaint;
        ItemPaint *itemPaint;
        bool imPaintAdded;
        bool maskPaintAdded;
        QRect *rect;
        QRectF *rectF;
        QRectF *viewRectF;
        bool viewRectFSet;
        QPointF centerPoint;

        ImageData *currentImage;
        QString currentFileName;
        bool currentImageCreated;

        bool maskActive;
        bool vectorsActive;
        bool vectorsDrawn;

        Settings *settings;
        PivData *pivData;

        DataContainer *filedata;

};
#endif // PIVDISPLAY_H
