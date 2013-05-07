/*
====================================================================================

File: output.cpp
Description: Handling output of velocity data.
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

#include <QFile>
#include <QString>
#include <cmath>
#include <iostream>
#include "output.h"
#include "pivdata.h"

Output::Output(Settings *settingsPass, DataContainer *filedataPass, QObject *parent) : QObject(parent)
{
    // Assigning global settings object to class-local pointer
    _settings = settingsPass;
    // Assinging globa data container object to class-local pointer
    _filedata = filedataPass;
}

Output::~Output()
{

}

void Output::outputCurrent()
{
    // Output the data currently displayed in the image viewer
    switch(_settings->outputFormat())
    {
    case OpenPIV::Text:
        outputASCII(_filedata->currentPivData());
        break;
    case OpenPIV::HDF5:
        outputHDF5();
        break;
    default:
        break;
    }
}

void Output::write(PivData *pivData)
{
    // Calls protected function to write to an ASCII file
    outputASCII(pivData);
}

void Output::outputASCII(PivData *pivData)
{
    int i, j;

    // Index of the data to be written
    int current = pivData->index();

    QFile file;
    QString filename;
    QString outputString;
    PivPointData data;

    // Required to flip the y dimension so that (x,y)=(0,0) occurs in the lower-left rather than
    // upper left corner
    int imageHeight = _settings->imageSize().height();

    // Filename is assigned to the pivData object based on the index of the image when the data
    // are calculated
    filename = pivData->name();
    filename.append(".txt");

    // Setting the filename from the pivData object to the QFile object
    file.setFileName(filename);
    // Checking if the file object can be opened
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        // Writing the header information
        file.write("x [pixels], y [pixels], u [pixels], v [pixels], ");
        file.write("SNR [peak/mean], valid [0 if masked], filtered [1 if filtered], avg. intensity\n");

        // Iterating through the PIV data and writing the correspending data at each point
        for (i = 0; i < pivData->height(); i++)
        {
            for (j = 0; j < pivData->width(); j++)
            {
                data = pivData->data(i,j);
                outputString.clear();
                if (data.valid)
                {
                    outputString.append(QString("%1\t").arg(data.x,10,'g',10));
                    outputString.append(QString("%1\t").arg(imageHeight - data.y,10,'g',10));
                    outputString.append(QString("%1\t").arg(data.u,10,'g',10));
                    outputString.append(QString("%1\t").arg(-data.v,10,'g',10));
                    outputString.append(QString("%1\t").arg(data.snr,10,'g',10));
                    outputString.append("1\t");
                    if (data.filtered) outputString.append("1\t");
                    else outputString.append("0\t");
                    outputString.append(QString("%1").arg(data.intensity,10,'g',10));
                }
                // Sets the velocity to zero if the data point is not valid in addition to
                // listing the (boolean) validity of the data in the corresponding column
                else
                {
                    outputString.append(QString("%1\t").arg(data.x,10,'g',10));
                    outputString.append(QString("%1\t").arg(imageHeight - data.y,10,'g',10));
                    outputString.append(QString("%1\t").arg(0.0,10,'g',10));
                    outputString.append(QString("%1\t").arg(0.0,10,'g',10));
                    outputString.append(QString("%1\t").arg(0.0,10,'g',10));
                    outputString.append("0\t");
                    if (data.filtered) outputString.append("1\t");
                    else outputString.append("0\t");
                    outputString.append(QString("%1").arg(0.0,10,'g',10));
                }
                // The end-of-line character may cause platform compatibility issues
                outputString.append("\n");
                // Writing the line string to the file
                file.write(outputString.toLocal8Bit());
            }
        }
        // Setting the vector file name to the data container object (not very portable)
        _filedata->setVectorFile(current,filename);
        file.close();
    }
    // Printing an error to the terminal if the file cannot be opened.
    // THIS NEEDS IMPROVING SINCE THE USER WILL SEE NOTHING IF THIS ERROR IS THROWN
    else std::cout << "Problem opening file: " << filename.toLocal8Bit().data() << std::endl;
}

void Output::outputHDF5()
{
    // Does nothing right now
}
