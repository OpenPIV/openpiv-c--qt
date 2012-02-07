#ifndef ITEMPAINT_H
#define ITEMPAINT_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsItem>

#include "settings.h"
#include "pivdata.h"

class GridPoint : public QGraphicsItem
{
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

class Vector : public QGraphicsItem
{
public:
    //Vector(qreal speedPass, qreal scalePass, QColor colourPass);
    Vector();

    void setVector(qreal speedPass, qreal scalePass, QColor colourPass);

    QRectF boundingRect() const;
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

class ItemPaint : public QObject
{
    Q_OBJECT

    public:
        explicit ItemPaint(QObject *parent = 0);
        virtual ~ItemPaint();

        void setScene(QGraphicsScene *scenePass);
        void drawGrid(QList<QPoint> pointList, Settings *settings);
        void drawVectors(PivData *pivData, Settings *settings);
        void removeVectors();
        void removeGrid();

    signals:
        void gridDrawn();
        void vectorsDrawn();

    protected:
        void createVectorGroup(int num);
        void createGridPointGroup(int num);

    private:
        QGraphicsScene *scene;
        QGraphicsItemGroup *gridPointGroup;
        QGraphicsItemGroup *vectorGroup;
        bool gridPointGroupCreated;
        bool vectorGroupCreated;
        QList<GridPoint *> *gridPointList;
        int currentGridSize;
        //QList<QGraphicsItem *> *vectorList;
        QList<Vector *> *vectorList;
};




#endif // ITEMPAINT_H
