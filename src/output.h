/*
====================================================================================

File: output.h
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


#ifndef OUTPUT_H
#define OUTPUT_H

#include <QObject>
#include "settings.h"
#include "datacontainer.h"
#include "pivdata.h"

//!  Class handling output of velocity data
/*!
  Currently handling only ASCII output to *.txt files.  However, the goal
  is to eventually integrate HDF5 functionality for large data sets.
*/

class Output : public QObject
{
Q_OBJECT

public:
    //! Constructor
    /*!
        Initializes output object

        \param Settings object to allow object to read user defined settings
        \param DataContainer object to allow objcet to list vector files which have been written to disk
        \return success of the operation
    */
    explicit Output(Settings *settingsPass, DataContainer *filedataPass, QObject *parent = 0);

    //! Destructor.
    virtual ~Output();

public slots:
    //! Outputs the velocity data of the image pair currently shown in the viewer
    void outputCurrent();
    //! Writes the given PivData to a file.  The filename is specified in the PivData object.
    void write(PivData *pivData);

signals:

protected:
    //! Protected function to write the velocity data in the PivData object to an ASCII file (*.txt)
    void outputASCII(PivData *pivData);
    //! Right now this does nothing, but in future versions will interface to the HDF5 output
    void outputHDF5();

private:
    Settings *_settings;
    DataContainer *_filedata;
};

#endif // OUTPUT_H
