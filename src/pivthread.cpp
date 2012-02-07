#include "pivthread.h"

#include <QVector>
#include <QSemaphore>
#include "pivdata.h"
#include <iostream>

// Processing modules
#include "fftcrosscorrelate.h"

//PivThread::PivThread(QSemaphore *freePass, QSemaphore *usedPass, QMutex *mutexPass, QVector<float> *dataVectorPass, QList<int> listPass, QObject *parent) : QThread(parent)
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
    //emit(finished());
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
        free->acquire();
        //mutex->lock();

        switch(_processor)
        {
        case OpenPIV::FFTCorrelator:
            pivData = fftCrossCorrelate->operator()(filelist.value(i));
            break;
        default:
            break;
        }
        pivData->setIndex(filelist.value(i).index());
        if (filter) analysis->filterData(pivData,filterOptions);

        dataVector->append(pivData);

        //mutex->unlock();
        used->release();
        ++i;
    }
}
