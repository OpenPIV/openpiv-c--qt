/*
====================================================================================

File: pivdisplay.cpp
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

#include <QGraphicsView>
#include <QMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QCoreApplication>
#include <cmath>
#include <iostream>

#include "pivdisplay.h"
#include "datacontainer.h"
#include "imagepaint.h"
#include "imagedata.h"
#include "itempaint.h"
#include "pivdata.h"

PivDisplay::PivDisplay(QWidget* parent) : QGraphicsView(parent)
{
    // Initializing the mouse
    mouseIsPressed = false;
    this->setMouseTracking(true);
    this->setCursor(Qt::OpenHandCursor);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Initializing dynamic objects
    image = new QImage;
    _scene = new QGraphicsScene;
    itemPaint = new ItemPaint;
    rect = new QRect;
    rectF = new QRectF;
    viewRectF = new QRectF;
    viewRectFSet = false;

    // Initializing various boolean objects to false
    currentImageCreated = false;

    imPaintAdded = false;
    maskPaintAdded = false;

    maskActive = false;
    vectorsActive = false;
    vectorsDrawn = false;

    // Passing the pointer to the viewer's scene to the ItemPaint object
    itemPaint->setScene(_scene);
}

PivDisplay::~PivDisplay()
{
    // Deleting dynamic objects
    delete image;
    delete _scene;
    delete rect;
    delete rectF;
    delete viewRectF;
}

void PivDisplay::setSettings(Settings *settingsPass)
{
    settings = settingsPass;
}

void PivDisplay::setData(DataContainer *filedataPass)
{
    filedata = filedataPass;
}

void PivDisplay::wheelEvent(QWheelEvent* event)
{
    // If user is rolling the wheel 'forwards' then zoom in otherwise zoom out
    if (event->delta() > 0) zoomIn();
    else zoomOut();
}

void PivDisplay::zoomIn()
{
    // Zooming in by a relative increment of 5%
    this->scale(1.05,1.05);
    viewRectF->setWidth(viewRectF->width()/1.05);
    viewRectF->setHeight(viewRectF->height()/1.05);
    viewRectFSet = true;
}

void PivDisplay::zoomOut()
{
    // Zooming out y a relative increment of 5%
    this->scale(1.0/1.05,1.0/1.05);
    viewRectF->setWidth(viewRectF->width()*1.05);
    viewRectF->setHeight(viewRectF->height()*1.05);
    viewRectFSet = true;
}

void PivDisplay::zoomFit()
{
    // Fitting the image to the size of the viewer window
    *viewRectF = *rectF;
    viewRectFSet = true;
    resize();
}

void PivDisplay::mousePressEvent(QMouseEvent* event)
{
    // Mapping the location where the mouse was pressed to point1
    point1 = mapToScene(QPoint(event->x(),event->y()));
    this->setCursor(Qt::ClosedHandCursor);
    mouseIsPressed = true;
}

void PivDisplay::mouseReleaseEvent(QMouseEvent* event)
{
    // Mapping the locaiton where th emouse was pressed to point2
    mouseIsPressed = false;
    point2 = mapToScene(QPoint(event->x(),event->y()));
    this->setCursor(Qt::OpenHandCursor);
}

void PivDisplay::mouseMoveEvent(QMouseEvent* event)
{
    double dx, dy;
    double xCenter, yCenter;
    int xCenterView, yCenterView;

    // If the mouse is pressed then a move event corresponds to a panning of the image
    if (mouseIsPressed)
    {
        // Get the current center of the viewing window
        xCenterView = int(double(this->width() / 2.0));
        yCenterView = int(double(this->height() / 2.0));

        point2 = mapToScene(QPoint(event->x(),event->y()));
        dx = double(point2.x() - point1.x());
        dy = double(point2.y() - point1.y());

        xCenter = QPointF(this->mapToScene(QPoint(xCenterView,yCenterView))).x();
        yCenter = QPointF(this->mapToScene(QPoint(xCenterView,yCenterView))).y();
        centerPoint = QPointF(double(xCenter-dx),double(yCenter-dy));
        // Center the image on the new centre in real time
        this->centerOn(centerPoint);
        viewRectFSet = true;
    }
    else
    {
        point1 = mapToScene(QPoint(event->x(),event->y()));
    }
    emit(mouseMoved(point1));
}

void PivDisplay::resizeEvent(QResizeEvent*)
{
    resize();
}

void PivDisplay::resize()
{
    // Always maintains the aspect ratio of the image
    this->fitInView(*viewRectF, Qt::KeepAspectRatio);
    this->centerOn(centerPoint);
}

QPointF PivDisplay::getPoint1()
{
    return point1;
}

QPointF PivDisplay::getPoint2()
{
    return point2;
}

void PivDisplay::drawLayers()
{
    // Always draw image layer
    if (currentImageCreated) delete currentImage;
    currentImage = new ImageData;
    currentImageCreated = true;
    if (currentImage->read(currentFileName))
    {
        this->drawImage(currentImage->toQImage());
    }

    // Next layer is mask image and grid
    if (maskActive)
    {
        drawMaskImage();
        itemPaint->drawGrid(*filedata->gridList(),settings);
    }
    else
    {
        if (maskPaintAdded) maskPaint->hide();
        itemPaint->removeGrid();
    }

    // Next layer is vectors
    if (vectorsActive)
    {
        if (filedata->hasCurrentVectors())
        {
            pivData = filedata->currentPivData();
            itemPaint->drawVectors(pivData, settings);
            vectorsDrawn = true;
        }
        else
        {
            itemPaint->removeVectors();
        }
    }
    refreshView();
    settings->setImageSize(image->size());
}

void PivDisplay::refreshView()
{
    this->setSceneRect(*rectF);
    this->setScene(_scene);
    if (viewRectFSet)
    {
        this->fitInView(*viewRectF, Qt::KeepAspectRatio);
        this->centerOn(centerPoint);
    }
    else
    {
        this->fitInView(*rectF, Qt::KeepAspectRatio);
        centerPoint = QPointF(rectF->width()/2,rectF->height()/2);
        viewRectFSet = true;
    }
    this->show();
}

void PivDisplay::maskToggled(bool isTrue)
{
    if (isTrue) maskActive = true;
    else maskActive = false;
    drawLayers();
}

void PivDisplay::vectorsToggled(bool isTrue)
{
    if (isTrue) vectorsActive = true;
    else
    {
        vectorsActive = false;
        if (vectorsDrawn) itemPaint->removeVectors();
    }
    drawLayers();
}

void PivDisplay::vectorsChanged()
{
    drawLayers();
}

void PivDisplay::displayCurrent()
{
    // Drawing either A or B image as defined in the DataContainer object: filedata
    if (filedata->isCurrentA()) currentFileName = filedata->currentData().imageA();
    else currentFileName = filedata->currentData().imageB();
    drawLayers();
}

void PivDisplay::drawImage(QImage imagePass)
{
    // ImPaint object performs the actual painting, but it is controlled and added to the scene here
    if (imPaintAdded)
    {
        imPaint->setImage(imagePass);
    }
    else
    {
        *image = imagePass;
        *rect = image->rect();
        *rectF = QRectF(*rect);
        if (viewRectF->width() == 0)
        {
            *viewRectF = *rectF;
            viewRectFSet = true;
        }
        imPaint = new ImagePaint(*image);
        _scene->addItem(imPaint);
        imPaintAdded = true;
    }
}

void PivDisplay::drawMaskImage()
{
    // MaskPaint object performs the actual painting, but it is controlled and added to the scene here
    maskImage = filedata->mask();
    if (!maskImage->isNull())
    {
        if (maskPaintAdded)
        {
            maskPaint->setImage(*maskImage);
        }
        else
        {
            maskPaint = new ImagePaint(*maskImage);
            _scene->addItem(maskPaint);
            maskPaintAdded = true;
        }
    }
}

