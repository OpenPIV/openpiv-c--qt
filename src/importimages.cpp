/*
====================================================================================

File: importimages.cpp
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

#include "importimages.h"
#include "datacontainer.h"
#include "ui_importimages.h"

#include <QDialog>
#include <QDir>

ImportImages::ImportImages(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    setupConnections();
}

ImportImages::~ImportImages()
{}

void ImportImages::setWindow(QString directoryPass)
{
    dirString = directoryPass;
    if (!dirString.isEmpty())
    {
        dirEdit->setText(dirString);
        QDir qDir = QDir(dirString);
        fileList = qDir.entryList();
        filter();
        imageListWidget->addItems(fileList);
    }
}

void ImportImages::setData(DataContainer *filedataPass)
{
    filedata = filedataPass;
}

QStringList ImportImages::listA()
{
    return frameA;
}

QStringList ImportImages::listB()
{
    return frameB;
}

void ImportImages::setupConnections()
{
    connect(importButton, SIGNAL(clicked()), this, SLOT(importPressed()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelPressed()));
    connect(imageListWidget, SIGNAL(removeSelected(QList<int>)), this, SLOT(removeFiles(QList<int>)));
}

void ImportImages::removeFiles (QList<int> selected)
{
    QStringList tempList;
    int intIterator = 0;
    for (int i = 0; i < fileList.count(); i++)
    {
        if (!(selected.at(intIterator) == i)) tempList.append( fileList.value(i) );
        else intIterator++;
    }
    fileList = tempList;
    imageListWidget->clear();
    imageListWidget->addItems( fileList );
}

void ImportImages::importPressed()
{
    QList<QListWidgetItem*> selectionList = imageListWidget->selectedItems();
    if (selectionList.size() <= 1) autoConfig(fileList);
    else
    {
        QStringList selectedList;
        for (int i = 0; i < selectionList.size(); i++)
        {
            selectedList.append(selectionList.value(i)->text());
        }
        autoConfig(selectedList);
    }
    if (!frameA.isEmpty() && !frameB.isEmpty()) filedata->append(frameA,frameB);
    frameA.clear();
    frameB.clear();
    imageListWidget->clear();
    this->hide();
}

void ImportImages::cancelPressed()
{
    this->hide();
}

void ImportImages::filter()
{
    int iterations = fileList.length();
    QStringList tempList( fileList );
    QString str;
    fileList.clear();

    // Sorting through the image list to find those with compatible image formats
    for (int i = 0; i < iterations; i++)
    {
        if (tempList.value(i).endsWith(".tif",Qt::CaseInsensitive) ||
            tempList.value(i).endsWith(".png",Qt::CaseInsensitive) ||
            tempList.value(i).endsWith(".bmp",Qt::CaseInsensitive) ||
            tempList.value(i).endsWith(".jpg",Qt::CaseInsensitive) ||
            tempList.value(i).endsWith(".jpeg",Qt::CaseInsensitive) ||
            tempList.value(i).endsWith(".gif",Qt::CaseInsensitive) ||
            tempList.value(i).endsWith(".pbm",Qt::CaseInsensitive) ||
            tempList.value(i).endsWith(".pgm",Qt::CaseInsensitive) ||
            tempList.value(i).endsWith(".ppm",Qt::CaseInsensitive) ||
            tempList.value(i).endsWith(".xbm",Qt::CaseInsensitive) ||
            tempList.value(i).endsWith(".xpm",Qt::CaseInsensitive) )
        {
            str = dirString;
            str.append("/");
            fileList.append(str.append(tempList.value(i)));
        }
    }
    fileList.sort();
}


/* This is an algorithm that attempts to sort image list into 'A' images and
   'B' images. */
void ImportImages::autoConfig (QStringList importList)
{
    importList.sort();

    int count, i, j, k, rows, nameLength, sizeCount, minLength, minNumber;
    QString pass, str, str1, str2, common;
    QStringList tempList, tempA, tempB;
    QList<int> numList;
    bool stop = false, more = true;
    bool *taken;

    rows = importList.size();
    pass = importList.at(0);
    taken = new bool [rows + 1];
    nameLength = pass.length();
    sizeCount = 0;
    for (i = 0; i < rows; i++)
    {
        pass = importList.at(i);
        taken[i] = false;
        if (pass.length() != nameLength) sizeCount++;
    }

    if (sizeCount == 0)
    {
        j = 0;
        for (i = 0; i < rows; i = i + 2)
        {
            if ((i+1) < importList.size())
            {
                str1 = importList.at(i);
                str2 = importList.at(i+1);
                frameA.insert(j,str1);
                frameB.insert(j,str2);
                j++;
            }
        }
    }

    else
    {
        /* Thoughts on algorithm

        - Remove preceding part of string that is common to all files in the list
        - need to make a temporary QStringList
        - Remove the end of string that is common to all files in the list
        - Now we have two options (if files are ordered in some sensical way) :
        1) strings with 1a, 1b, 2a, 2b, ... 100a, 100b, .... (possible '_a' or preceding 'a', etc.)
        2) strings with 1, 2, 3, ...., 100, 101, ....
        - Test for option (2).  If true then distribute the list alternately and hope for the best.  If false
        then find matching 'a' and 'b' pairs and distribute them between the lists
        */
        common = importList.at(0);
        j = common.size();
        stop = false;
        while (!stop)
        {
            j--;
            common.resize(j);
            count = 0;
            for (i = 0; i < rows; i++)
            {
                pass = importList.at(i);
                if (pass.contains(common)) count++;
            }
            if (count == rows || common.size() == 0) stop = true;
        }

        for (i = 0; i < rows; i++)
        {
            pass = importList.at(i);
            tempList.append(pass.remove(common));
        }

        common = tempList.at(0);
        stop = false;
        count = 0;
        while (!stop)
        {
            common.remove(0,1);
            for (i = 0; i < rows; i++)
            {
                pass = tempList.at(i);
                if (pass.contains(common)) count++;
            }
            if (count == rows || common.size() == 0) stop = true;
        }

        count = 0;
        for (i = 0; i < rows; i++)
        {
            pass = tempList.at(i);
            pass.remove(common);
            tempList.insert(i,pass);
            if (onlyNum(pass)) count++;
        }

        // The case when there are no 'a' or 'b' marking or such, i.e. numbered 1,2,3,...
        if (count > 0)
        {
            for (i = 0; i < rows; i++) numList.append(tempList.at(i).toInt());
            k = 0;
            while (k < (importList.size() / 2))
            {
                int minIndex = -1;
                minNumber = 100000000;
                for (j = 0; j < rows; j++)
                {
                    if (numList.at(j) < minNumber)
                    {
                        minNumber = numList.at(j);
                        minIndex = j;
                    }
                }
                numList.replace(minIndex,100000000);
                numList.replace(minIndex + 1,100000000);
                tempList.removeAt(minIndex);
                tempList.removeAt(minIndex + 1);
                frameA.insert(k,importList.at(minIndex));
                frameB.insert(k,importList.at(minIndex + 1));
                k++;
            }
        }
        // The case of '1a,1b,2a,2b,...'
        else {
            tempA.clear(); tempB.clear();
            for (i = 0; i < rows; i = i + 2)
            {
                str1 = importList.at(i);
                str2 = importList.at(i+1);
                tempA.append(str1);
                tempB.append(str2);
            }

            // Now need to rank the lists
            k = 0;
            while (!tempA.isEmpty())
            {
                minLength = 1000000000; more = true;
                for (i = 0; i < tempA.size(); i++)
                {
                    str1 = tempA.at(i);
                if (str1.size() < minLength) minLength = str1.size();
                }
                while (more)
                {
                    count = 0;
                    for (j = 0; j < tempA.size(); j++)
                    {
                        str1 = tempA.at(j);
                        str2 = tempB.at(j);
                        if (str1.size() == minLength)
                        {
                            frameA.insert(k,str1);
                            frameB.insert(k,str2);
                            tempA.removeAt(j);
                            tempB.removeAt(j);
                            k++;
                        }
                        else count++;
                    }
                    if (count == tempA.size()) more = false;
                }
            }
        }
    }
    delete [] taken;
}

bool ImportImages::onlyNum(QString str)
{
    int i, count;
    count = 0;
    for (i = 0; i < str.size(); i++) if (str.at(i).isLetter()) count++;
    if (count > 0) return false;
    else return true;
}
