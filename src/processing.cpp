/*
====================================================================================

File: processing.cpp
Description: The processing class handles processing operations.  These operations
    include batch processing in addition to the processing of individual image pairs.
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

#include <QObject>
#include <QPoint>
#include <QStringList>

#include <QSemaphore>
#include <QThread>
#include <QMutex>
#include <iostream>
#include <fstream>

#include "settings.h"
#include "processing.h"
#include "imagedata.h"
#include "pivdata.h"
#include "pivthread.h"
#include "outputthread.h"
#include "batchwindow.h"

// Processing modules
//// Correlators
#include "fftcrosscorrelate.h"

QSemaphore freePairs(10);
QSemaphore usedPairs;
QMutex mutex;

Processing::Processing(Settings *settingsPass, DataContainer *filedataPass, QObject *parent) : QObject(parent)
{
    filedata = filedataPass;
    settings = settingsPass;

    // Initialization of the batch window user interface
    batchWindow = new BatchWindow;
    batchWindow->setSettings(settings);

    connect(batchWindow, SIGNAL(startProcess()), this, SLOT(processBatch()));
    connect(batchWindow, SIGNAL(stopProcess()), this, SLOT(stopBatch()));

    output = new Output(settings,filedata);

    threadsCreated = false;
}

Processing::~Processing()
{
    delete batchWindow;
    deleteThreads();
    delete output;
}

void Processing::deleteThreads()
{
    // Deletes the thread objects if they were created
    if (threadsCreated)
    {
        while (threadList.size() > 0)
            delete threadList.takeFirst();

        delete outputThread;
        threadsCreated = false;
    }
}

void Processing::launchBatchWindow()
{
    batchWindow->refresh();
    batchWindow->show();
}

void Processing::processCurrentImagePair()
{
    if (!settings->outputFolder().isEmpty()) processCurrent();
}

void Processing::processCurrent()
{
    // This requires further refactoring still since a FFTCrossCorrelate object need not be created
    // if a different method is being used.
    PivData *pivData;
    FFTCrossCorrelate *fftCrossCorrelate = new FFTCrossCorrelate(settings,*filedata->gridList());

    // Some form of feedback should be provided to the user since larger images can take more than
    // a couple of seconds to process and display.

    switch(settings->processor())
    {
    case OpenPIV::FFTCorrelator:
        pivData = fftCrossCorrelate->operator()(filedata->currentData());
        break;
    default:
        break;
    }
    delete fftCrossCorrelate;
    pivData->setIndex(filedata->currentIndex());
    pivData->setName(filedata->currentData().imageA());
    filedata->setCurrentPivData(pivData);
    emit(currentProcessed());
}

void Processing::processBatch()
{
    batchWindow->setProgressRange(filedata->size());
    if (settings->batchThreading()) processBatchParallel();
    else processBatchSerial();
}

void Processing::processBatchParallel()
{
    // Initializes the processing threads
    initializeThreads();
    // Starts each thread
    for (int i = 0; i < threadList.size(); i++)
    {
        threadList.value(i)->process();
    }
    // Starts the ouptut thread
    outputThread->startOutput();
}

void Processing::stopBatch()
{
    // Stops the process of each processing thread
    for (int i = 0; i < threadList.size(); i++)
    {
        threadList.value(i)->stopProcess();
    }
    // Stops the output thread
    outputThread->stopProcess();
}

void Processing::initializeThreads()
{
    int datasize = filedata->size();

    // Only ever need 1 output thread
    outputThread = new OutputThread(&freePairs,&usedPairs,&mutex,&dataVector,datasize);
    outputThread->setOutputObject(output);

    // Connecting progress signals from output thread to progress bar in batch window
    connect(outputThread, SIGNAL(fileOutput()), batchWindow, SLOT(addToProgress()));

    int i;
    // Since the output thread requires very little CPU availablity, a processing thread
    // is created for each CPU on the system.
    int N = QThread::idealThreadCount();

    // Each thread is initialized and stored in the the threadList
    for (i = 0; i < N; i++)
    {
        threadList.append(new PivThread(&freePairs,&usedPairs,&mutex,&dataVector,splitList(i,N,datasize)));
        threadList.value(i)->setSettings(settings);
        threadList.value(i)->setFileData(filedata);
    }
    connect(outputThread, SIGNAL(doneProcess()), this, SLOT(emitBatchProcessed()));
}

void Processing::emitBatchProcessed()
{
    emit(batchProcessed());
}

QList<int> Processing::splitList(int currentThread, int totalThreads, int datasize)
{
    // The list is staggered so that data are still computed relatively sequentially
    // even though the list is spread over multiple threads.
    QList<int> list;
    for (int i = currentThread; i < datasize; i += totalThreads)
    {
        list.append(i);
    }
    return list;
}

void Processing::processBatchSerial()
{
    // Does nothing now
}
