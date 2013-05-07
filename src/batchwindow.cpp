/*
====================================================================================

File: batchwindow.cpp
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

#include <iostream>

#include "batchwindow.h"
#include "ui_batchwindow.h"

BatchWindow::BatchWindow(QWidget *parent) : QDialog(parent)
{
    // Qt command to setup the UI
    setupUi(this);
    // Setting up the signal and slot connections
    setupConnections();
    // Initialization of the progress to zero
    progress = 0;
    progressBar->setValue(progress);
}

BatchWindow::~BatchWindow()
{

}

void BatchWindow::setupConnections()
{
    // The various check boxes are linked to the commitSettings slot
    connect(maskCheck, SIGNAL(stateChanged(int)), this, SLOT(commitSettings()));
    connect(filterCheck, SIGNAL(stateChanged(int)), this, SLOT(commitSettings()));
    connect(imagesCheck, SIGNAL(stateChanged(int)), this, SLOT(commitSettings()));
    connect(vectorsCheck, SIGNAL(stateChanged(int)), this, SLOT(commitSettings()));
    connect(threadingCheck, SIGNAL(stateChanged(int)), this, SLOT(commitSettings()));

    // Connection of the start and stop buttons
    connect(startButton, SIGNAL(clicked()), this, SLOT(startClicked()));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(stopClicked()));
}

void BatchWindow::setSettings(Settings *settingsPass)
{
    // Receives a pointer to the program-wide Settings object
    _settings = settingsPass;
    // Refreshes the UI
    refreshEnabled();
}

void BatchWindow::startClicked()
{
    emit(startProcess());
}

void BatchWindow::stopClicked()
{
    emit(stopProcess());
}

void BatchWindow::setProgressRange(int max)
{
    // Minimum of the progress bar is always zero
    progressBar->setMinimum(0);
    // Sets the maximum of the progress bar to the int passed
    // (ideally the total number of image pairs)
    progressBar->setMaximum(max);
}

void BatchWindow::addToProgress()
{
    // Increment the private int progress
    progress++;
    // Set a new value of the progress
    progressBar->setValue(progress);
}

void BatchWindow::commitSettings()
{
    // Writing the varoius setting boxes to the program-wide Settings object
    _settings->setBatchMask(maskCheck->isChecked());
    if (!maskCheck->isChecked())
    {
        _settings->setIsMask(false);
    }
    _settings->setBatchFilter(filterCheck->isChecked());
    if (threadingCheck->isChecked())
    {
        imagesCheck->setEnabled(false);
        vectorsCheck->setEnabled(false);
    }
    else
    {
        imagesCheck->setEnabled(true);
        vectorsCheck->setEnabled(true);
    }
    _settings->setBatchThreading(threadingCheck->isChecked());
    _settings->setBatchShowImage(imagesCheck->isChecked());
    _settings->setBatchShowVectors(vectorsCheck->isChecked());
    // Refreshing the UI
    refreshEnabled();
}

void BatchWindow::refreshEnabled()
{
    // Handling refreshing of the UI based on Settings object
    maskLabel->setEnabled(_settings->batchMask());
    maskIcon->setEnabled(_settings->batchMask());
    maskCheck->setChecked(_settings->batchMask());

    filterLabel->setEnabled(_settings->batchFilter());
    filterIcon->setEnabled(_settings->batchFilter());
    filterCheck->setChecked(_settings->batchFilter());

    threadingCheck->setChecked(_settings->batchThreading());
    vectorsCheck->setChecked(_settings->batchShowVectors());
    imagesCheck->setChecked(_settings->batchShowImage());
}

void BatchWindow::refresh()
{
    // Refreshing the UI
    refreshEnabled();
}
