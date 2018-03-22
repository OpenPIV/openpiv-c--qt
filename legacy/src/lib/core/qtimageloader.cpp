
#include "qtimageloader.h"

// qt
#include <QImage>
#include <QImageReader>
#include <QIODevice>

// local
#include <imagedata.h>

static const bool registeredloader = ImageLoader::registerLoader< QtImageLoader >();

QtImageLoader::~QtImageLoader()
{}

bool QtImageLoader::canLoad( QIODevice* iodevice ) const
{
    return QImageReader( iodevice ).canRead();
}

QSharedPointer< ImageData > QtImageLoader::load( QIODevice* iodevice ) const
{
    if ( !iodevice )
        throw ImageLoaderException( "iodevice is NULL" );

    QImageReader reader( iodevice );
    QSharedPointer< ImageData > im( new ImageData( reader.size() ) );
    QImage qimage( reader.read() );
    if ( qimage == QImage() )
        throw ImageLoaderException(
            qPrintable(
                QString( "failed to load image: %1" ).arg( reader.errorString() ) ) );

    const size_t width = im->width();
    const size_t height = im->height();
    
    for (size_t i=0; i<height; ++i)
    {
        ImageData::PixelType* line = im->linebuffer( i );
        for (size_t j=0; j<width; ++j)
            *(line + j) = qGray( qimage.pixel( j, i ) );
    }
    
    return im;
}

QString QtImageLoader::name() const
{
    return "QtImageLoader";
}

int QtImageLoader::priority() const
{
    return 2;
}
