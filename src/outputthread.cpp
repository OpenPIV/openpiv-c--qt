/*
====================================================================================

File: outputthread.cpp
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

#include "outputthread.h"
#include "pivdata.h"
#include <QCoreApplication>
#include <iostream>
#include <fstream>

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
    // Sets the class variable abort to true which exits the process loop
    abort = true;
}

int OutputThread::startOutput()
{
    start();
    return 0;
}

void OutputThread::run()
{
    PivData *pivData;

    int i = 0;
    while (i < datasize && !abort)
    {
        // Acquire data from the data container wrapped by the semaphore
        used->acquire();

        pivData = dataVector->front();
        dataVector->pop_front();

        free->release();
        // Release ownership of the data container once data object has been removed

        // Data object is written by the output object
        output->write(pivData);

        // Data object is deleted
        delete pivData;

        // Signal is emitted that data have been written to another file
        emit(fileOutput());
        ++i;
    }
    // Signal emitted when loop is completed
    emit(doneProcess());
}
