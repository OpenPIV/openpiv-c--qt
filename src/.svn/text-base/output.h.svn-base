#ifndef OUTPUT_H
#define OUTPUT_H

#include <QObject>
#include "settings.h"
#include "datacontainer.h"
#include "pivdata.h"

class Output : public QObject
{
    Q_OBJECT

    public:
        explicit Output(Settings *settingsPass, DataContainer *filedataPass, QObject *parent = 0);
        virtual ~Output();

    public slots:
        void outputCurrent();
        void write(PivData *pivData);

    signals:

    protected:
        void outputASCII(PivData *pivData);
        void outputHDF5();

    private:
        Settings *_settings;
        DataContainer *_filedata;
};

#endif // OUTPUT_H
