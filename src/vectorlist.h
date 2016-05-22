/*
====================================================================================

File: vectorlist.h
Description: Inherits QTableWidget to provide user interface to the list of vectors.
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

#ifndef VECTORLIST_H
#define VECTORLIST_H

#include <QTableWidget>
#include "datacontainer.h"

//! Inherits QTableWidget to provide user interface to the list of vectors.

class VectorList : public QTableWidget
{

    Q_OBJECT

public:
    //! Constructor
    explicit VectorList(QWidget *parent = 0);
    //! Destructor
    virtual ~VectorList();

    //! Sets the global DataContainer object
    void setData(DataContainer *filedataPass);

signals:
    //! Emits a signal when a file has been selected (not simply highlighted)
    void fileClicked(int rowSelected);

public slots:
    //! Populates the list with the data contained in the DataContainer object
    void populate();
    //! Updates the list if the DataContainer object has been changed elsewhere
    void update();
    //! Highlights row and changes the text to bold-face
    void highlightRow(int rowSelected);
    //! Clears the highlighting
    void clearHighlight();

protected:
    void setupConnections();

protected slots:
    void rowSelect(int row, int column);
    void rowDoubleClicked(int row, int column);

private:
    DataContainer *filedata;
    int rowHeight;
    int rows;
    int rowSelected;
    int previousRow;
};

#endif // VECTORLIST_H
