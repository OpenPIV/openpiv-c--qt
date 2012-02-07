#include "filteroptions.h"

FilterOptions::FilterOptions()
{
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
