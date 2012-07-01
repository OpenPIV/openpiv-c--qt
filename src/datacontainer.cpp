/*
====================================================================================

File: datacontainer.h
Description: Class that links together the data of a PIV realization and a container
    class for these data: index, image filenames, PIV data filename.
Copyright (C) 2012  OpenPIV (http://www.openpiv.net)

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
    // Initializing properties of MetaData
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
    // Setting local pointer to global Settings object
    settings = settingsPass;

    // Initializing variables
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

    /* Based on other functions, the lists should always be the same size.
      However, if they have ended up of different sizes, we take the smallest. */
    if (listA.size() > listB.size()) listLength = listB.size();
    else listLength = listA.size();

    // Loop over the new pair of lists appending data to the container
    for (int i = 0; i < listLength; i++)
    {
        data.setImageA(listA.value(i));
        data.setImageB(listB.value(i));
        data.setIndex(oldListLength + i);
        data.setVectorFile("");
        _container.append(data);
    }
    // Emit signal that images have been imported
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
    // Launch file dialog to get image mask file name
    maskName = QFileDialog::getOpenFileName();
    if (maskCreated) delete maskData;
    tempData = new QImage(maskName);

    QSize imageSize;
    imageSize = settings->imageSize();

    if (!tempData->isNull())
    {
        // Scales the mask image to the image size.  May want to change this functionality in the future.
        maskData = new QImage(tempData->scaled(imageSize, Qt::IgnoreAspectRatio));
        maskCreated = true;
        /* If a mask is loaded, it is assumed that the user wants to use the mask so it we set this
           in the global Settings object */
        settings->setIsMask(true);
        // Signal emitted to say the mask image has been loaded
        emit (maskLoaded());
    }
    delete tempData;
}

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
    // Need to check if the current image pair has an associated vector file to dispaly
    if (_container.value(_currentIndex).hasVectors())
    {
        // Deleting the current PivData object if there is one
        if (pivDataResponsible)
        {
            delete _currentPivData;
            pivDataResponsible = false;
        }
        // Reading the vector file associated with the current image pair
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


