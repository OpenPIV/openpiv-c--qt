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
    gridPointGroupCreated = false;
    gridPointList = new QList<GridPoint *>;
    vectorGroupCreated = false;
    //vectorList = new QList<QGraphicsItem *>;
    vectorList = new QList<Vector *>;
}

ItemPaint::~ItemPaint()
{
    delete gridPointList;
    delete vectorList;
}

void ItemPaint::setScene(QGraphicsScene *scenePass)
{
    scene = scenePass;
}

void ItemPaint::drawGrid(QList<QPoint> pointList, Settings *settings)
{
    GridPoint *gridPoint;
    int _intLengthX = settings->intLengthX();
    int _intLengthY = settings->intLengthY();
//    if (gridPointGroupCreated)
//    {
//        scene->destroyItemGroup(gridPointGroup);
//        gridPointList->clear();
//        gridPointGroupCreated = false;
//    }
    if (!gridPointGroupCreated || pointList.size() != currentGridSize) createGridPointGroup(pointList.size());

    int centreX, centreY;
    for (int i = 0; i < pointList.size(); i++)
    {
        gridPoint = gridPointList->value(i);
        centreX = pointList.value(i).rx() + _intLengthX/2;
        centreY = pointList.value(i).ry() + _intLengthY/2;

        gridPoint->setPos(centreX,centreY);
        gridPoint->setVisible(true);
        //gridPointList->append(gridPoint);
    }
//    gridPointGroup = scene->createItemGroup(*gridPointList);
//    gridPointGroupCreated = true;
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

    //if (vectorGroupCreated) removeVectors();

    if (!vectorGroupCreated) createVectorGroup(pivData->numValid());
    vecIndex = 0;
    for (i = 0; i < _height; i++)
    {
        for (j = 0; j < _width; j++)
        {
            if (pivData->isValid(i,j))
            {
                vector = vectorList->value(vecIndex);
                _x = pivData->data(i,j).x + _intLengthX/2.0;
                _y = pivData->data(i,j).y + _intLengthY/2.0;
                _u = pivData->data(i,j).u - vectorSub;
                _v = -pivData->data(i,j).v;
                speed = sqrt(_u*_u + _v*_v);
                //if (pivData->data(i,j).filtered) vector = new Vector(speed, scale, colourFiltered);
                if (pivData->data(i,j).filtered) vector->setVector(speed, scale, colourFiltered);
                //else vector = new Vector(speed, scale, colourUnfiltered);
                else vector->setVector(speed, scale, colourUnfiltered);
                vector->setPos(_x,_y);
                angle = atan2(_v,_u);
                vector->setRotation(-angle/pi*180.0);
                //vectorList->append(vector);
                vector->setVisible(true);
                vecIndex++;
            }
        }
    }
//    vectorGroup = scene->createItemGroup(*vectorList);
//    vectorGroupCreated = true;
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
//    if (vectorGroupCreated)
//    {
//        scene->destroyItemGroup(vectorGroup);
//        while (vectorList->size() > 0)
//            delete vectorList->takeFirst();
//        vectorGroupCreated = false;
//    }
    for (int i = 0; i < vectorList->size(); i++)
    {
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
    qreal width = 15;
    qreal thick = 2;
    painter->setBrush(Qt::darkGreen);
    painter->setPen(Qt::darkGreen);
    painter->drawRect(QRectF(-width/2.0,-thick/2.0,width,thick));
    painter->drawRect(QRectF(-thick/2.0,-width/2.0,thick,width));
}

/*------------*/

//Vector::Vector(qreal speedPass, qreal scalePass, QColor colourPass) : QGraphicsItem()
//{
//    _speed = speedPass;
//    _scale = scalePass;
//    _length = _scale * _speed;
//    _headWidth = 0.10 * _length;
//    _tailWidth = 0.25 * _headWidth;
//    _lineLength = 0.75 * _length;
//    _headLength = _length - _lineLength;
//    _colour = colourPass;
//}

Vector::Vector() : QGraphicsItem()
{

}

void Vector::setVector(qreal speedPass, qreal scalePass, QColor colourPass)
{
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
