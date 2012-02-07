/*
====================================================================================

File: importimages.h
Description: This class inherits QDialog and the layout is designed in Qt Designer.
    It is the dialog interface used to read images from the selected directory.  Native
    file browsers perform quite slowly with a significant quantity of images in a
    directory.  With TR-PIV systems this can be upwards of 10,000 image pairs.
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


#ifndef IMPORTIMAGES_H
#define IMPORTIMAGES_H

#include <QtGui/QDialog>
#include <QMenu>
#include "importimagelist.h"
#include "datacontainer.h"
#include "ui_importimages.h"

class ImportImages : public QDialog, private Ui::ImportImages 
{
    
    Q_OBJECT

public:
    ImportImages(QWidget *parent = 0);
    virtual ~ImportImages();
    void setWindow(QString directoryPass);
    QStringList listA();
    QStringList listB();
    void setData(DataContainer *filedataPass);
                       
public slots:
    void removeFiles(QList<int>);
    
protected:
    void setupConnections();
    void filter();
    
    void autoConfig(QStringList importList);
    bool onlyNum(QString str);
                             
protected slots:
    void importPressed();
    void cancelPressed();
    
signals:
    void imagesImported();
    //void frameAupdated(QStringList frameA);
    //void frameBupdated(QStringList frameB);
    //void frameAupdated(QStringList* frameA);
    //void frameBupdated(QStringList* frameB);
    
private:
    QStringList fileList;
    QStringList frameA;
    QStringList frameB;
    DataContainer *filedata;
    QString dirString;
};

#endif // IMPORTIMAGES_H
