/*
====================================================================================

File: gaussiansubpixel.h
Description: Header file for the algorithm to provide subpixel estimation of the
    displacement.
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

#ifndef GAUSSIANSUBPIXEL_H
#define GAUSSIANSUBPIXEL_H

#include "pivdata.h"

//! Algorithm to provide a subpixel estimate of the displacement
/*!
    \param Requires a pointer to an existing correlation map
    \param Integer for the horizontal size of the interrogation window
    \param Integer for the vertical size of the interrogation window
*/
PivPointData gaussianSubPixel(double *cmap, int intLengthX, int intLengthY);

#endif // GAUSSIANSUBPIXEL_H
