#include <QPainter>
#include <iostream>
#include <cmath>
#include "colourbar.h"

ColourBar::ColourBar(QWidget *parent) :
    QWidget(parent)
{
    labelHeight = 20;
    labelWidth = 30;
    min = 0.0;
    max = 1.0;
    nBins = 100;
    minLabel = new QLabel(this);
    maxLabel = new QLabel(this);
    onlyPositive = true;
    this->hide();
}

void ColourBar::setRange(int minPass, int maxPass)
{
    if (maxPass > minPass)
    {
        max = maxPass;
        min = minPass;
    }
    else
    {
        max = minPass;
        min = maxPass;
    }
    if (min < 0.0)
    {
        zero = (0.0 - min) / (max - min);
        onlyPositive = false;
    }
    else if (max < 0.0)
    {
        onlyPositive = true;
    }
    else onlyPositive = true;
    this->repaint();
    drawLabels();
}

void ColourBar::paintEvent(QPaintEvent *event)
{    
    colourBarRect = QRect(labelWidth,0,this->width()-2*labelWidth,this->height());
    generateGradient();
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::black);
    painter.setBrush(linearGrad);
    painter.drawRect(colourBarRect);
    painter.end();
    drawLabels();
}

void ColourBar::drawLabels()
{
    minLabel->setGeometry(QRect(0,this->height()/2-labelHeight/2,labelWidth,labelHeight));
    maxLabel->setGeometry(QRect(this->width()-labelWidth,this->height()/2-labelHeight/2,labelWidth,labelHeight));

    minLabel->setText(QString("%1").arg(min));
    maxLabel->setText(QString("%1").arg(max));

    minLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    maxLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

void ColourBar::generateGradient()
{
    double middle;
    double r, g, b;
    int i;
    QColor iColour;
    QColor first,second,third;
    linearGrad = QLinearGradient(QPointF(0,0.5),QPointF(1.0,0.5));
    linearGrad.setCoordinateMode(QGradient::ObjectBoundingMode);
    if (onlyPositive)
    {
        middle = 0.5;
        first = QColor(0,0,131);
        second = QColor(0,255,200);
        third = QColor(255,255,0);
    }
    else
    {
        middle = zero;
        first = QColor(0,0,131);
        second = QColor(255,255,255);
        third = QColor(236,0,0);

    }
    linearGrad.setColorAt(0, first);
    linearGrad.setColorAt(middle, second);
    linearGrad.setColorAt(1, third);

    colourList.clear();
    int nToMiddle = int(floor(nBins * middle));
    for (i = 0; i < nToMiddle; i++)
    {
        r = double(i) / double(nToMiddle) * (second.red() - first.red()) + first.red();
        g = double(i) / double(nToMiddle) * (second.green() - first.green()) + first.green();
        b = double(i) / double(nToMiddle) * (second.blue() - first.blue()) + first.blue();
        iColour.setRgb(int(r),int(g),int(b));
        colourList.append(iColour);
    }
    for (i = nToMiddle; i < nBins; i++)
    {
        r = double(i) / double(nToMiddle) * (third.red() - second.red()) + first.red();
        g = double(i) / double(nToMiddle) * (third.green() - second.green()) + first.green();
        b = double(i) / double(nToMiddle) * (third.blue() - second.blue()) + first.blue();
        iColour.setRgb(int(r),int(g),int(b));
        colourList.append(iColour);
    }
}

QColor ColourBar::color(int percentile)
{
    if (percentile < colourList.size())
    {
        return colourList.value(percentile);
    }
    else return Qt::black;
}
