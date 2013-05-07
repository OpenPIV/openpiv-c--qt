/*
====================================================================================

File: pivthread.cpp
Description: A thread class that performs PIV operations on a list of image pairs.
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

#include "pivthread.h"

#include <QVector>
#include <QSemaphore>
#include "pivdata.h"
#include <iostream>

// Processing modules
#include "fftcrosscorrelate.h"

PivThread::PivThread(QSemaphore *freePass, QSemaphore *usedPass, QMutex *mutexPass, QVector<PivData*> *dataVectorPass, QList<int> listPass, QObject *parent) : QThread(parent)
{
    free = freePass;
    used = usedPass;
    mutex = mutexPass;
    dataVector = dataVectorPass;
    list = listPass;

    analysisCreated = false;
    settingsSet = false;
    filedataSet = false;
    filter = false;

    abort = false;
}

PivThread::~PivThread()
{
    wait();
    switch(_processor)
    {
    case OpenPIV::FFTCorrelator:
        delete fftCrossCorrelate;
        break;
    default:
        break;
    }

    if (analysisCreated) delete analysis;
}

void PivThread::setSettings(Settings *settingsPass)
{
    settings = settingsPass;
    _processor = settings->processor();
    settingsSet = true;
}

void PivThread::setFileData(DataContainer *filedataPass)
{
    filedata = filedataPass;
    for (int i = 0; i < list.size(); i++)
    {
        filelist.append(filedata->data(list.value(i)));
    }
    filedataSet = true;
}

void PivThread::initializeProcessor()
{
    if (filedataSet && settingsSet)
    {
        switch(_processor)
        {
        case OpenPIV::FFTCorrelator:
            fftCrossCorrelate = new FFTCrossCorrelate(settings,*filedata->gridList());
            break;
        default:
            break;
        }
        filter = settings->batchFilter();
        analysis = new Analysis(settings,filedata);
        filterOptions = settings->filterOptions();
        analysisCreated = true;
    }
}

int PivThread::process()
{
    mutex->lock();
    initializeProcessor();
    mutex->unlock();
    start();

    return 0;
}

void PivThread::stopProcess()
{
    abort = true;
}

void PivThread::run()
{
    PivData *pivData;

    int i = 0;
    while (i < filelist.size() && !abort)
    {
        // Acquires access to the data container object
        free->acquire();

        // Process the image pair based on user settings.  Data are stored in pivData
        switch(_processor)
        {
        case OpenPIV::FFTCorrelator:
            pivData = fftCrossCorrelate->operator()(filelist.value(i));
            break;
        default:
            break;
        }
        // Sets the index of the data (for internal use only)
        pivData->setIndex(filelist.value(i).index());
        // Sets the filename of the data
        pivData->setName(filelist.value(i).imageA());
        // If filtering is enabled then filter the data based on user-defined options
        if (filter) analysis->filterData(pivData,filterOptions);

        // Append the pivData to dataVector
        dataVector->append(pivData);

        // Release hold on the data container
        used->release();
        ++i;
    }
}
