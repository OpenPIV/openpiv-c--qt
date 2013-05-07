/*
====================================================================================

File: vectorlist.cpp
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

#include "vectorlist.h"
#include <QHeaderView>
#include <QTableWidget>
#include <iostream>

VectorList::VectorList(QWidget *parent) : QTableWidget(parent)
{
    // Physical dimension of the row (in pixels)
    rowHeight = 20;

    rows = 0;
    rowSelected = 0;
    previousRow = 0;
    setupConnections();
}

VectorList::~VectorList()
{

}

void VectorList::setupConnections()
{
    connect(this, SIGNAL(cellClicked(int,int)), this, SLOT(rowSelect(int,int)));
    connect(this, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(rowDoubleClicked(int,int)));
}

void VectorList::setData(DataContainer *filedataPass)
{
    filedata = filedataPass;
}

void VectorList::populate()
{
    this->clearContents();
    QTableWidgetItem *item;
    this->setColumnCount(2);
    this->setShowGrid(false);
    rows = filedata->size();
    this->setRowCount(rows);
    this->verticalHeader()->hide();
    QStringList labelList;
    labelList.append("Pair Index");
    labelList.append("Filename");
    this->setHorizontalHeaderLabels(labelList);
    this->horizontalHeader()->show();
    this->setSortingEnabled(false);
    for (int i = 0; i < rows; i++)
    {
        item = new QTableWidgetItem(QString("%1").arg(filedata->data(i).index()));
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        this->setItem(i,0,item);
        item = new QTableWidgetItem(filedata->data(i).vectorFile());
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        this->setItem(i,1,item);
        this->setRowHeight(i,rowHeight);
    }
    this->resizeColumnsToContents();
}

void VectorList::update()
{
    for (int i = 0; i < filedata->size(); i++)
    {
        this->item(i,1)->setText(filedata->data(i).vectorFile());
    }
    this->resizeColumnToContents(1);
}

void VectorList::highlightRow(int rowSelectedPass)
{
    rowSelected = rowSelectedPass;
    QFont font0, font1;
    font0 = this->item(rowSelected,0)->font();
    font0.setBold(true);
    this->item(rowSelected,0)->setFont(font0);
    font1 = this->item(rowSelected,1)->font();
    font1.setBold(true);
    this->item(rowSelected,1)->setFont(font1);
}

void VectorList::clearHighlight()
{
    QFont font0, font1;
    font0 = this->item(rowSelected,0)->font();
    font0.setBold(false);
    this->item(rowSelected,0)->setFont(font0);
    font1 = this->item(rowSelected,1)->font();
    font1.setBold(false);
    this->item(rowSelected,1)->setFont(font1);
}

void VectorList::rowSelect(int row, int column)
{
    this->selectRow(row);
    previousRow = row;
}

void VectorList::rowDoubleClicked(int row, int column)
{
    this->selectRow(row);
    emit(fileClicked(row));
}
