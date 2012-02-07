/*
====================================================================================

File: imagelist.h
Description: This function inherits QTableWidget and provides the interface between
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

class ImageList : public QTableWidget
{
    Q_OBJECT

    public:
        ImageList(QWidget* parent = 0);
        virtual ~ImageList();

        void setData(DataContainer *filedataPass);
        void setA(bool isAPass);

    public slots:
        void populate();
        void highlightRow(int rowSelected);
        void clearHighlight();

    protected:
        void setupConnections();

    protected slots:
        void rowSelect(int row, int column);
        void rowDoubleClicked(int row, int column);

    signals:
        void fileClicked(QString fileName, int rowSelected);
        void fileClicked(int rowSelected);
        void itemEntered(QTableWidgetItem* item);

    private:
        bool isA;
        DataContainer *filedata;
        int rowHeight;
        int rows;
        int rowSelected;
        int previousRow;
};

#endif // IMAGELIST_H
