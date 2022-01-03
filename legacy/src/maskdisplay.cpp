/*
====================================================================================

File: maskdisplay.cpp
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

/* Currently users make an image in a 3rd party software (such as Gimp) of the same
   resolution and whatever areas aren't coloured to alpha are used in the PIV
   computations.  There was an initial attempt to allow the user to draw simple
   shapes by connecting points but this was abandoned (although remnants of this
   code currently remain).  */

#include "maskdisplay.h"
#include "maskpaint.h"
#include "imagepaint.h"
#include "imagedata.h"

#include <QtGui/QGraphicsView>
#include <QMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QFileDialog>
#include <math.h>


MaskDisplay::MaskDisplay(QWidget* parent) : QGraphicsView(parent)
{
    this->setCursor(Qt::ArrowCursor);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    image = new QImage;
    scene = new QGraphicsScene;
    imPaint = new ImagePaint;
   // painter = new QPainter;
    styleItem = new QStyleOptionGraphicsItem;
    rect = new QRect;
    rectF = new QRectF;
    viewRectF = new QRectF;

    maskPaint = new ImagePaint;
    maskRect = new QRect;
    maskRectF = new QRectF;
    //maskPainter = new QPainter;
    //maskStyleItem = new QStyleOptionGraphicsItem;

    currentImageCreated = false;
    imPaintAdded = false;
}

MaskDisplay::~MaskDisplay()
{
    delete maskPaint;
    delete maskRect;
    delete maskRectF;

    delete image;
    delete scene;
    //delete painter;
    delete styleItem;
    delete rect;
    delete rectF;
    delete viewRectF; 

    //delete maskPainter;
    //delete maskStyleItem;
}

void MaskDisplay::wheelEvent(QWheelEvent* event)
{
    if (event->delta() > 0)
    {
        this->scale(1.05,1.05);
        viewRectF->setWidth(viewRectF->width()/1.05);
        viewRectF->setHeight(viewRectF->height()/1.05);
    }
    else
    {
        this->scale(1.0/1.05,1.0/1.05);
        viewRectF->setWidth(viewRectF->width()*1.05);
        viewRectF->setHeight(viewRectF->height()*1.05);
    }
}

void MaskDisplay::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        point1 = mapToScene(QPoint(event->x(),event->y()));
        this->setCursor(Qt::ClosedHandCursor);
        mouseIsPressed = true;
    }
}

void MaskDisplay::mouseReleaseEvent(QMouseEvent* event)
{
    mouseIsPressed = false;
    point2 = mapToScene(QPoint(event->x(),event->y()));
    this->setCursor(Qt::OpenHandCursor);
}

void MaskDisplay::mouseMoveEvent(QMouseEvent* event)
{
    double dx, dy;
    double xCenter, yCenter;
    int xCenterView, yCenterView;

    if (mouseIsPressed)
    {
        xCenterView = int(round(double(this->width() / 2.0)));
        yCenterView = int(round(double(this->height() / 2.0)));

        point2 = mapToScene(QPoint(event->x(),event->y()));
        dx = double(point2.x() - point1.x());
        dy = double(point2.y() - point1.y());

        xCenter = QPointF(this->mapToScene(QPoint(xCenterView,yCenterView))).x();
        yCenter = QPointF(this->mapToScene(QPoint(xCenterView,yCenterView))).y();
        centerPoint = QPointF(double(xCenter-dx),double(yCenter-dy));
        this->centerOn(centerPoint);
    }
    else
    {
        point1 = mapToScene(QPoint(event->x(),event->y()));
    }
}

void MaskDisplay::mouseDoubleClickEvent(QMouseEvent* event)
{
    //painter2 = new QPainter;
    //painter2->begin(this);
    //maskPaint->addPoint(QPointF(event->x(),event->y()));
    //maskPaint->setDevice(this);
    //maskPaint->paint(painter2, styleItem, this);
    //painter2->end();
    //delete painter2;
}

void MaskDisplay::resizeEvent(QResizeEvent *event)
{
    this->fitInView(*viewRectF, Qt::KeepAspectRatio);
    this->centerOn(centerPoint);
}

void MaskDisplay::drawImage(QImage imagePass)
{
    if (imPaintAdded) scene->removeItem(imPaint);
    painter = new QPainter;
    *image = imagePass;
    *rect = image->rect();
    *rectF = QRectF(*rect);
    *viewRectF = *rectF;
    imPaint->setupPainter(*rectF);
    imPaint->loadImage(*image);
    imPaint->paint(painter,styleItem,this);
    scene->addItem(imPaint);
    imPaintAdded = true;
    this->setSceneRect(QRectF(*rect));
    this->setScene(scene);
    this->fitInView(QRectF(*rect), Qt::KeepAspectRatio);
    centerPoint = QPointF(rectF->width()/2,rectF->height()/2);
    this->show();
    emit(imDrawn());
    delete painter;
}

void MaskDisplay::display(QString fileName)
{
    if (currentImageCreated) delete currentImage;
    currentImage = new ImageData;
    currentImageCreated = true;
    if (currentImage->read(fileName))
        this->drawImage(currentImage->toQImage());
}

void MaskDisplay::importMask()
{
    painter = new QPainter;
    maskName = QFileDialog::getOpenFileName(this);
    maskImage = new QImage(maskName);
    // setup scaling stuff
    *maskRect = maskImage->rect();
    *maskRectF = QRectF(*maskRect);
    maskPaint->setupPainter(*maskRectF);
    maskPaint->loadImage(*maskImage);
    maskPaint->paint(painter,styleItem,this);
    scene->addItem(maskPaint);
    delete painter;
}

bool MaskDisplay::isDrawn()
{
    return imPaintAdded;
}

QImage* MaskDisplay::maskDisplayed()
{
    return maskImage;
}
