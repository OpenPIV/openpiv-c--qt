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
    mouseIsPressed = false;
    this->setMouseTracking(true);
    this->setCursor(Qt::OpenHandCursor);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    image = new QImage;
    _scene = new QGraphicsScene;
    itemPaint = new ItemPaint;
    rect = new QRect;
    rectF = new QRectF;
    viewRectF = new QRectF;
    viewRectFSet = false;

    currentImageCreated = false;

    imPaintAdded = false;
    maskPaintAdded = false;

    maskActive = false;
    vectorsActive = false;
    vectorsDrawn = false;

    itemPaint->setScene(_scene);
}

PivDisplay::~PivDisplay()
{
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
    if (event->delta() > 0) zoomIn();
    else zoomOut();
}

void PivDisplay::zoomIn()
{
    this->scale(1.05,1.05);
    viewRectF->setWidth(viewRectF->width()/1.05);
    viewRectF->setHeight(viewRectF->height()/1.05);
    viewRectFSet = true;
}

void PivDisplay::zoomOut()
{
    this->scale(1.0/1.05,1.0/1.05);
    viewRectF->setWidth(viewRectF->width()*1.05);
    viewRectF->setHeight(viewRectF->height()*1.05);
    viewRectFSet = true;
}

void PivDisplay::zoomFit()
{
    *viewRectF = *rectF;
    viewRectFSet = true;
    resize();
}

void PivDisplay::mousePressEvent(QMouseEvent* event)
{
    point1 = mapToScene(QPoint(event->x(),event->y()));
    this->setCursor(Qt::ClosedHandCursor);
    mouseIsPressed = true;
}

void PivDisplay::mouseReleaseEvent(QMouseEvent* event)
{
    mouseIsPressed = false;
    point2 = mapToScene(QPoint(event->x(),event->y()));
    this->setCursor(Qt::OpenHandCursor);
}

void PivDisplay::mouseMoveEvent(QMouseEvent* event)
{
    double dx, dy;
    double xCenter, yCenter;
    int xCenterView, yCenterView;

    if (mouseIsPressed)
    {
        xCenterView = int(double(this->width() / 2.0));
        yCenterView = int(double(this->height() / 2.0));

        point2 = mapToScene(QPoint(event->x(),event->y()));
        dx = double(point2.x() - point1.x());
        dy = double(point2.y() - point1.y());

        xCenter = QPointF(this->mapToScene(QPoint(xCenterView,yCenterView))).x();
        yCenter = QPointF(this->mapToScene(QPoint(xCenterView,yCenterView))).y();
        centerPoint = QPointF(double(xCenter-dx),double(yCenter-dy));
        this->centerOn(centerPoint);
        viewRectFSet = true;
    }
    else
    {
        point1 = mapToScene(QPoint(event->x(),event->y()));
    }
    emit(mouseMoved(point1));
}

void PivDisplay::resizeEvent(QResizeEvent *event)
{
    resize();
}

void PivDisplay::resize()
{
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
    if (filedata->isCurrentA()) currentFileName = filedata->currentData().imageA();
    else currentFileName = filedata->currentData().imageB();
    drawLayers();
}

void PivDisplay::drawImage(QImage imagePass)
{
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

