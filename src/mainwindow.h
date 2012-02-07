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

#include <QMainWindow>

#include "pivdisplay.h"
#include "imagelist.h"
#include "vectorlist.h"

#include "importimages.h"
#include "batchwindow.h"

#include "maskdropdown.h"

#include "processing.h"
#include "analysis.h"
#include "datacontainer.h"
#include "output.h"

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = 0);
        ~MainWindow();

    protected:
        // Initialization
        void setupWindows();
        void initialize();
        void setupConnections();

        void initializeProcessTab();
        void initializeFilterTab();
        void initializeDataQualityTab();
        void initializeViewTab();


    protected slots:
        void launchImageImport();

        void notifyFolderChange();
        void pivAclicked(int rowSelected);
        void pivBclicked(int rowSelected);
        void vectorClicked(int rowSelected);
        void forwardOne();
        void backwardOne();
        void chooseFilteredColour();
        void chooseUnfilteredColour();
        void setColourButtons();

        void setFilterTab();
        void filterChanged();
        void setFilterValues();

        void setDisplayTab();

        void setDoingPIV();
        void setOutput();
        void chooseOutputFolder();

        void batchDone();

        void setDataQualityTab();
        void qualityChanged();

        void updatePositionLabel(QPointF xyPoint);

    private:
        bool isA;
        Settings *settings;

        ImportImages *importImages;
        MaskDropDown *maskDropDown;

        Processing *process;
        Analysis *analysis;
        DataContainer *filedata;
        Output *output;
};

#endif // MAINWINDOW_H
