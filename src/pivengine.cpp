#include "pivengine.h"
#include <iostream>

#include <QList>
#include <QPoint>
#include <QMutex>

// Subpixel interpolators
#include "gaussiansubpixel.h"
#include "settings.h"

PivEngine::PivEngine(Settings *settingsPass, QList<QPoint> gridPass)
{
//    _settings = settingsPass;
    _intLengthX = settingsPass->intLengthX();
    _intLengthY = settingsPass->intLengthY();
    _detector = settingsPass->detector();
    _cmap = new double [_intLengthX*2*_intLengthY*2+1];
    cmapCreated = true;
    _grid = gridPass;
}

PivEngine::~PivEngine()
{
    if (cmapCreated)
    {
        delete [] _cmap;
        cmapCreated = false;
    }
    if (imagesCreated)
    {
        delete _imageA;
        delete _imageB;
        imagesCreated = false;
    }
}

/* --------- Access functions ----------*/
MetaData PivEngine::data() { return _data; }
//Settings* PivEngine::settings() { return _settings; }
int PivEngine::intLengthX() { return _intLengthX; }
int PivEngine::intLengthY() { return _intLengthY; }
int PivEngine::detector() { return _detector; }

QList<QPoint> PivEngine::grid() { return _grid; }

bool PivEngine::imagesAvailable() { return imagesCreated; }
ImageData* PivEngine::imageA() { return _imageA; }
ImageData* PivEngine::imageB() { return _imageB; }
double PivEngine::meanImageA() { return _meanImageA; }
double PivEngine::meanImageB() { return _meanImageB; }

double* PivEngine::cmap() { return _cmap; }
/* --------- Access functions ----------*/

PivData* PivEngine::operator ()(MetaData dataPass)
{
    PivData *pivData;
    pivData = new PivData;
    _data = dataPass;
    loadImages();
    PivPointData pointData;
    QList<PivPointData> _dataList;
    for (int i = 0; i < _grid.size(); i++)
    {
        _meanImageA = this->imageMean(_imageA,_grid.value(i).ry(),_grid.value(i).rx());
        _meanImageB = this->imageMean(_imageB,_grid.value(i).ry(),_grid.value(i).rx());
        pointData = this->velocity(_grid.value(i).ry(),_grid.value(i).rx());
        if (_meanImageA > _meanImageB) pointData.intensity = _meanImageB;
        else pointData.intensity = _meanImageA;
        _dataList.append(pointData);
    }
    pivData->setLists(_dataList);
    delete _imageA;
    delete _imageB;
    imagesCreated = false;
    return pivData;
}

PivPointData PivEngine::velocity(int topLeftRow, int topLeftColumn)
{
    PivPointData pointData;

    if (crossCorrelate(topLeftRow, topLeftColumn))
    {
        switch (_detector)
        {
        case OpenPIV::GaussianSubPixel:
            pointData = gaussianSubPixel(_cmap,_intLengthX,_intLengthY);
            break;
        default:
            break;
        }
    }
    else
    {
        pointData.u = 0.0;
        pointData.v = 0.0;
        pointData.snr = 0.0;
    }

    pointData.filtered = false;
    pointData.x = topLeftColumn;
    pointData.y = topLeftRow;

    return pointData;
}

void PivEngine::loadImages()
{
    _imageA = new ImageData;
    _imageB = new ImageData;

    if (_imageA->read(_data.imageA()) && _imageB->read(_data.imageB())) imagesCreated = true;
    else
    {
        delete _imageA;
        delete _imageB;
        imagesCreated = false;
    }
}

bool PivEngine::crossCorrelate(int topLeftRow, int topLeftColumn)
{
    // This is the only necssary function to change when this class is inherited.
    return false;
}

double PivEngine::imageMean(ImageData *image, int topLeftRow, int topLeftColumn)
{
    double mean = 0.0;
    int count = 0;
    double* data = image->buffer();
    int imageWidth = image->width();

    int imageI, imageJ;

    for (imageI = topLeftRow; imageI < (topLeftRow + _intLengthY); imageI++)
    {
        for (imageJ = topLeftColumn; imageJ < (topLeftColumn + _intLengthX); imageJ++)
        {
            mean += data[imageWidth*imageI + imageJ];
            count++;
        }
    }
    mean = mean / count;

    return mean;
}
