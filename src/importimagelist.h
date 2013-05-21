/*
====================================================================================

File: importimagelist.h
Description: These classes inherit QListWidget and QMenu and provides extra
    functionality (such as pop-ups, etc.) to the base QListWidget class.
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

#ifndef IMPORTIMAGELIST_H
#define IMPORTIMAGELIST_H

#include <QListWidget>
#include <QMouseEvent>
#include <QObject>
#include <QMenu>

//! This class inherits QMenu and provides extra functionality.
class ImportPopUp : public QMenu
{
Q_OBJECT

public:
    //! Constructor
    ImportPopUp();
    //! Destructor
    virtual ~ImportPopUp();

protected:
    //! Initialization of menu items
    void populateMenu();

protected slots:
    //! Menu option to remove the items that the user has selected
    void removeThoseSelected();
    //! Menu option to remove all of the items that the user has not selected
    void removeThoseNotSelected();

signals:
    //! Signal emitted to remove items from the list (if flag is true)
    void removeItems(bool flag);

private:
    QAction *removeSelected;
    QAction *removeNotSelected;
};

//!  This class inherits QListWidget and provides extra functionality.
class ImportImageList : public QListWidget
{
Q_OBJECT

public:
    //! Constructor
    ImportImageList(QWidget *parent = 0);
    //! Destructor
    virtual ~ImportImageList();

protected:
    //! Re-implementation of this function from QListWidget to control right-click action
    void mouseReleaseEvent (QMouseEvent *event);

protected slots:
    //! Handles removal of files from the import list when signaled by the pop-up menu
    void removeFiles(bool flag);

signals:
    //! Signals the actual list to remove the files with a list of indices
    void removeSelected(QList<int> selectedPass);

private:
    ImportPopUp *popUp;
    QList<int> selected;

};

#endif // IMPORTIMAGELIST_H
