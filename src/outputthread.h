/*
====================================================================================

File: outputthread.h
Description: A thread class that outputs PIV data.
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

#ifndef OUTPUTTHREAD_H
#define OUTPUTTHREAD_H

#include <QThread>
#include <QSemaphore>
#include <QVector>
#include <QMutex>

#include "pivdata.h"
#include "output.h"

//! A thread class that performs PIV operations on a list of image pairs.
/*!
  Class is inherited from the QThread class to provide thread-safe parallel processing of PIV datasets.
*/

class OutputThread : public QThread
{
    Q_OBJECT

public:
    //! Constructor
    /*!
        \param Semaphore used to acquire free image
        \param Semaphore used to release used image
        \param Mutex locker
        \param A QVector object of the PivData objects for the resulting data
        \param Number of image pairs
    */
    explicit OutputThread(QSemaphore *freePass, QSemaphore *usedPass, QMutex *mutexPass, QVector<PivData*> *dataVectorPass, int datasizePass, QObject *parent = 0);
    //! Destructor
    virtual ~OutputThread();

    //! Passing an output object for the thread
    void setOutputObject(Output *outputPass);

    //! Starts the output thread
    int startOutput();
    //! Stops the output thread
    void stopProcess();

signals:
    //! Signal is emitted when a file has been written
    void fileOutput();
    //! Signal is emitted when all files have been written
    void doneProcess();

private:
    // Called when the process is started
    void run();

    QSemaphore *free;
    QSemaphore *used;
    QMutex *mutex;
    QVector<PivData*> *dataVector;
    Output *output;
    int datasize;

    bool abort;
};

#endif // OUTPUTTHREAD_H
