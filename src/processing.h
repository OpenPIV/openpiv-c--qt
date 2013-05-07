/*
====================================================================================

File: processing.h
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

#ifndef PROCESSING_H
#define PROCESSING_H

#include <QStringList>
#include <QPoint>
#include <QObject>

#include <QSemaphore>
#include <QThread>
#include <QMutex>

#include "imagedata.h"
#include "pivdata.h"
#include "datacontainer.h"
#include "settings.h"
#include "pivengine.h"
#include "fftcrosscorrelate.h"
#include "batchwindow.h"
#include "output.h"
#include "pivthread.h"
#include "outputthread.h"

//! Process handling class
/*!
    The processing class handles processing operations.  These operations
    include batch processing in addition to the processing of individual image pairs.
*/
class Processing : public QObject
{

    Q_OBJECT

public:
    //! Constructor
    /*!
       Constructor for the processing class

       \param Global settings object
       \param Global data container object
    */
    explicit Processing(Settings *settingsPass, DataContainer *filedataPass, QObject *parent = 0);
    //! Destructor
    virtual ~Processing();

public slots:
    //! Processes the current image pair upon receipt of signal
    void processCurrentImagePair();
    //! Processes the entire batch of images upon receipt of signal
    void processBatch();

signals:
    //! Emits a signal when the current image has been processed
    void currentProcessed();
    //! Emits a signal when the entire batch has been processed
    void batchProcessed();

protected slots:
    //! Launches the batch processing window
    void launchBatchWindow();
    //! Handler for emitted the batchProcessed signal
    void emitBatchProcessed();
    //! Stops the batch processing
    void stopBatch();

protected:
    //! Process the current image pair
    void processCurrent();
    //! Process the entire batch without multiple threads
    void processBatchSerial();
    //! Process the entire batch with multiple threads
    void processBatchParallel();
    //! Initialize the processing and output threads
    void initializeThreads();
    //! Delete the processing and output threads
    void deleteThreads();
    //! Returns a QList of indices for a given thread that are divided among the processing threads
    QList<int> splitList(int currentThread, int totalThreads, int datasize);

private:
    // Local pointers to global data
    Settings *settings;
    DataContainer *filedata;
    BatchWindow *batchWindow;

    QVector<PivData*> dataVector;
    QList<PivThread*> threadList;
    Output *output;
    OutputThread* outputThread;
    bool threadsCreated;
};

#endif // PROCESSING_H
