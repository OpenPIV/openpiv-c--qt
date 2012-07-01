/*
====================================================================================

File: analysis.h
Description: This class is continually under development to include new analyses in
  post-processing. The most fundamental post-processing is filtering, but
  many others such as numerical differentiation, time-averaging, etc. can
  be added to this class.  The primary function of this class is to pass
  data to external functions.
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


#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <QObject>

#include "pivdata.h"
#include "settings.h"
#include "filteroptions.h"
#include "datacontainer.h"

//!  Class to handle analysis of PIV data
/*!
  This class is continually under development to include new analyses in
  post-processing. The most fundamental post-processing is filtering, but
  many others such as numerical differentiation, time-averaging, etc. can
  be added to this class.  The primary function of this class is to pass
  data to external functions.
*/

class Analysis : public QObject
{
    Q_OBJECT

    public:
        //! Constructor
        Analysis(Settings *settingsPass, DataContainer *fileDataPass, QObject *parent = 0);
        //! Destructor
        virtual ~Analysis();

        //! Takes a PivData object and a FilterOptions object and filters the data
        void filterData(PivData *pivData, FilterOptions filterOptions);

    public slots:
        //! Slot to process the currently displayed data
        void filterCurrent();

    signals:
        //! Emits when the current data have been filtered
        void currentFiltered();

    private:
        Settings *settings;
        DataContainer *filedata;
};

#endif // ANALYSIS_H
