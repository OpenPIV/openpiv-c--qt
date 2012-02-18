/*
====================================================================================

File: datacontainer.h
Description: Class that links together the data of a PIV realization and a container
    class for these data: index, image filenames, PIV data filename.
Copyright (C) 2012  OpenPIV (http://www.openpiv.net)

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

#ifndef DATACONTAINER_H
#define DATACONTAINER_H

#include <QStringList>
#include <QObject>
#include <QImage>

#include "settings.h"
#include "pivdata.h"

/*!
    Class that links together the data of a PIV realization.  These data
    include: index, image filenames, PIV data filename.
*/
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


//! Container class for MetaData objects.
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
