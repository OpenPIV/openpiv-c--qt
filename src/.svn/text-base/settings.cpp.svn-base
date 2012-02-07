#include <QSize>
#include <QColor>
#include <cmath>
#include <iostream>
#include "settings.h"
#include "filteroptions.h"

Settings::Settings()
{
    // Processing
    _deltaX = 16;
    _deltaY = 16;
    _intLengthX = 32;
    _intLengthY = 32;
    _imageSize.setWidth(-1);
    _imageSize.setHeight(-1);
    _isMask = false;    
    _processor = OpenPIV::FFTCorrelator;
    _detector = OpenPIV::GaussianSubPixel;
    _roi = QRect(0,0,0,0);
    roiSet = false;

    // Vectors
    _vectorColourUnfiltered = QColor(Qt::cyan);
    _vectorColourFiltered = QColor(Qt::red);
    _vectorScale = 5.0;
    _vectorSub = 0.0;

    // Session
    _expName = "OpenPIV";

    // Output
    _outputFolder = "";
    _outputFormat = OpenPIV::Text;

    // Batch
    _batchFilter = false;
    _batchMask = false;
    _batchShowImage = false;
    _batchShowVectors = false;
    _batchThreading = true;
}

Settings::~Settings()
{
}



/* ----------Processing -------------*/

int Settings::deltaX() { return _deltaX; }
void Settings::setDeltaX(int deltaXPass)
{
    xSpin = deltaXPass;
    changeDeltaX();
}

int Settings::deltaY() { return _deltaY; }
void Settings::setDeltaY(int deltaYPass)
{
    ySpin = deltaYPass;
    changeDeltaY();
}

void Settings::changeDeltaX()
{
    _deltaX = int(double(_intLengthX) * double(100-xSpin) / 100);
    emit(processSettingsChanged());
}

void Settings::changeDeltaY()
{
    _deltaY = int(double(_intLengthY) * double(100-ySpin) / 100);
    emit(processSettingsChanged());
}

int Settings::intLengthX() { return _intLengthX; }
void Settings::setIntLengthX(int intLengthXPass)
{
    _intLengthX = int(pow(2.0,double(4+intLengthXPass)));
    changeDeltaX();
}

int Settings::intLengthY() { return _intLengthY; }
void Settings::setIntLengthY(int intLengthYPass)
{
    _intLengthY = int(pow(2.0,double(4+intLengthYPass)));
    changeDeltaY();
}

int Settings::processor() { return _processor; }
void Settings::setProcessor(int processPass) { _processor = processPass; }

int Settings::detector() { return _detector; }
void Settings::setDetector(int detectPass) { _detector = detectPass; }

QSize Settings::imageSize() { return _imageSize; }
void Settings::setImageSize(QSize imageSizePass)
{
    bool toEmit = false;
    if (!((imageSizePass.width() == _imageSize.width()) && (imageSizePass.height() == _imageSize.height())))
        toEmit = true;
    _imageSize = imageSizePass;
    if (!roiSet)
    {
        this->setRoi(QRect(0,0,_imageSize.width(),_imageSize.height()));
        roiSet = true;
    }
    else if (roiSet && toEmit)
    {
        if (_roi.left() < 0) _roi.setLeft(0);
        if (_roi.right() > _imageSize.width() || _roi.right() < 0) _roi.setRight(_imageSize.width());
        if (_roi.bottom() > _imageSize.height() || _roi.bottom() < 0) _roi.setBottom(_imageSize.height());
        if (_roi.top() < 0) _roi.setTop(0);
    }
    if (toEmit) emit(imageSizeChanged());
}

bool Settings::isMask() { return _isMask; }
void Settings::setIsMask(bool isMaskPass)
{
    _batchMask = isMaskPass;
    _isMask = isMaskPass;
    emit(imageSizeChanged());
}

QRect Settings::roi() { return _roi; }
void Settings::setRoi(QRect roiPass)
{
    _roi = roiPass;
    if (_roi.left() < 0) _roi.setLeft(0);
    if (_roi.right() > _imageSize.width()) _roi.setLeft(_imageSize.width());
    if (_roi.bottom() > _imageSize.height()) _roi.setBottom(_imageSize.height());
    if (_roi.top() < 0) _roi.setTop(0);
}

/*-------Session--------------*/


/*-------Batch Processing-------*/

bool Settings::batchFilter() { return _batchFilter; }
void Settings::setBatchFilter(bool batchFilterPass) { _batchFilter = batchFilterPass; }

bool Settings::batchMask() { return _batchMask; }
void Settings::setBatchMask(bool batchMaskPass) { _batchMask = batchMaskPass; }

bool Settings::batchShowImage() { return _batchShowImage; }
void Settings::setBatchShowImage(bool batchShowImagePass) { _batchShowImage = batchShowImagePass; }

bool Settings::batchShowVectors() { return _batchShowVectors; }
void Settings::setBatchShowVectors(bool batchShowVectorsPass) { _batchShowVectors = batchShowVectorsPass; }

bool Settings::batchThreading() { return _batchThreading; }
void Settings::setBatchThreading(bool batchThreadingPass) { _batchThreading = batchThreadingPass; }

/*-------- Vectors ------------*/

QColor Settings::vectorColourFiltered() { return _vectorColourFiltered; }
void Settings::setVectorColourFiltered(QColor vectorColourPass)
{
    _vectorColourFiltered = vectorColourPass;
    emit(vectorSettingChanged());
}
QColor Settings::vectorColourUnfiltered() { return _vectorColourUnfiltered; }
void Settings::setVectorColourUnfiltered(QColor vectorColourPass)
{
    _vectorColourUnfiltered = vectorColourPass;
    emit(vectorSettingChanged());
}
double Settings::vectorScale() { return _vectorScale; }
void Settings::setVectorScale(double vectorScalePass)
{
    _vectorScale = vectorScalePass;
    emit(vectorSettingChanged());
}

double Settings::vectorSub() { return _vectorSub; }
void Settings::setVectorSub(double vectorSubPass)
{
    _vectorSub = vectorSubPass;
    emit(vectorSettingChanged());
}

/* ---------Filter-----------*/

FilterOptions Settings::filterOptions() { return _filterOptions; }
void Settings::setFilterOptions(FilterOptions filterOptionsPass)
{
    _filterOptions = filterOptionsPass;
}

/* ---------Output-----------*/

QString Settings::expName() { return _expName; }
void Settings::setExpName(QString expNamePass)
{
    _expName = expNamePass;
}
QString Settings::outputFolder() { return _outputFolder; }
void Settings::setOutputFolder(QString outputFolderPass)
{
    _outputFolder = outputFolderPass;
}
int Settings::outputFormat() { return _outputFormat; }
void Settings::setOutputFormat(int outputFormatPass)
{
    _outputFormat = outputFormatPass;
}
