#ifndef FILTEROPTIONS_H
#define FILTEROPTIONS_H

namespace OpenPIV
{
    // Filters
    const int LocalMean = 0;
    const int LocalMedian = 1;
    const int InterpolateMean = 0;
}

class FilterOptions
{
    public:
        FilterOptions();
        virtual ~FilterOptions();

        // Global
        bool globalStd();
        void enableGlobalStd(bool globalStdPass);
        double nStd();
        void setNStd(double nStdPass);
        bool globalRange();
        void enableGlobalRange(bool globalRangePass);
        double umin();
        double umax();
        double vmin();
        double vmax();
        void setRange(double uminPass, double umaxPass, double vminPass, double vmaxPass);

        // Local
        bool local();
        void enableLocal(bool localPass);
        int localMethod();
        void setLocalMethod(int localMethodPass);
        int localNxN();
        void setLocalNxN(int localNxNPass);
        double uTol();
        double vTol();
        void setLocalTolerance(double uTolPass, double vTolPass);

        // Interpolate
        bool interpolate();
        void enableInterpolate(bool interpolatePass);
        int intMethod();
        void setIntMethod(int intMethodPass);
        int intNxN();
        void setIntNxN(int intNxNPass);

        // Smoothing
        bool smoothing();
        void enableSmoothing(bool smoothingPass);
        int smoothNxN();
        void setSmoothNxN(int smoothNxNPass);
        double smoothRadius();
        void setSmoothRadius(double smoothRadiusPass);

        // SNR
        bool snr();
        void enableSNR(bool snrPass);
        double snrThresh();
        void setSnrThresh(double snrThreshPass);

        // Image Intensity
        bool imageIntensity();
        void enableImageIntensity(bool imageIntensityPass);
        double imageThresh();
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
