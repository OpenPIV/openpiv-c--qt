#include <iostream>

#include "batchwindow.h"
#include "ui_batchwindow.h"

BatchWindow::BatchWindow(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    setupConnections();
    progress = 0;
    progressBar->setValue(progress);
}

BatchWindow::~BatchWindow()
{

}

void BatchWindow::setupConnections()
{
    connect(maskCheck, SIGNAL(stateChanged(int)), this, SLOT(commitSettings()));
    connect(filterCheck, SIGNAL(stateChanged(int)), this, SLOT(commitSettings()));
    connect(imagesCheck, SIGNAL(stateChanged(int)), this, SLOT(commitSettings()));
    connect(vectorsCheck, SIGNAL(stateChanged(int)), this, SLOT(commitSettings()));
    connect(threadingCheck, SIGNAL(stateChanged(int)), this, SLOT(commitSettings()));

    connect(startButton, SIGNAL(clicked()), this, SLOT(startClicked()));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(stopClicked()));
}

void BatchWindow::setSettings(Settings *settingsPass)
{
    _settings = settingsPass;
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

//void BatchWindow::setProcess(Processing *processPass)
//{
//    _process = processPass;
//    connect(startButton, SIGNAL(clicked()), _process, SLOT(processBatch()));
//}

void BatchWindow::setProgressRange(int max)
{
    progressBar->setMinimum(0);
    progressBar->setMaximum(max);
}

void BatchWindow::addToProgress()
{
    progress++;
    progressBar->setValue(progress);
}

void BatchWindow::commitSettings()
{
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
    refreshEnabled();
}

void BatchWindow::refreshEnabled()
{
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
    refreshEnabled();
}
