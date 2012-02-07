#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <QObject>

#include "pivdata.h"
#include "settings.h"
#include "filteroptions.h"
#include "datacontainer.h"

class Analysis : public QObject
{
    Q_OBJECT

    public:
        Analysis(Settings *settingsPass, DataContainer *fileDataPass, QObject *parent = 0);
        virtual ~Analysis();

        void filterData(PivData *pivData, FilterOptions filterOptions);

    public slots:
        void filterCurrent();

    signals:
        void currentFiltered();

    private:
        Settings *settings;
        DataContainer *filedata;
};

#endif // ANALYSIS_H
