#ifndef DATACONTAINER_H
#define DATACONTAINER_H

#include <QStringList>
#include <QObject>
#include <QImage>

#include "settings.h"
#include "pivdata.h"

class MetaData
{
    public:
        MetaData();
        virtual ~MetaData();

        int index();
        void setIndex(int indexPass);
        QString imageA();
        void setImageA(QString imageAPass);
        QString imageB();
        void setImageB(QString imageBPass);
        bool hasVectors();
        QString vectorFile();
        void setVectorFile(QString vectorFilePass);

    protected:


    private:
        int _index;
        QString _imageA;
        QString _imageB;
        QString _vectorFile;
};

class DataContainer : public QObject
{

    Q_OBJECT

    public:
        DataContainer(Settings *settingsPass, QObject *parent = 0);
        virtual ~DataContainer();

        void append(QStringList listA, QStringList listB);

        int size();
        MetaData data(int index);
        void setData(int index, MetaData dataPass);

        //void setGrid(QList<QPoint> gridListPass);
        QList<QPoint>* gridList();
        QImage* mask();

        // Current as defined by what's in the display
        int currentIndex();
        bool isCurrentA();
        MetaData currentData();
        bool hasCurrentVectors();
        PivData *currentPivData();

    public slots:
        void setCurrentIndex(int currentIndexPass, bool isAPass);
        void setCurrentPivData(PivData *pivDataPass);
        void setVectorFile(int index, QString fileName);
        void importMask();

    signals:
        void imagesImported();
        void vectorListUpdated();
        void maskLoaded();

    protected:
        void createGrid();

    private:
        Settings *settings;

        QList<MetaData> _container;
        QList<QPoint> _gridList;
        int _currentIndex;
        bool _isA;

        bool _currentVectors;
        PivData *_currentPivData;
        bool pivDataResponsible;

        QImage *maskData;
        bool maskCreated;

};

#endif // DATACONTAINER_H
