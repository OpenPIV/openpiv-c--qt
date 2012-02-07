/*
====================================================================================

File: importimagelist.cpp
Description: This class inherits QListWidget and provides extra functionality (such as
    pop-ups, etc.) to the base QListWidget class.
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

#include "importimagelist.h"
#include "importimages.h"

#include <QListWidget>
#include <QMouseEvent>
#include <QMenu>

ImportImageList::ImportImageList(QWidget *parent) : QListWidget(parent)
{
    popUp = new ImportPopUp;
    connect(popUp, SIGNAL(removeItems(bool)), this, SLOT(removeFiles(bool)));
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

ImportImageList::~ImportImageList()
{
    delete popUp;
}

void ImportImageList::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        popUp->popup(event->globalPos());
}

void ImportImageList::removeFiles(bool flag)
{
    int i = 0;
    selected.clear();
    if (flag)
    {
        while (i < this->count())
        {
            if (this->item(i)->isSelected()) selected.append(i);
            i++;
        }
    }
    else
    {
        while (i < this->count())
        {
            if (!this->item(i)->isSelected()) selected.append(i);
            i++;
        }
    }
    emit(removeSelected(selected));
}

ImportPopUp::ImportPopUp() : QMenu()
{
    populateMenu();
}

ImportPopUp::~ImportPopUp()
{

}

void ImportPopUp::populateMenu()
{
    /* These allow the user to exclude some of the available images from being added
       to the image lists. */
    removeSelected = new QAction("Remove selected items",this);
    removeNotSelected = new QAction("Remove items not selected",this);
    connect(removeSelected, SIGNAL(triggered(bool)), this, SLOT(removeThoseSelected()));
    connect(removeNotSelected, SIGNAL(triggered(bool)), this, SLOT(removeThoseNotSelected()));
    this->addAction(removeSelected);
    this->addAction(removeNotSelected);
}

void ImportPopUp::removeThoseSelected()
{
    emit(removeItems(true));
}

void ImportPopUp::removeThoseNotSelected()
{
    emit(removeItems(false));
}
