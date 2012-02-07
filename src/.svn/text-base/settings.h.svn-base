#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QColor>
#include <QSize>
#include <QList>
#include <QPoint>
#include <QRect>

#include "filteroptions.h"

namespace OpenPIV
{
    // Processors
    const int FFTCorrelator = 0;

    // Detectors
    const int GaussianSubPixel = 0;

    // Output
    const int Text = 0;
    const int HDF5 = 1;
};

class Settings : public QObject
{
    Q_OBJECT

    public:
        Settings();
        virtual ~Settings();

        // Processing
        int deltaX();       
        int deltaY();
        int intLengthX();
        int intLengthY();
        int processor();
        int detector();
        QSize imageSize();
        void setImageSize(QSize imageSizePass);
        bool isMask();
        QRect roi();

        // Batch processing
        bool batchFilter();
        bool batchMask();
        bool batchShowImage();
        bool batchShowVectors();
        bool batchThreading();

        // Vectors
        QColor vectorColourFiltered();
        QColor vectorColourUnfiltered();
        double vectorScale();
        double vectorSub();

        // Filters
        FilterOptions filterOptions();

        // Output
        QString expName();
        QString outputFolder();
        int outputFormat();

    public slots:
        
        // Processing
        void setDeltaX(int deltaXPass);
        void setDeltaY(int deltaYPass);
        void setIntLengthX(int intLengthXPass);
        void setIntLengthY(int intLengthYPass);
        void setProcessor(int processPass);
        void setDetector(int detectPass);
        void setIsMask(bool isMaskPass);
        void setRoi(QRect roiPass);

        // Batch processing
        void setBatchFilter(bool batchFilterPass);
        void setBatchMask(bool batchMaskPass);
        void setBatchShowImage(bool batchShowImagePass);
        void setBatchShowVectors(bool batchShowVectorsPass);
        void setBatchThreading(bool batchThreadingPass);

        // Vectors
        void setVectorColourFiltered(QColor vectorColourPass);
        void setVectorColourUnfiltered(QColor vectorColourPass);
        void setVectorScale(double vectorScalePass);
        void setVectorSub(double vectorSubPass);

        // Filters
        void setFilterOptions(FilterOptions filterOptionsPass);

        // Output
        void setExpName(QString expNamePass);
        void setOutputFolder(QString outputFolderPass);
        void setOutputFormat(int outputFormatPass);

    signals:
        // Display
        void imageSizeChanged();

        // Processing
        void processSettingsChanged();

        // Vectors
        void vectorSettingChanged();

    protected:
        void changeDeltaX();
        void changeDeltaY();

    private:
        // Display
        QSize _imageSize;

        // Masking
        bool _isMask;
        QRect _roi;
        bool roiSet;

        // Processing
        int _deltaX, _deltaY;
        int xSpin, ySpin;
        int _intLengthX, _intLengthY;
        int _processor;
        int _detector;

        // Batch processing
        bool _batchFilter;
        bool _batchMask;
        bool _batchShowImage;
        bool _batchShowVectors;
        bool _batchThreading;

        // Vectors
        QColor _vectorColourFiltered;
        QColor _vectorColourUnfiltered;
        double _vectorScale;
        double _vectorSub;

        // Filters
        FilterOptions _filterOptions;

        // Output
        QString _outputFolder;
        QString _expName;
        int _outputFormat;
};

#endif // SETTINGS_H
