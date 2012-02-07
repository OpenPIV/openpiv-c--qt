#ifndef GENERATEGRID_H
#define GENERATEGRID_H

#include <QList>
#include <QPoint>
#include <QImage>
#include "settings.h"

QList<QPoint> generateGrid(QImage *maskData, Settings *settings);

QList<QPoint> generateGrid(Settings *settings);

#endif // GENERATEGRID_H
