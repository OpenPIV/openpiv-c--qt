#include <QStringList>
#include <QObject>
#include <iostream>
#include <QFileDialog>

#include "settings.h"
#include "datacontainer.h"
#include "pivdata.h"
#include "generategrid.h"

/*----------------------------MetaData class---------------------------------------------*/

MetaData::MetaData()
{
    _index = -1;
    _imageA = "";
    _imageB = "";
    _vectorFile = "";
}

MetaData::~MetaData()
{
}

int MetaData::index(){ return _index; }
void MetaData::setIndex(int indexPass) { _index = indexPass; }

QString MetaData::imageA() { return _imageA; }
void MetaData::setImageA(QString imageAPass) { _imageA = imageAPass; }

QString MetaData::imageB() { return _imageB; }
void MetaData::setImageB(QString imageBPass) { _imageB = imageBPass; }

bool MetaData::hasVectors()
{
    return !(_vectorFile.isEmpty());
}

QString MetaData::vectorFile() { return _vectorFile; }
void MetaData::setVectorFile(QString vectorFilePass) { _vectorFile = vectorFilePass; }

/*------------------------DataContainer class------------------------------------------*/

DataContainer::DataContainer(Settings *settingsPass, QObject *parent) : QObject(parent)
{
    settings = settingsPass;
    _currentIndex = -1;
    _isA = true;
    _currentVectors = false;
    pivDataResponsible = false;
    maskCreated = false;
}

DataContainer::~DataContainer()
{
    if (pivDataResponsible) delete _currentPivData;
    if (maskCreated) delete maskData;
}

void DataContainer::append(QStringList listA, QStringList listB)
{
    int listLength;
    int oldListLength = _container.size();
    MetaData data;
    if (listA.size() > listB.size()) listLength = listB.size();
    else listLength = listA.size();

    for (int i = 0; i < listLength; i++)
    {
        data.setImageA(listA.value(i));
        data.setImageB(listB.value(i));
        data.setIndex(oldListLength + i);
        data.setVectorFile("");
        _container.append(data);
    }
    emit(imagesImported());
}

int DataContainer::size()
{
    return _container.size();
}

MetaData DataContainer::data(int index)
{
    return _container.value(index);
}

MetaData DataContainer::currentData()
{
    return _container.value(_currentIndex);
}

void DataContainer::setVectorFile(int index, QString fileName)
{
    MetaData metaData = _container.value(index);
    metaData.setVectorFile(fileName);
    _container.replace(index,metaData);
    emit(vectorListUpdated());
}

QImage* DataContainer::mask()
{
    return maskData;
}

void DataContainer::importMask()
{
    QString maskName;
    QImage *tempData;
    maskName = QFileDialog::getOpenFileName();
    if (maskCreated) delete maskData;
    tempData = new QImage(maskName);

    QSize imageSize;
    imageSize = settings->imageSize();

    if (!tempData->isNull())
    {
        maskData = new QImage(tempData->scaled(imageSize, Qt::IgnoreAspectRatio));
        maskCreated = true;
        settings->setIsMask(true);
        emit (maskLoaded());
    }
    delete tempData;
}

//void DataContainer::setGrid(QList<QPoint> gridListPass)
//{
//    _gridList = gridListPass;
//}

QList<QPoint>* DataContainer::gridList()
{
    if (settings->isMask() && maskCreated) _gridList = generateGrid(maskData,settings);
    else _gridList = generateGrid(settings);
    return &_gridList;
}

int DataContainer::currentIndex()
{
    return _currentIndex;
}

void DataContainer::setCurrentIndex(int currentIndexPass, bool isAPass)
{
    _currentIndex = currentIndexPass;
    _isA = isAPass;
    if (_container.value(_currentIndex).hasVectors())
    {
        if (pivDataResponsible)
        {
            delete _currentPivData;
            pivDataResponsible = false;
        }
        _currentPivData = new PivData;
        _currentPivData->read(_currentIndex,_container.value(_currentIndex).vectorFile(),settings->imageSize().height());
        pivDataResponsible = true;
        _currentVectors = true;
    }
}

bool DataContainer::isCurrentA()
{
    return _isA;
}

void DataContainer::setCurrentPivData(PivData *pivDataPass)
{
    if (pivDataResponsible) delete _currentPivData;
    _currentPivData = pivDataPass;
    pivDataResponsible = true;
    if (!_currentPivData->isEmpty())
    {
        _currentVectors = true;
    }
    else
    {
        _currentVectors = false;
    }
}

bool DataContainer::hasCurrentVectors()
{
    if (pivDataResponsible)
    {
        if (_currentPivData->index() == _currentIndex) return _currentVectors;
        else return false;
    }
    else return false;
}

PivData* DataContainer::currentPivData()
{
    return _currentPivData;
}


