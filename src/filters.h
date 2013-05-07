/*
====================================================================================

File: filters.h
Description: Header file for the various filtering algorithms.
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

#ifndef FILTERS_H
#define FILTERS_H

#include "filteroptions.h"
#include "pivdata.h"

// SNR
//! Filtering based on the signal-to-noise ratio
void snr(PivData *pivData, FilterOptions filterOptions);

// Image intensity
//! Filtering based on the image intensity
void imageIntensity(PivData *pivData, FilterOptions filterOptions);

// Global
//! Filtering based on the user-defined range of horizontal and vertical displacements
void globalRange(PivData *pivData, FilterOptions filterOptions);
//! Filtering based on a multiple of the global standard deviation
void globalStd(PivData *pivData, FilterOptions filterOptions);

// Local
//! Filtering based on the deviation from a local central estimate (i.e., mean, median)
void localDetect(PivData *pivData, FilterOptions filterOptions);

// Interpolation
//! Interpolating data that is missing after filtering
void meanInterpolate(PivData *pivData, FilterOptions filterOptions);

// Smoothing
//! Gaussian smoothing of the data
void gaussianBlur(PivData *pivData, FilterOptions filterOptions);

#endif // FILTERS_H
