
#pragma once

#include "imageloader.h"

/// loader using Qt's QImageLoaders 
class QtImageLoader : public ImageLoader
{
public:
    virtual ~QtImageLoader();

    virtual bool canLoad( QIODevice* iodevice ) const;
    virtual QSharedPointer< ImageData > load( QIODevice* iodevice ) const;
    virtual QString name() const;
    virtual int priority() const;
};
