
#pragma once

#include "ImageLoader.h"

/// loader of PNM images with support for bit depths
/// over 8-bits per channel; will sniff the input data for
/// P[1-6] header.
///
/// Treats contained data as linear i.e. not a "true" PNM
/// image as no gamma correction is applied
class PNMImageLoader : public ImageLoader
{
public:
    virtual ~PNMImageLoader();

    virtual bool canLoad( std::istream& ) const override;
    virtual bool canSave() const override;
    virtual void load( std::istream&, G16Image& ) const override;
    virtual void load( std::istream&, GFImage& ) const override;
    virtual void load( std::istream&, RGBA16Image& ) const override;
    virtual void save( std::ostream&, const G16Image& ) const override;
    virtual void save( std::ostream&, const GFImage& ) const override;
    virtual void save( std::ostream&, const RGBA16Image& ) const override;
    virtual void save( std::ostream&, const G16ImageView& ) const override;
    virtual void save( std::ostream&, const GFImageView& ) const override;
    virtual void save( std::ostream&, const RGBA16ImageView& ) const override;

    virtual std::string name() const override;
    virtual int priority() const override;
};
