#include <iostream>

#include "analysis.h"
#include "pivdata.h"
#include "settings.h"
#include "datacontainer.h"
#include "filteroptions.h"

#include "filters.h"

Analysis::Analysis(Settings *settingsPass, DataContainer *fileDataPass, QObject *parent) : QObject(parent)
{
    settings = settingsPass;
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
    if (filterOptions.snr()) snr(pivData,filterOptions);
    if (filterOptions.imageIntensity()) imageIntensity(pivData,filterOptions);
    if (filterOptions.globalRange()) globalRange(pivData,filterOptions);
    if (filterOptions.globalStd()) globalStd(pivData,filterOptions);
    if (filterOptions.local()) localDetect(pivData,filterOptions);
    if (filterOptions.interpolate()) meanInterpolate(pivData, filterOptions);
    if (filterOptions.smoothing()) gaussianBlur(pivData, filterOptions);
}
