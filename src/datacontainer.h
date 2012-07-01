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
    //! Empty constructor for a MetaData boject
    MetaData();
    //! Empty destructor for a MetaData boject
    virtual ~MetaData();

    //! Returns the index of the current image pair
    int index();
    //! Sets the index of the current image pair
    void setIndex(int indexPass);
    //! Returns the filename of image A
    QString imageA();
    //! Sets the filename of image A
    void setImageA(QString imageAPass);
    //! Returns the filename of image B
    QString imageB();
    //! Sets the filename of image B
    void setImageB(QString imageBPass);
    //! Returns true if the vectors have been computed for the image pair, otherwise false.
    bool hasVectors();
    //! Returns the filename of the computed vector file
    QString vectorFile();
    //! Sets the filename of the vector map
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

    //! Constructor
    /*!
        \param settingsPass is a pointer to the program wide Settings object
    */
    DataContainer(Settings *settingsPass, QObject *parent = 0);
    //! Empty destructor
    virtual ~DataContainer();

    //! Appends images lists to the data container
    /*!
        \param listA is the list of A images
        \param listB is the list of B images
    */
    void append(QStringList listA, QStringList listB);

    //! Returns the total number of image pairs
    int size();
    //! Returns a copy of the MetaData object at the given index
    MetaData data(int index);
    //! Sets the provided MetaData object at the provided index
    void setData(int index, MetaData dataPass);

    //! Returns a pointer to the list of grid points at which velocities are computed
    QList<QPoint>* gridList();
    //! Returns a pointer to the user supplied mask image
    QImage* mask();

    // Current as defined by what's in the display
    //! Returns the integer value of the current index (current defined by the display)
    int currentIndex();
    //! Returns true if the A frame is currently displayed
    bool isCurrentA();
    //! Returns a MetaData object of the current pair (current defined by the display)
    MetaData currentData();
    //! Returns true if vectors have been computed for the current pair (current defined by the display)
    bool hasCurrentVectors();
    //! Returns the PivData of the current image pair (current defined by the display)
    PivData *currentPivData();

public slots:
    //! Sets the current index based on what the user wants to see in the current display
    /*!
        \param currentIndexPass integer value corresponding to the index of the displayed image
        \param isAPass is true if the user has selected the 'A' image of an image pair, false otherwise
    */
    void setCurrentIndex(int currentIndexPass, bool isAPass);

    //! Sets the current data to the passed PivData object
    /*!
        \param pivDataPass is a pointer to the current PivData object
    */
    void setCurrentPivData(PivData *pivDataPass);

    //! Sets the file name o fhte vector file associated with the image pair
    /*!
        \param index
        \param fileName is the name of the vector file associated with the image pair
    */
    void setVectorFile(int index, QString fileName);

    //! Runs the file dialog and stores mask image object
    void importMask();

signals:
    //! Signal emitted after images have been appended to the list
    void imagesImported();
    //! Signal emitted after a vector file has been associated with an image pair
    void vectorListUpdated();
    //! Signal emitted after an image mask has been selected
    void maskLoaded();

private:
    Settings *settings;

    QList<MetaData> _container;
    QList<QPoint> _gridList;
    int _currentIndex;
    bool _isA;

    bool _currentVectors;
    PivData *_currentPivData;
    // True if this object has taken ownership of a PivData object
    bool pivDataResponsible;

    QImage *maskData;
    bool maskCreated;

};

#endif // DATACONTAINER_H
