/*
====================================================================================

File: batchwindow.h
Description: This class handles UI operations for setting up a batch PIV process.
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

#ifndef BATCHWINDOW_H
#define BATCHWINDOW_H

#include <QDialog>

#include "settings.h"
#include "ui_batchwindow.h"

//!  Class handling the UI for setting up a batch PIV process.
/*!
  The majority of PIV computations are performed in batch and this
  class handles its implementation.
*/
class BatchWindow : public QDialog, private Ui::BatchWindow
{
    Q_OBJECT

public:
    //! Constructor.
    explicit BatchWindow(QWidget *parent = 0);

    //! Destructor
    virtual ~BatchWindow();

    //! Function to pass settings object to this class.
    /*!
        \param pass a Settings object
    */
    void setSettings(Settings *settingsPass);
    //! A window refresh function called from the MainWindow class
    void refresh();

signals:
    //! Signal notifiying the Processing class to start a batch process.
    void startProcess();
    //! Signal notifiying the Processing class to stop a batch process.
    void stopProcess();

public slots:
    //! Slot that updates the progress range upon expansion of the image list
    /*!
        \param max is an int corresponding to the total number of image pairs
    */
    void setProgressRange(int max);

    //! A slot that incrementally (by 1) updates the progress of the batch process.
    void addToProgress();

protected:
    //! Sets up the signal and slot connections upon initialization of the object
    void setupConnections();

protected slots:
    //! Slot which is called when public function refresh() is called
    void refreshEnabled();
    //! Slot which writes the settings set in the UI to the Settings object
    void commitSettings();

    //! Protected handling of operations when start button is clicked.
    void startClicked();
    //! Protected handling of operations when stop button is clicked.
    void stopClicked();

private:
    //! Private pointer to the program-wide Settings object
    Settings *_settings;
    //! Private int holding the progress of the batch process.
    int progress;
};

#endif // BATCHWINDOW_H
