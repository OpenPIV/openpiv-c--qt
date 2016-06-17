
#pragma once

#include "imageloader.h"

/// loader of TIFF images with support for bit depths
/// over 8-bits per channel; will sniff the input data for
/// TIFF of 0x49 49 2b 00 or 0x4d 4d 00 2b
class TIFFImageLoader : public ImageLoader
{
public:
    virtual ~TIFFImageLoader();

    virtual bool canLoad( QIODevice* iodevice ) const;
    virtual QSharedPointer< ImageData > load( QIODevice* iodevice ) const;
    virtual QString name() const;
    virtual int priority() const;
};
