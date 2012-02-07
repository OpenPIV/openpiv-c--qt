#include "outputthread.h"
#include "pivdata.h"
#include <QCoreApplication>
#include <iostream>
#include <fstream>

//OutputThread::OutputThread(QSemaphore *freePass, QSemaphore *usedPass, QMutex *mutexPass, QVector<float> *dataVectorPass, int datasizePass, QObject *parent) : QThread(parent)
OutputThread::OutputThread(QSemaphore *freePass, QSemaphore *usedPass, QMutex *mutexPass, QVector<PivData*> *dataVectorPass, int datasizePass, QObject *parent) : QThread(parent)
{
    free = freePass;
    used = usedPass;
    mutex = mutexPass;
    dataVector = dataVectorPass;
    datasize = datasizePass;

    abort = false;
}

OutputThread::~OutputThread()
{
    wait();
}

void OutputThread::setOutputObject(Output *outputPass)
{
    output = outputPass;
}

void OutputThread::stopProcess()
{
    abort = true;
}

int OutputThread::startOutput()
{
    start();
//    wait();
    return 0;
}

void OutputThread::run()
{
    PivData *pivData;
//    std::ofstream file;

//    file.open("/home/zjtaylor/share/trial.txt");
    int i = 0;
    while (i < datasize && !abort)
    {
        used->acquire();
        //mutex->lock();
        pivData = dataVector->front();
        dataVector->pop_front();
        //mutex->unlock();
        free->release();
        output->write(pivData);
        delete pivData;
        emit(fileOutput());
        ++i;
    }
    emit(doneProcess());
    //file.close();
}
