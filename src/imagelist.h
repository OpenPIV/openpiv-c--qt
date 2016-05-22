/*
====================================================================================

File: imagelist.h
Description: This class inherits QTableWidget and provides the interface between
    the user and the image lists.  This class is typically instantiated twice for
    the 'A' images and the 'B' images.
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

#ifndef IMAGELIST_H
#define IMAGELIST_H

#include <QObject>
#include <QTableWidget>

#include "datacontainer.h"

//!  Class inherited from QTableWidget to handle the display of each image list.
/*!
    This class inherits QTableWidget and provides the interface between
    the user and the image lists.  This class is typically instantiated twice for
    the 'A' images and the 'B' images.
*/

class ImageList : public QTableWidget
{

Q_OBJECT

public:
    //! Constructor
    ImageList(QWidget* parent = 0);

    //! Destructor
    virtual ~ImageList();

    //! A pointer to the filedata is passed to the list upon initialization
    /*!
        \param Pointer to the global DataContainer object
    */
    void setData(DataContainer *filedataPass);
    //! Determines if the current list is the A or B list of images
    void setA(bool isAPass);

public slots:
    //! Populates the display list with the data in the global DataContainer object
    void populate();
    //! Highlights the selected row
    void highlightRow(int rowSelected);
    //! Clears the highligh (for example if B is selected after A was previously selected)
    void clearHighlight();

protected:
    //! Protected function to setup the signal and slot connections
    void setupConnections();

protected slots:
    //! Protected slot to select the row in the table internally
    void rowSelect(int row, int column);
    //! Protected slot called when a row is double clicked
    void rowDoubleClicked(int row, int column);

signals:
    //! Signal is emitted when a row is double-clicked
    void fileClicked(QString fileName, int rowSelected);
    //! Signal is emitted when a row is double-clicked
    void fileClicked(int rowSelected);

private:
    bool isA;
    DataContainer *filedata;
    int rowHeight;
    int rows;
    int rowSelected;
    int previousRow;

};

#endif // IMAGELIST_H
