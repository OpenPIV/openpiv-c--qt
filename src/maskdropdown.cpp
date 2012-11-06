/*
====================================================================================

File: maskdropdown.cpp
Description: Class handling the mask drop down menu based on QMenu
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

#include "maskdropdown.h"
#include "pivdisplay.h"

#include <QMenu>

MaskDropDown::MaskDropDown(QWidget *parent) : QMenu(parent)
{
    loaded = false;
    QAction *tempAction;
    tempAction = new QAction("Import mask...", this);
    actionList.append(tempAction);
    tempAction = new QAction("Show grid", this);
    tempAction->setCheckable(true);
    tempAction->setChecked(false);
    actionList.append(tempAction);
    tempAction = new QAction("Clear mask", this);
    actionList.append(tempAction);

    this->addActions(actionList);
    actionList.value(0)->setEnabled(loaded);
    connect(actionList.value(0), SIGNAL(triggered()), this, SLOT(importTrigger()));
    actionList.value(1)->setEnabled(loaded);
    connect(actionList.value(1), SIGNAL(triggered(bool)), this, SLOT(gridTrigger(bool)));
    actionList.value(2)->setEnabled(loaded);
    connect(actionList.value(2), SIGNAL(triggered()), this, SLOT(clearTrigger()));
}

MaskDropDown::~MaskDropDown()
{

}

void MaskDropDown::imageLoaded()
{
    loaded = true;
    actionList.value(0)->setEnabled(loaded);
}

void MaskDropDown::importTrigger()
{
    emit(importMaskClicked());
}

void MaskDropDown::maskLoaded()
{
    actionList.value(1)->setEnabled(true);
    actionList.value(1)->setChecked(true);
    actionList.value(2)->setEnabled(true);
    emit (gridToggled(true));
}

void MaskDropDown::gridTrigger(bool checkState)
{
    emit(gridToggled(checkState));
    actionList.value(1)->setChecked(checkState);
}

void MaskDropDown::clearTrigger()
{
    emit(clearMask(false));
}
