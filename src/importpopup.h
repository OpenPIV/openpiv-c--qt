/*
====================================================================================

File: importpopup.h
Description: This class inherits QMenu and provides a popup menu for the import
    images window.
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

#ifndef IMPORTPOPUP_H
#define IMPORTPOPUP_H

#include <QObject>
#include <QtGui/QMenu>

class ImportPopUp : public QMenu
{
    Q_OBJECT

    public:
        ImportPopUp();
        virtual ~ImportPopUp();

    protected:
        void populateMenu();

    protected slots:
        void removeThoseSelected();
        void removeThoseNotSelected();

    signals:
        void removeItems(bool flag);

    private:
        QAction *removeSelected;
        QAction *removeNotSelected;
};

#endif // IMPORTPOPUP_H
