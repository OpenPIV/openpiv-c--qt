
#pragma once

#include "ImageLoader.h"

/// loader of TIFF images with support for bit depths
/// over 8-bits per channel; will sniff the input data for
/// TIFF of 0x49 49 2b 00 or 0x4d 4d 00 2b
class TIFFImageLoader : public ImageLoader
{
public:
    virtual ~TIFFImageLoader();

    virtual bool canLoad( std::istream& ) const override;
    virtual void load( std::istream&, G16Image& ) const override;
    virtual void load( std::istream&, DoubleImage& ) const override;
    virtual void load( std::istream&, RGBA16Image& ) const override;

    virtual std::string name() const override;
    virtual int priority() const override;
};
