/*
====================================================================================

File: generategrid.h
Description: Header file for the algorithm to generate the list of points at which
    to calculate the velocity whether a mask has been loaded or not.
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

#ifndef GENERATEGRID_H
#define GENERATEGRID_H

#include <QList>
#include <QPoint>
#include <QImage>
#include "settings.h"

//! Overloaded function to generate grid if a mask is provided
QList<QPoint> generateGrid(QImage *maskData, Settings *settings);
//! Overloaded function to generate grid if a mask is not provided
QList<QPoint> generateGrid(Settings *settings);

#endif // GENERATEGRID_H
