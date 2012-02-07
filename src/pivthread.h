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

class PivThread : public QThread
{
    Q_OBJECT

    public:
        explicit PivThread(QSemaphore *freePass, QSemaphore *usedPass, QMutex *mutexPass, QVector<PivData*> *dataVectorPass, QList<int> listPass, QObject *parent = 0);
        virtual ~PivThread();

        void setSettings(Settings *settingsPass);
        void setFileData(DataContainer *filedataPass);

        int process();
        void stopProcess();

    protected:
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

        //Processors
        FFTCrossCorrelate *fftCrossCorrelate;
};

#endif // PIVTHREAD_H
