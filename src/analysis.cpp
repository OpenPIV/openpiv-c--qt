/*
====================================================================================

File: analysis.cpp
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

#include <iostream>

#include "analysis.h"
#include "pivdata.h"
#include "settings.h"
#include "datacontainer.h"
#include "filteroptions.h"

#include "filters.h"

Analysis::Analysis(Settings *settingsPass, DataContainer *fileDataPass, QObject *parent) : QObject(parent)
{
    // Global session settings
    settings = settingsPass;
    // Object containing file name and index information for images and vector files
    filedata = fileDataPass;
}

Analysis::~Analysis()
{
}

void Analysis::filterCurrent()
{
    if (filedata->hasCurrentVectors())
    {
        filterData(filedata->currentPivData(), settings->filterOptions());
        emit(currentFiltered());
    }
}

void Analysis::filterData(PivData *pivData, FilterOptions filterOptions)
{
    // Checking each filtering option and calling external filter functions (included from filters.h)
    if (filterOptions.snr()) snr(pivData,filterOptions);
    if (filterOptions.imageIntensity()) imageIntensity(pivData,filterOptions);
    if (filterOptions.globalRange()) globalRange(pivData,filterOptions);
    if (filterOptions.globalStd()) globalStd(pivData,filterOptions);
    if (filterOptions.local()) localDetect(pivData,filterOptions);
    if (filterOptions.interpolate()) meanInterpolate(pivData, filterOptions);
    if (filterOptions.smoothing()) gaussianBlur(pivData, filterOptions);
}
