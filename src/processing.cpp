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
    PivData *pivData;
    FFTCrossCorrelate *fftCrossCorrelate = new FFTCrossCorrelate(settings,*filedata->gridList());
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
    initializeThreads();
    for (int i = 0; i < threadList.size(); i++)
    {
        threadList.value(i)->process();
    }
    outputThread->startOutput();
}

void Processing::stopBatch()
{
    for (int i = 0; i < threadList.size(); i++)
    {
        threadList.value(i)->stopProcess();
    }
    outputThread->stopProcess();
}

void Processing::initializeThreads()
{
    int datasize = filedata->size();

    outputThread = new OutputThread(&freePairs,&usedPairs,&mutex,&dataVector,datasize);
    outputThread->setOutputObject(output);

    connect(outputThread, SIGNAL(fileOutput()), batchWindow, SLOT(addToProgress()));

    int i;
    int N = QThread::idealThreadCount();

    for (i = 0; i < N; i++)
    {
        threadList.append(new PivThread(&freePairs,&usedPairs,&mutex,&dataVector,splitList(i,N,datasize)));
        threadList.value(i)->setSettings(settings);
        threadList.value(i)->setFileData(filedata);
        //threadList.value(i)->start();

    }
    connect(outputThread, SIGNAL(doneProcess()), this, SLOT(emitBatchProcessed()));
    //outputThread->start();
    //outputThread->startOutput();
//    for (i = 0; i < N; i++)
//    {
//        threadList.value(i)->wait();
//    }
//    outputThread->wait();


    //return 0;
}

void Processing::emitBatchProcessed()
{
    std::cout << "processed\n";
    emit(batchProcessed());
}

QList<int> Processing::splitList(int currentThread, int totalThreads, int datasize)
{
    QList<int> list;
    for (int i = currentThread; i < datasize; i += totalThreads)
    {
        list.append(i);
    }
    return list;
}

void Processing::processBatchSerial()
{

}
