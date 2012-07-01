/*
====================================================================================

File: filteroptions.cpp
Description: Container class for all of the various filtering options.
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

#include "filteroptions.h"

FilterOptions::FilterOptions()
{
    // Setting default parameters for the filtering options

    // Global
    _globalStd = false;
    _nStd = 3.0;
    _globalRange = false;
    _umin = -10.0;
    _umax = 10.0;
    _vmin = -10.0;
    _vmax = 10.0;

    // Local
    _local = false;
    _localMethod = OpenPIV::LocalMedian;
    _localNxN = 5;
    _uTol = 3.0;
    _vTol = 3.0;

    // Interpolate
    _interpolate = false;
    _intNxN = 5;
    _intMethod = OpenPIV::InterpolateMean;

    // Smoothing
    _smoothing = false;
    _smoothNxN = 5;
    _smoothRadius = 0.8;

    // SNR
    _snr = false;
    _snrThresh = 2.0;

    // Image Intensity
    _imageIntensity = false;
    _imageThresh = 0.0;
}

FilterOptions::~FilterOptions()
{
}

// Global
bool FilterOptions::globalStd() { return _globalStd; }
void FilterOptions::enableGlobalStd(bool globalStdPass) { _globalStd = globalStdPass; }

double FilterOptions::nStd() { return _nStd; }
void FilterOptions::setNStd(double nStdPass) { _nStd = nStdPass; }

bool FilterOptions::globalRange() { return _globalRange; }
void FilterOptions::enableGlobalRange(bool globalRangePass) { _globalRange = globalRangePass; }

double FilterOptions::umin() { return _umin; }
double FilterOptions::umax() { return _umax; }
double FilterOptions::vmin() { return _vmin; }
double FilterOptions::vmax() { return _vmax; }
void FilterOptions::setRange(double uminPass, double umaxPass, double vminPass, double vmaxPass)
{
    _umin = uminPass;
    _umax = umaxPass;
    _vmin = vminPass;
    _vmax = vmaxPass;
}

// Local
bool FilterOptions::local() { return _local; }
void FilterOptions::enableLocal(bool localPass) { _local = localPass; }

int FilterOptions::localMethod() { return _localMethod; }
void FilterOptions::setLocalMethod(int localMethodPass) { _localMethod = localMethodPass; }

int FilterOptions::localNxN() { return _localNxN; }
void FilterOptions::setLocalNxN(int localNxNPass) { _localNxN = localNxNPass; }

double FilterOptions::uTol() { return _uTol; }
double FilterOptions::vTol() { return _vTol; }
void FilterOptions::setLocalTolerance(double uTolPass, double vTolPass)
{
    _uTol = uTolPass;
    _vTol = vTolPass;
}

// Interpolate
bool FilterOptions::interpolate() { return _interpolate; }
void FilterOptions::enableInterpolate(bool interpolatePass) { _interpolate = interpolatePass; }

int FilterOptions::intMethod() { return _intMethod; }
void FilterOptions::setIntMethod(int intMethodPass) { _intMethod = intMethodPass; }

int FilterOptions::intNxN() { return _intNxN; }
void FilterOptions::setIntNxN(int intNxNPass) { _intNxN = intNxNPass; }

// Smoothing
bool FilterOptions::smoothing() { return _smoothing; }
void FilterOptions::enableSmoothing(bool smoothingPass) { _smoothing = smoothingPass; }

int FilterOptions::smoothNxN() { return _smoothNxN; }
void FilterOptions::setSmoothNxN(int smoothNxNPass) { _smoothNxN = smoothNxNPass; }

double FilterOptions::smoothRadius() { return _smoothRadius; }
void FilterOptions::setSmoothRadius(double smoothRadiusPass) { _smoothRadius = smoothRadiusPass; }

// SNR
bool FilterOptions::snr() { return _snr; }
void FilterOptions::enableSNR(bool snrPass) { _snr = snrPass; }

double FilterOptions::snrThresh() { return _snrThresh; }
void FilterOptions::setSnrThresh(double snrThreshPass) { _snrThresh = snrThreshPass; }

// Image Intensity
bool FilterOptions::imageIntensity() { return _imageIntensity; }
void FilterOptions::enableImageIntensity(bool imageIntensityPass) { _imageIntensity = imageIntensityPass; }

double FilterOptions::imageThresh() { return _imageThresh; }
void FilterOptions::setImageThresh(double imageThreshPass) { _imageThresh = imageThreshPass; }
