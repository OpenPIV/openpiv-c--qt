/*
====================================================================================

File: pivthread.h
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

#ifndef PIVTHREAD_H
#define PIVTHREAD_H

#include <QThread>
#include <QSemaphore>
#include <QVector>
#include <QMutex>
#include <QList>
#include "settings.h"
#include "datacontainer.h"
#include "pivdata.h"

#include "analysis.h"

// Processors
#include "fftcrosscorrelate.h"

//! A thread class that performs PIV operations on a list of image pairs.
/*!
  Class is inherited from the QThread class to provide thread-safe parallel processing of PIV datasets.
*/
class PivThread : public QThread
{
    Q_OBJECT

public:
    //! Constructor
    /*!
        \param Semaphore used to acquire free image
        \param Semaphore used to release used image
        \param Mutex locker
        \param A QVector object of the PivData objects for the resulting data
        \param A QList of integers corresponding to indices on which to operate in the DataContainer object
    */
    explicit PivThread(QSemaphore *freePass, QSemaphore *usedPass, QMutex *mutexPass, QVector<PivData*> *dataVectorPass, QList<int> listPass, QObject *parent = 0);
    //! Destructor
    virtual ~PivThread();

    //! For passing a pointer to the global settings object
    void setSettings(Settings *settingsPass);
    //! For passing a pointer to the global DataContainer object
    void setFileData(DataContainer *filedataPass);

    //! Public function to start processing
    int process();
    //! Halt the current process on this thread
    void stopProcess();

protected:
    //! Protected function to initialize the processing engine
    void initializeProcessor();

private:
    void run();

    QSemaphore *free;
    QSemaphore *used;
    QMutex *mutex;
    QVector<PivData*> *dataVector;
    QList<int> list;
    QList<MetaData> filelist;

    Settings *settings;
    bool settingsSet;
    int _processor;
    DataContainer *filedata;
    bool filedataSet;

    bool filter;
    FilterOptions filterOptions;
    Analysis *analysis;
    bool analysisCreated;

    bool abort;

    //List the various types of processors here
    FFTCrossCorrelate *fftCrossCorrelate;
};

#endif // PIVTHREAD_H
