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

class Processing : public QObject
{
    Q_OBJECT

    public:
        explicit Processing(Settings *settingsPass, DataContainer *filedataPass, QObject *parent = 0);
        virtual ~Processing();

    public slots:
        void processCurrentImagePair();
        void processBatch();

    signals:
        void currentProcessed();
        void batchProcessed();

    protected slots:
        void launchBatchWindow();
        void emitBatchProcessed();
        void stopBatch();

    protected:
        void processCurrent();
        void processBatchSerial();
        void processBatchParallel();
        void initializeThreads();
        void deleteThreads();


        QList<int> splitList(int currentThread, int totalThreads, int datasize);

    private:
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
