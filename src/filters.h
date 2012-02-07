#ifndef FILTERS_H
#define FILTERS_H

#include "filteroptions.h"
#include "pivdata.h"

// SNR
void snr(PivData *pivData, FilterOptions filterOptions);

// Image intensity

void imageIntensity(PivData *pivData, FilterOptions filterOptions);

// Global
void globalRange(PivData *pivData, FilterOptions filterOptions);
void globalStd(PivData *pivData, FilterOptions filterOptions);

// Local
void localDetect(PivData *pivData, FilterOptions filterOptions);

// Interpolation
void meanInterpolate(PivData *pivData, FilterOptions filterOptions);

// Smoothing
void gaussianBlur(PivData *pivData, FilterOptions filterOptions);

#endif // FILTERS_H
