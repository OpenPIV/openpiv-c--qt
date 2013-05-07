/*
====================================================================================

File: maskdropdown.h
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

#ifndef MASKDROPDOWN_H
#define MASKDROPDOWN_H

#include <QObject>
#include <QMenu>
#include "pivdisplay.h"

//!  Class handling the mask drop down menu based on QMenu.

class MaskDropDown : public QMenu
{
Q_OBJECT

public:
    //! Constructor
    MaskDropDown(QWidget *parent = 0);

    //! Destructor
    virtual ~MaskDropDown();

public slots:
    //! Slot that is signalled when image data have been loaded
    void imageLoaded();
    //! Slot that is signalled when a mask image has been loaded
    void maskLoaded();

protected slots:
    //! Triggers the dialog box to select a masking image
    void importTrigger();
    //! Shows the grid
    void gridTrigger(bool checkState);
    //! Clears the mask data
    void clearTrigger();

signals:
    //! Emitted when user input is to choose a mask
    void importMaskClicked();
    //! Emitted to draw/clear grid from the display
    void gridToggled(bool isTrue);
    //! Emitted if the user wishes to cancel a previous mask
    void clearMask(bool isTrue);

private:
    bool loaded;
    PivDisplay *pivDisplay;
    QList<QAction*> actionList;

};

#endif // MASKDROPDOWN_H
