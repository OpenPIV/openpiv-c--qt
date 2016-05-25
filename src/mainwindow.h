/*
====================================================================================

File: mainwindow.h
Description: This class inherits QMainWindow and contains functions to interact with
    the main window.  It is from here where the user executes a majority of this
    program's functions.
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include "pivdisplay.h"
#include "imagelist.h"
#include "vectorlist.h"

#include "importimages.h"
#include "batchwindow.h"

#include "maskdropdown.h"

#include "session.h"

#include "ui_mainwindow.h"

/*! This class inherits QMainWindow and contains functions to interact with
    the main window.  It is from here where the user executes a majority of this
    program's functions.
*/

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    //! Constructor
    MainWindow(QWidget *parent = 0);
    //! Destructor
    ~MainWindow();

protected:
    //! Setup the various windows which can be brought up using the main window
    void setupWindows();
    //! Initialization of the global parameters
    void initialize();
    //! Establish the myriad of signal and slot connections
    void setupConnections();

    //! Initialization of the process tab in the ribbon bar
    void initializeProcessTab();
    //! Initialization of the filter tab in the ribbon bar
    void initializeFilterTab();
    //! Initialization of the data quality tab in the ribbon bar
    void initializeDataQualityTab();
    //! Initialization of the view tab in the ribbon bar
    void initializeViewTab();

protected slots:
    //! Slot to launch the image import dialog
    void launchImageImport();

    //! Handles a change in output folder
    void notifyFolderChange();
    //! Handling of a request to show an 'A' image
    void pivAclicked(int rowSelected);
    //! Handling of a request to show a 'B' image
    void pivBclicked(int rowSelected);
    //! Handling of a request to show some PIV data
    void vectorClicked(int rowSelected);
    //! Advances the current index forward one
    void forwardOne();
    //! Moves the current index back one
    void backwardOne();
    //! Opens the filtered data colour dialog
    void chooseFilteredColour();
    //! Opens the unfiltered data colour dialog
    void chooseUnfilteredColour();
    //! Handles redrawing the colour buttons
    void setColourButtons();

    //! Sets up the filter tab based on global Settings object
    void setFilterTab();
    //! Handles a change to the filter settings
    void filterChanged();
    //! Sets the filter tab values
    void setFilterValues();

    //! Sets up the display tab based on global Settings object
    void setDisplayTab();

    //! Sets up the 'Doing PIV' tab based on global Settings object
    void setDoingPIV();
    //! Sets the output parameters in the global Settings object
    void setOutput();
    //! Launches the folder selection dialog
    void chooseOutputFolder();

    //! Handles the display options when a batch operation is finished
    void batchDone();

    //! Sets up the data quality tab based on global Settings object
    void setDataQualityTab();
    //! Handles a change to the data quality settings
    void qualityChanged();

    //! Handles the (x,y) position label for the mouse within the scene
    void updatePositionLabel(QPointF xyPoint);

private:
    bool isA;

    ImportImages *importImages;
    MaskDropDown *maskDropDown;

    // Refactoring to be implemented by a session class
    Session *session;
    Settings *settings;
    Processing *process;
    Analysis *analysis;
    DataContainer *filedata;
    Output *output;
};

#endif // MAINWINDOW_H
