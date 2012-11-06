/*
====================================================================================

File: itempaint.cpp
Description: These classes handle the drawing of vectors and grid points in
    addition to handling their painting in the display.
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

#include <QPainter>
#include <QImage>
#include <cmath>
#include <iostream>

#include "itempaint.h"
#include "pivdata.h"
#include "settings.h"

const float pi = 3.14159265359;

ItemPaint::ItemPaint(QObject *parent) : QObject(parent)
{
    // Initialization of variables
    gridPointGroupCreated = false;
    gridPointList = new QList<GridPoint *>;
    vectorGroupCreated = false;
    vectorList = new QList<Vector *>;
}

ItemPaint::~ItemPaint()
{
    // Deleting the two lists
    delete gridPointList;
    delete vectorList;
}

void ItemPaint::setScene(QGraphicsScene *scenePass)
{
    scene = scenePass;
}

void ItemPaint::drawGrid(QList<QPoint> pointList, Settings *settings)
{
    // Drawing the grid points
    GridPoint *gridPoint;
    int _intLengthX = settings->intLengthX();
    int _intLengthY = settings->intLengthY();
    if (!gridPointGroupCreated || pointList.size() != currentGridSize) createGridPointGroup(pointList.size());

    int centreX, centreY;
    for (int i = 0; i < pointList.size(); i++)
    {
        gridPoint = gridPointList->value(i);
        centreX = pointList.value(i).rx() + _intLengthX/2;
        centreY = pointList.value(i).ry() + _intLengthY/2;

        // Set the position of a given grid point
        gridPoint->setPos(centreX,centreY);
        // Grid points are added and deleted using visibility rather than actually creating/deleting objects
        gridPoint->setVisible(true);
    }
    emit(gridDrawn());
}

void ItemPaint::drawVectors(PivData *pivData, Settings *settings)
{
    Vector *vector;
    qreal _x, _y, _u, _v, angle, speed;
    int _width = pivData->width();
    int _height = pivData->height();

    int _intLengthX = settings->intLengthX();
    int _intLengthY = settings->intLengthY();
    double scale = settings->vectorScale();
    double vectorSub = settings->vectorSub();
    QColor colourFiltered = settings->vectorColourFiltered();
    QColor colourUnfiltered = settings->vectorColourUnfiltered();

    int i, j, vecIndex;

    if (!vectorGroupCreated) createVectorGroup(pivData->numValid());
    vecIndex = 0;
    for (i = 0; i < _height; i++)
    {
        for (j = 0; j < _width; j++)
        {
            if (pivData->isValid(i,j))
            {
                vector = vectorList->value(vecIndex);
                // Set the magnitude of the vector
                _x = pivData->data(i,j).x + _intLengthX/2.0;
                _y = pivData->data(i,j).y + _intLengthY/2.0;
                _u = pivData->data(i,j).u - vectorSub;
                _v = -pivData->data(i,j).v;
                speed = sqrt(_u*_u + _v*_v);
                // Set the colour of the vector
                if (pivData->data(i,j).filtered) vector->setVector(speed, scale, colourFiltered);
                else vector->setVector(speed, scale, colourUnfiltered);
                // Set the position of the vector
                vector->setPos(_x,_y);
                // Set the orientation of the vector
                angle = atan2(_v,_u);
                vector->setRotation(-angle/pi*180.0);
                // Vectors are added and deleted using visibility rather than actually creating/deleting objects
                vector->setVisible(true);

                vecIndex++;
            }
        }
    }
    emit(vectorsDrawn());
}

void ItemPaint::createGridPointGroup(int num)
{
    currentGridSize = num;
    GridPoint *gridPoint;
    for (int i = 0; i < num; i++)
    {
        gridPoint = new GridPoint;
        gridPointList->append(gridPoint);
        scene->addItem(gridPointList->value(i));
        gridPointList->value(i)->hide();
    }
    gridPointGroupCreated = true;
}

void ItemPaint::removeGrid()
{
    for (int i = 0; i < gridPointList->size(); i++)
    {
        // Grid points are hidden rather than deleted to save time
        gridPointList->value(i)->hide();
    }
}

void ItemPaint::createVectorGroup(int num)
{
    Vector *vector;
    for (int i = 0; i < num; i++)
    {
        vector = new Vector;
        vectorList->append(vector);
        scene->addItem(vectorList->value(i));
        vectorList->value(i)->hide();
    }
    vectorGroupCreated = true;
}

void ItemPaint::removeVectors()
{
    for (int i = 0; i < vectorList->size(); i++)
    {
        // Vectors are hidden rather than deleted to save time
        vectorList->value(i)->hide();
    }
}

/*--------------*/

QRectF GridPoint::boundingRect() const
{
    qreal width = 15;
    return QRectF(-width/2.0,-width/2.0,width,width);
}

void GridPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // Actual painting of grid point objects
    qreal width = 15;
    qreal thick = 2;
    painter->setBrush(Qt::darkGreen);
    painter->setPen(Qt::darkGreen);
    painter->drawRect(QRectF(-width/2.0,-thick/2.0,width,thick));
    painter->drawRect(QRectF(-thick/2.0,-width/2.0,thick,width));
}

/*------------*/

Vector::Vector() : QGraphicsItem()
{

}

void Vector::setVector(qreal speedPass, qreal scalePass, QColor colourPass)
{
    // Visually appealing ratios between the various aspects of a vector head and tail
    _speed = speedPass;
    _scale = scalePass;
    _length = _scale * _speed;
    _headWidth = 0.10 * _length;
    _tailWidth = 0.25 * _headWidth;
    _lineLength = 0.75 * _length;
    _headLength = _length - _lineLength;
    _colour = colourPass;
}

QRectF Vector::boundingRect() const
{
    return QRectF(0.0, -_headWidth/2.0 , _length, _headWidth);
}

void Vector::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // Actual painting of vector objects
    QPainterPath path;

    painter->setBrush(_colour);
    painter->setPen(_colour);

    path.moveTo(0.0, -_tailWidth/2.0);
    path.lineTo(0.0, _tailWidth/2.0);
    path.lineTo(_lineLength, _tailWidth/2.0);
    path.lineTo(_lineLength, _headWidth/2.0);
    path.lineTo(_length, 0.0);
    path.lineTo(_lineLength, -_headWidth/2.0);
    path.lineTo(_lineLength, -_tailWidth/2.0);
    path.lineTo(0.0, -_tailWidth/2.0);

    painter->drawPath(path);
}
