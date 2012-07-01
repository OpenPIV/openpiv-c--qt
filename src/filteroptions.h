/*
====================================================================================

File: filteroptions.h
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

#ifndef FILTEROPTIONS_H
#define FILTEROPTIONS_H

namespace OpenPIV
{
    //! Filters
    const int LocalMean = 0;
    const int LocalMedian = 1;
    const int InterpolateMean = 0;
}

//! Container class for all of the various filtering options.
class FilterOptions
{

public:
    //! Constructor
    FilterOptions();
    //! Destructor
    virtual ~FilterOptions();

    // Global filters
    //! Returns true if filtering based on the global standard deviation, false otherwise.
    bool globalStd();
    //! Set to true if filtering based on the global standard deviation.
    void enableGlobalStd(bool globalStdPass);
    //! Returns a double indicating the multiplier on the standard deviation threshold.
    double nStd();
    //! Sets the multiplier on the standard deviation threshold.
    void setNStd(double nStdPass);

    //! Returns true if filtering based on the global displacement range, false otherwise.
    bool globalRange();
    //! Set to true if filtering based on the global displacement range.
    void enableGlobalRange(bool globalRangePass);
    //! Returns a double indicating the minimum allowable horizontal displacement (in pixels).
    double umin();
    //! Returns a double indicating the maximum allowable horizontal displacement (in pixels).
    double umax();
    //! Returns a double indicating the minimum allowable vertical displacement (in pixels).
    double vmin();
    //! Returns a double indicating the maximum allowable vertical displacement (in pixels).
    double vmax();
    /*!
        Sets the ranges for the hoizontal and vertical velocities.

        \param Minimum horizontal displacement (in pixels)
        \param Maximum horizontal displacement (in pixels)
        \param Minimum vertical displacement (in pixels)
        \param Maximum vertical displacement (in pixels)
    */
    void setRange(double uminPass, double umaxPass, double vminPass, double vmaxPass);

    // Local filters
    //! Returns true if local filtering is enabled, false otherwise.
    bool local();
    //! Set to true if filtering locally.
    void enableLocal(bool localPass);
    //! Returns an integer value relating to the method used for local filtering (see OpenPIV namespace)
    int localMethod();
    //! Set the type of method used for local filtering (see OpenPIV namespace \sa OpenPIV)
    void setLocalMethod(int localMethodPass);
    //! Returns an integer number for the size of the square local neighbourhood
    int localNxN();
    //! Set an integer number for the size of the square local neighbourhood
    void setLocalNxN(int localNxNPass);
    //! Returns the tolerance for the horizontal deviation from the local central estimator (i.e., mean or median) (in pixels)
    double uTol();
    //! Returns the tolerance for the vertical deviation from the local central estimator (i.e., mean or median) (in pixels)
    double vTol();
    /*!
        Sets the tolerances for the deviations from the local central estimator (i.e., mean or median)

        \param Tolerance for the deviation of the horizontal displacement from the local central estimator (in pixels)
        \param Tolerance for the deviation of the vertical displacement from the local central estimator (in pixels)
    */
    void setLocalTolerance(double uTolPass, double vTolPass);

    // Interpolate
    //! Returns true if interpolating filtered grid points, false otherwise.
    bool interpolate();
    //! Set to true if interpolating filtered grid points.
    void enableInterpolate(bool interpolatePass);
    //! Returns an intenger number for the method used to interpolate missing grid points (see OpenPIV namespace)
    int intMethod();
    //! Sets an integer number for the method used to interpolate missing grid points (see OpenPIV namespace)
    void setIntMethod(int intMethodPass);
    //! Returns the size of the square neighbourhood used to interpolate missing grid points
    int intNxN();
    //! Sets the size of the square neighbourhood used to interpolate missing grid points
    void setIntNxN(int intNxNPass);

    // Smoothing
    //! Returns true if Gaussian smoothing is to be applied to the whole field, false otherwise.
    bool smoothing();
    //! Set to true if Gaussian smoothing is to be applied to the whole field.
    void enableSmoothing(bool smoothingPass);
    //! Returns the size of the square kernel used to the smooth the data
    int smoothNxN();
    //! Sets the size of the square kernel used to the smooth the data
    void setSmoothNxN(int smoothNxNPass);
    //! Returns the radius of the Gaussian function used to smooth the data
    double smoothRadius();
    //! Sets the radius of the Gaussian function used to smooth the data
    void setSmoothRadius(double smoothRadiusPass);

    // SNR
    //! Returns true if filtering based on the signal-to-noise ratio, false otherwise.
    bool snr();
    //! Set to true if filtering based on the signal-to-noise ratio.
    void enableSNR(bool snrPass);
    //! Returns the threshold for the signal-to-noise ratio
    double snrThresh();
    //! Sets the threshold for the signal-to-noise ratio
    void setSnrThresh(double snrThreshPass);

    // Image Intensity
    //! Returns true if filtering based on the local image intensity, false otherwise.
    bool imageIntensity();
    //! Set to true if filtering based on the local image intensity.
    void enableImageIntensity(bool imageIntensityPass);
    //! Returns the value of the local image intensity threshold.
    double imageThresh();
    //! Sets the value of the local image intensity threshold.
    void setImageThresh(double imageThreshPass);

private:
    // Global
    bool _globalStd;
    double _nStd;
    bool _globalRange;
    double _umin, _umax, _vmin, _vmax;

    // Local
    bool _local;
    int _localMethod;
    int _localNxN;
    double _uTol;
    double _vTol;

    // Interpolate
    bool _interpolate;
    int _intMethod;
    int _intNxN;

    // Smoothing
    bool _smoothing;
    int _smoothNxN;
    double _smoothRadius;

    // SNR
    bool _snr;
    double _snrThresh;

    // Image Intensity
    bool _imageIntensity;
    double _imageThresh;
};

#endif // FILTEROPTIONS_H
