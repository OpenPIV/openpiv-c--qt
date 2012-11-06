/*
====================================================================================

File: itempaint.h
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

#ifndef ITEMPAINT_H
#define ITEMPAINT_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsItem>

#include "settings.h"
#include "pivdata.h"

//! Reimplementation of QGraphicsItem to draw grid points
class GridPoint : public QGraphicsItem
{
    //! Reimplemented from QGraphicsItem
    QRectF boundingRect() const;
    //! Reimplemented from QGraphicsItem
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

//! Reimplementation of QGraphicsItem to draw vectors
class Vector : public QGraphicsItem
{
public:
    //! Constructor
    Vector();

    //! Function used to pass parameters for the vector object
    void setVector(qreal speedPass, qreal scalePass, QColor colourPass);

    //! Reimplemented from QGraphicsItem
    QRectF boundingRect() const;
    //! Reimplemented from QGraphicsItem
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QColor _colour;
    qreal _speed;
    qreal _scale;
    qreal _length;
    qreal _headWidth;
    qreal _tailWidth;
    qreal _lineWidth;
    qreal _lineLength;
    qreal _headLength;
};

//! Class handling the drawing of grid points and vectors
class ItemPaint : public QObject
{
Q_OBJECT

public:
    //! Constructor
    explicit ItemPaint(QObject *parent = 0);
    //! Destructor
    virtual ~ItemPaint();

    //! Function to pass a pointer to the QGraphicsScene on which to paint items
    void setScene(QGraphicsScene *scenePass);
    //! Handles drawing the grid points
    void drawGrid(QList<QPoint> pointList, Settings *settings);
    //! Handles removal of the grid
    void removeGrid();
    //! Handles drawing the vectors
    void drawVectors(PivData *pivData, Settings *settings);
    //! Handles removal of the vectors
    void removeVectors();

signals:
    //! Signal emitted when the grid is drawn or updated
    void gridDrawn();
    //! Signal emitted when the vectors are drawn or updated
    void vectorsDrawn();

protected:
    //! Creates the QGraphicsItemGroup for the vectors
    void createVectorGroup(int num);
    //! Creates the QGraphicsItemGroup for the grid points
    void createGridPointGroup(int num);

private:
    QGraphicsScene *scene;
    QGraphicsItemGroup *gridPointGroup;
    QGraphicsItemGroup *vectorGroup;
    bool gridPointGroupCreated;
    bool vectorGroupCreated;
    QList<GridPoint *> *gridPointList;
    int currentGridSize;
    QList<Vector *> *vectorList;
};

#endif // ITEMPAINT_H
