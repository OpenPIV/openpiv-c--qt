/*
====================================================================================

File: mainwindow.cpp
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

#include <QtGui>
#include <cmath>
#include <iostream>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "settings.h"
#include "filteroptions.h"

#include "importimages.h"

#include "imagedata.h"
#include "pivdata.h"

#include "imagelist.h"
#include "vectorlist.h"
#include "imagepaint.h"

// Need to come back and comment this class when the Session class is up and running properly

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    initialize();
    setupWindows();
    setupConnections();
    initializeProcessTab();
    initializeFilterTab();
    setFilterTab();
    setDisplayTab();
    setColourButtons();
    setDoingPIV();
    initializeDataQualityTab();
    setDataQualityTab();
    initializeViewTab();
}

MainWindow::~MainWindow()
{
    delete importImages;
    delete filedata;
    delete maskDropDown;
    delete process;
    delete analysis;
}

/*--------Initializing----------------------------*/

void MainWindow::initialize()
{
    // This needs to be refactored so that the Session class
    // handles some of the initializing of these object
    session = new Session;

    settings = new Settings;
    filedata = new DataContainer(settings);
    process = new Processing(settings,filedata);
    analysis = new Analysis(settings,filedata);
    output = new Output(settings,filedata);

    // This should all stay
    aListWidget->setA(true);
    bListWidget->setA(false);
    aListWidget->setData(filedata);
    bListWidget->setData(filedata);
    vectorListWidget->setData(filedata);
    pivDisplay->setSettings(settings);
    pivDisplay->setData(filedata);
    isA = true;
}

void MainWindow::setupWindows()
{
    importImages = new ImportImages;
    importImages->setData(filedata);
    maskDropDown = new MaskDropDown;
    maskButton->setMenu(maskDropDown);
}

void MainWindow::setupConnections()
{
    // Session
    connect(saveSession, SIGNAL(clicked()), session, SLOT(save()));
    connect(openSession, SIGNAL(clicked()), session, SLOT(open()));

    // Importing
    connect(importButton, SIGNAL(clicked()), this, SLOT(launchImageImport()));
    connect(filedata, SIGNAL(imagesImported()), aListWidget, SLOT(populate()));
    connect(filedata, SIGNAL(imagesImported()), bListWidget, SLOT(populate()));
    connect(filedata, SIGNAL(imagesImported()), vectorListWidget, SLOT(populate()));
    connect(filedata, SIGNAL(imagesImported()), this, SLOT(notifyFolderChange()));
    connect(filedata, SIGNAL(vectorListUpdated()), vectorListWidget, SLOT(update()));

    // Image selection
    connect(aListWidget, SIGNAL(fileClicked(int)), this, SLOT(pivAclicked(int)));
    connect(bListWidget, SIGNAL(fileClicked(int)), this, SLOT(pivBclicked(int)));
    connect(vectorListWidget, SIGNAL(fileClicked(int)), this, SLOT(vectorClicked(int)));
    connect(forwardButton, SIGNAL(clicked()), this, SLOT(forwardOne()));
    connect(backButton, SIGNAL(clicked()), this, SLOT(backwardOne()));

    // Image/vector viewing
    connect(zoomInButton, SIGNAL(clicked()), pivDisplay, SLOT(zoomIn()));
    connect(zoomOutButton, SIGNAL(clicked()), pivDisplay, SLOT(zoomOut()));
    connect(zoomFitButton, SIGNAL(clicked()), pivDisplay, SLOT(zoomFit()));
    // The following should be moved into settings
    connect(vectorToggle, SIGNAL(toggled(bool)), pivDisplay, SLOT(vectorsToggled(bool)));
    //
    connect(colourButtonFiltered, SIGNAL(clicked()), this, SLOT(chooseFilteredColour()));
    connect(colourButtonUnfiltered, SIGNAL(clicked()), this, SLOT(chooseUnfilteredColour()));
    connect(scaleSpin, SIGNAL(valueChanged(double)), settings, SLOT(setVectorScale(double)));
    connect(subSpin, SIGNAL(valueChanged(double)), settings, SLOT(setVectorSub(double)));
    connect(settings, SIGNAL(vectorSettingChanged()), pivDisplay, SLOT(vectorsChanged()));

    // Masking
    connect(maskButton, SIGNAL(clicked()), maskButton, SLOT(showMenu()));
    connect(filedata,SIGNAL(imagesImported()), maskDropDown, SLOT(imageLoaded()));
    connect(maskDropDown, SIGNAL(importMaskClicked()), filedata, SLOT(importMask()));
    connect(filedata, SIGNAL(maskLoaded()), maskDropDown, SLOT(maskLoaded()));
    connect(maskDropDown, SIGNAL(gridToggled(bool)), pivDisplay, SLOT(maskToggled(bool)));
    connect(maskDropDown, SIGNAL(clearMask(bool)), settings, SLOT(setIsMask(bool)));

    // Process tab
    connect(hSizeCombo, SIGNAL(activated(int)), settings, SLOT(setIntLengthX(int)));
    connect(vSizeCombo, SIGNAL(activated(int)), settings, SLOT(setIntLengthY(int)));
    connect(hSpaceSpin, SIGNAL(valueChanged(int)), settings, SLOT(setDeltaX(int)));
    connect(vSpaceSpin, SIGNAL(valueChanged(int)), settings, SLOT(setDeltaY(int)));

    // Filter tab
    connect(globalRangeCheck, SIGNAL(toggled(bool)), this, SLOT(filterChanged()));
    connect(minUedit, SIGNAL(textChanged(QString)), this, SLOT(setFilterValues()));
    connect(maxUedit, SIGNAL(textChanged(QString)), this, SLOT(setFilterValues()));
    connect(minVedit, SIGNAL(textChanged(QString)), this, SLOT(setFilterValues()));
    connect(maxVedit, SIGNAL(textChanged(QString)), this, SLOT(setFilterValues()));
    connect(globalStDevCheck, SIGNAL(toggled(bool)), this, SLOT(filterChanged()));
    connect(nStdDevSpin, SIGNAL(valueChanged(double)), this, SLOT(setFilterValues()));

    connect(localCheck, SIGNAL(toggled(bool)), this, SLOT(filterChanged()));
    connect(localMethodCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setFilterValues()));
    connect(localNxNCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setFilterValues()));
    connect(localUedit, SIGNAL(textChanged(QString)), this, SLOT(setFilterValues()));
    connect(localVedit, SIGNAL(textChanged(QString)), this, SLOT(setFilterValues()));

    connect(interpolateCheck, SIGNAL(toggled(bool)), this, SLOT(filterChanged()));
    connect(interpolateMethodCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setFilterValues()));
    connect(interpolateNxNCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setFilterValues()));

    connect(smoothCheck, SIGNAL(toggled(bool)), this, SLOT(filterChanged()));
    connect(smoothRadiusEdit, SIGNAL(textChanged(QString)), this, SLOT(setFilterValues()));
    connect(smoothNxNCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setFilterValues()));

    // DoingPIV
    connect(outputFolderEdit, SIGNAL(textEdited(QString)), this, SLOT(setOutput()));
    connect(outputFolderButton, SIGNAL(clicked()), this, SLOT(chooseOutputFolder()));
    connect(outputFormatCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setOutput()));
    connect(correlateButton, SIGNAL(clicked()), process, SLOT(processCurrentImagePair()));
    connect(process, SIGNAL(currentProcessed()), pivDisplay, SLOT(displayCurrent()));
    connect(process, SIGNAL(currentProcessed()), output, SLOT(outputCurrent()));
    connect(filterButton, SIGNAL(clicked()), analysis, SLOT(filterCurrent()));
    connect(analysis, SIGNAL(currentFiltered()), pivDisplay, SLOT(displayCurrent()));
    connect(analysis, SIGNAL(currentFiltered()), output, SLOT(outputCurrent()));
    connect(batchButton, SIGNAL(clicked()), process, SLOT(launchBatchWindow()));
    connect(process, SIGNAL(batchProcessed()), this, SLOT(batchDone()));
    connect(pivDisplay, SIGNAL(mouseMoved(QPointF)), this, SLOT(updatePositionLabel(QPointF)));

    // Data Quality
    connect(filterSNRCheck, SIGNAL(toggled(bool)), this, SLOT(qualityChanged()));
    connect(snrEdit, SIGNAL(textEdited(QString)), this, SLOT(qualityChanged()));
    connect(filterIntCheck, SIGNAL(toggled(bool)), this, SLOT(qualityChanged()));
    connect(intensityEdit, SIGNAL(textEdited(QString)), this, SLOT(qualityChanged()));
}

/*-------- Setting the tabs -----------------*/

void MainWindow::setDoingPIV()
{
    outputFolderEdit->setText(settings->outputFolder());
    outputFormatCombo->insertItem(OpenPIV::Text,"Text file");
    outputFormatCombo->insertItem(OpenPIV::HDF5,"HDF5");
    outputFormatCombo->setCurrentIndex(0);
}

void MainWindow::initializeProcessTab()
{
    int i, index;
    double power2;

    for (i = 0; i < 4; i++)
    {
        power2 = pow(2.0,double(4+i));
        hSizeCombo->insertItem(i,QString("%1").arg(power2));
        vSizeCombo->insertItem(i,QString("%1").arg(power2));
    }

    index = int(log10(double(settings->intLengthX())) / log10(2.0)) - 4;
    hSizeCombo->setCurrentIndex(index);
    index = int(log10(double(settings->intLengthY())) / log10(2.0)) - 4;
    vSizeCombo->setCurrentIndex(index);

    hSpaceSpin->setMinimum(0);
    vSpaceSpin->setMinimum(0);
    hSpaceSpin->setMaximum(100);
    vSpaceSpin->setMaximum(100);
    double value;
    value = double(settings->deltaX()) / double(settings->intLengthX()) * 100;
    hSpaceSpin->setValue(int(value));
    value = double(settings->deltaY()) / double(settings->intLengthY()) * 100;
    vSpaceSpin->setValue(int(value));

    crossCorrCombo->insertItem(OpenPIV::FFTCorrelator,"Fast Fourier Transform");
    interpolateCombo->insertItem(OpenPIV::GaussianSubPixel,"Three-point Gaussian");
}

void MainWindow::initializeFilterTab()
{
    // Global
    minUedit->setValidator(new QDoubleValidator(-256.0, 256.0, 3, minUedit));
    maxUedit->setValidator(new QDoubleValidator(-256.0, 256.0, 3, maxUedit));
    minVedit->setValidator(new QDoubleValidator(-256.0, 256.0, 3, minVedit));
    maxVedit->setValidator(new QDoubleValidator(-256.0, 256.0, 3, maxVedit));

    // Local
    QString label;
    int i, index;
    localMethodCombo->insertItem(OpenPIV::LocalMean,"Mean");
    localMethodCombo->insertItem(OpenPIV::LocalMedian,"Median");

    for (i = 0; i < 5; i++)
    {
        index = (i+1)*2 + 1;
        label = QString("%1x%2").arg(index).arg(index);
        localNxNCombo->insertItem(i,label);
    }
    localUedit->setValidator(new QDoubleValidator(0.0, 256.0, 3, localUedit));
    localVedit->setValidator(new QDoubleValidator(0.0, 256.0, 3, localVedit));

    // Interpolate
    interpolateMethodCombo->insertItem(OpenPIV::InterpolateMean,"Mean");
    for (i = 0; i < 5; i++)
    {
        index = (i+1)*2 + 1;
        label = QString("%1x%2").arg(index).arg(index);
        interpolateNxNCombo->insertItem(i,label);
    }

    // Smoothing
    for (i = 0; i < 5; i++)
    {
        index = (i+1)*2 + 1;
        label = QString("%1x%2").arg(index).arg(index);
        smoothNxNCombo->insertItem(i,label);
    }
    smoothRadiusEdit->setValidator(new QDoubleValidator(0.0, 1000.0, 3, smoothRadiusEdit));
}

void MainWindow::setFilterTab()
{
    FilterOptions filterOptions = settings->filterOptions();

    // Global
    minUedit->setText(QString("%1").arg(filterOptions.umin()));
    maxUedit->setText(QString("%1").arg(filterOptions.umax()));
    minVedit->setText(QString("%1").arg(filterOptions.vmin()));
    maxVedit->setText(QString("%1").arg(filterOptions.vmax()));

    minUedit->setEnabled(filterOptions.globalRange());
    maxUedit->setEnabled(filterOptions.globalRange());
    minVedit->setEnabled(filterOptions.globalRange());
    maxVedit->setEnabled(filterOptions.globalRange());

    nStdDevSpin->setValue(filterOptions.nStd());
    nStdDevSpin->setEnabled(filterOptions.globalStd());

    // Local
    localMethodCombo->setEnabled(filterOptions.local());
    localMethodCombo->setCurrentIndex(filterOptions.localMethod());
    localNxNCombo->setEnabled(filterOptions.local());
    localNxNCombo->setCurrentIndex((filterOptions.localNxN()-1)/2-1);
    localUedit->setEnabled(filterOptions.local());
    localVedit->setEnabled(filterOptions.local());
    localUedit->setText(QString("%1").arg(filterOptions.uTol()));
    localVedit->setText(QString("%1").arg(filterOptions.vTol()));

    // Interpolate
    interpolateMethodCombo->setEnabled(filterOptions.interpolate());
    interpolateMethodCombo->setCurrentIndex(filterOptions.intMethod());
    interpolateNxNCombo->setEnabled(filterOptions.interpolate());
    interpolateNxNCombo->setCurrentIndex((filterOptions.intNxN()-1)/2-1);

    // Smoothing
    smoothNxNCombo->setEnabled(filterOptions.smoothing());
    smoothNxNCombo->setCurrentIndex((filterOptions.smoothNxN()-1)/2-1);
    smoothRadiusEdit->setEnabled(filterOptions.smoothing());
    smoothRadiusEdit->setText(QString("%1").arg(filterOptions.smoothRadius()));
}

void MainWindow::setDisplayTab()
{
    vectorToggle->setChecked(true);
}

void MainWindow::initializeDataQualityTab()
{
    snrEdit->setValidator(new QDoubleValidator(0.0, 1000.0, 3, snrEdit));
    intensityEdit->setValidator(new QDoubleValidator(0.0, 5000.0, 3, intensityEdit));
}

void MainWindow::setDataQualityTab()
{
    filterSNRCheck->setChecked(settings->filterOptions().snr());
    snrEdit->setEnabled(settings->filterOptions().snr());
    snrEdit->setText(QString("%1").arg(settings->filterOptions().snrThresh()));

    filterIntCheck->setChecked(settings->filterOptions().imageIntensity());
    intensityEdit->setEnabled(settings->filterOptions().imageIntensity());
    intensityEdit->setText(QString("%1").arg(settings->filterOptions().imageThresh()));
}

void MainWindow::initializeViewTab()
{
    scaleSpin->setValue(settings->vectorScale());
    subSpin->setValue(settings->vectorSub());
}


/*----------Launching windows---------------------------------*/

void MainWindow::launchImageImport()
{
    QString directory = QFileDialog::getExistingDirectory(this);
    if (!directory.isEmpty())
    {
        outputFolderEdit->setText(directory);
        settings->setOutputFolder(directory);
        importImages->setWindow(directory);
        importImages->show();        
    }
}

void MainWindow::notifyFolderChange()
{
    QString directory = settings->outputFolder();
    QString boxText = QString(tr("The output folder has been set to:\n %1")).arg(directory);
    boxText.append(tr("\n This folder can be changed on the \"Doing PIV\" tab.\n"));
    QMessageBox::information(this,QString(tr("Output Folder Set")),boxText);
    pivAclicked(0);
}

/*----------Display----------------------------------------------*/

void MainWindow::pivAclicked(int rowSelected)
{
    aListWidget->clearHighlight();
    bListWidget->clearHighlight();
    vectorListWidget->clearHighlight();
    aListWidget->highlightRow(rowSelected);
    vectorListWidget->highlightRow(rowSelected);
    isA = true;
    filedata->setCurrentIndex(rowSelected,true);
    pivDisplay->displayCurrent();
}

void MainWindow::pivBclicked(int rowSelected)
{
    aListWidget->clearHighlight();
    bListWidget->clearHighlight();
    vectorListWidget->clearHighlight();
    bListWidget->highlightRow(rowSelected);
    vectorListWidget->highlightRow(rowSelected);
    isA = false;
    filedata->setCurrentIndex(rowSelected,false);
    pivDisplay->displayCurrent();
}

void MainWindow::vectorClicked(int rowSelected)
{
    aListWidget->clearHighlight();
    bListWidget->clearHighlight();
    vectorListWidget->clearHighlight();
    vectorListWidget->highlightRow(rowSelected);
    isA = true;
    filedata->setCurrentIndex(rowSelected,false);
    pivDisplay->displayCurrent();
}

void MainWindow::batchDone()
{
    vectorClicked(0);
}

void MainWindow::forwardOne()
{
    int nextIndex = filedata->currentIndex()+1;
    if (nextIndex < filedata->size())
    {
        if (isA) pivAclicked(nextIndex);
        else pivBclicked(nextIndex);
    }
}

void MainWindow::backwardOne()
{
    int backIndex = filedata->currentIndex() - 1;
    if (backIndex >= 0)
    {
        if (isA) pivAclicked(backIndex);
        else pivBclicked(backIndex);
    }
}

void MainWindow::chooseFilteredColour()
{
    QColor colour;
    colour = QColorDialog::getColor(settings->vectorColourFiltered(),this);
    settings->setVectorColourFiltered(colour);
    setColourButtons();
}

void MainWindow::chooseUnfilteredColour()
{
    QColor colour;
    colour = QColorDialog::getColor(settings->vectorColourUnfiltered(),this);
    settings->setVectorColourUnfiltered(colour);
    setColourButtons();
}

void MainWindow::setColourButtons()
{
    QColor colour;
    QString styleItem;
    colour = settings->vectorColourUnfiltered();
    styleItem = QString("background-color: rgb(%1,%2,%3);").arg(colour.red()).arg(colour.green()).arg(colour.blue());
    colourButtonUnfiltered->setStyleSheet(styleItem);
    colour = settings->vectorColourFiltered();
    styleItem = QString("background-color: rgb(%1,%2,%3);").arg(colour.red()).arg(colour.green()).arg(colour.blue());
    colourButtonFiltered->setStyleSheet(styleItem);
}

void MainWindow::updatePositionLabel(QPointF xyPoint)
{
    int x = floor(xyPoint.x());
    int y = settings->imageSize().height() - floor(xyPoint.y());
    positionLabel->setText(QString("%1, %2").arg(x).arg(y));
}

/* ----------- Output ----------------*/

void MainWindow::setOutput()
{
    settings->setOutputFolder(outputFolderEdit->text());
    settings->setOutputFormat(outputFormatCombo->currentIndex());
}

void MainWindow::chooseOutputFolder()
{
    QString directory = QFileDialog::getExistingDirectory(this);
    outputFolderEdit->setText(directory);
    setOutput();
}

/* ----------- Filters ---------------*/

void MainWindow::filterChanged()
{
    FilterOptions filterOptions;
    filterOptions = settings->filterOptions();

    // Global
    filterOptions.enableGlobalRange(globalRangeCheck->isChecked());
    filterOptions.enableGlobalStd(globalStDevCheck->isChecked());

    // Local
    filterOptions.enableLocal(localCheck->isChecked());

    // Interpolate
    filterOptions.enableInterpolate(interpolateCheck->isChecked());

    // Smoothing
    filterOptions.enableSmoothing(smoothCheck->isChecked());

    settings->setFilterOptions(filterOptions);
    setFilterTab();
}

void MainWindow::setFilterValues()
{
    FilterOptions filterOptions;
    filterOptions = settings->filterOptions();

    // Global
    double temp;

    if (filterOptions.globalRange())
    {
        double umin = minUedit->text().toDouble();
        double umax = maxUedit->text().toDouble();
        if (umin > umax)
        {
            temp = umin;
            umin = umax;
            umax = temp;
        }
        double vmin = minVedit->text().toDouble();
        double vmax = maxVedit->text().toDouble();
        if (vmin > vmax)
        {
            temp = vmin;
            vmin = vmax;
            vmax = temp;
        }
        filterOptions.setRange(umin,umax,vmin,vmax);
    }

    if (filterOptions.globalStd()) filterOptions.setNStd(nStdDevSpin->value());

    // Local
    double u, v;
    if (filterOptions.local())
    {
        filterOptions.setLocalMethod(localMethodCombo->currentIndex());
        filterOptions.setLocalNxN((localNxNCombo->currentIndex()+1)*2+1);
        u = localUedit->text().toDouble();
        v = localVedit->text().toDouble();
        filterOptions.setLocalTolerance(u, v);
    }

    // Interpolate
    if (filterOptions.interpolate())
    {
        filterOptions.setIntMethod(interpolateMethodCombo->currentIndex());
        filterOptions.setLocalNxN((interpolateNxNCombo->currentIndex()+1)*2+1);
    }

    // Smoothing
    if (filterOptions.smoothing())
    {
        filterOptions.setSmoothRadius(smoothRadiusEdit->text().toDouble());
        filterOptions.setSmoothNxN((smoothNxNCombo->currentIndex()+1)*2+1);
    }

    // Write and delay changes
    settings->setFilterOptions(filterOptions);
    QTimer::singleShot(10000, this, SLOT(setFilterTab()));
}

void MainWindow::qualityChanged()
{
    FilterOptions filterOptions = settings->filterOptions();
    filterOptions.enableSNR(filterSNRCheck->isChecked());
    snrEdit->setEnabled(filterSNRCheck->isChecked());
    filterOptions.setSnrThresh(snrEdit->text().toDouble());

    filterOptions.enableImageIntensity(filterIntCheck->isChecked());
    intensityEdit->setEnabled(filterIntCheck->isChecked());
    filterOptions.setImageThresh(intensityEdit->text().toDouble());

    settings->setFilterOptions(filterOptions);
    QTimer::singleShot(10000, this, SLOT(setDataQualityTab()));
}

