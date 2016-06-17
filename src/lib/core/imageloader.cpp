
#include "imageloader.h"

// qt
#include <QString>
#include <QIODevice>
#include <QList>
#include <QtAlgorithms>
#include <QDebug>

typedef QList< QSharedPointer< ImageLoader > > ImageLoaderList;

static ImageLoaderList& loaders()
{
    static ImageLoaderList static_loaders;
    return static_loaders;
}

QSharedPointer< ImageLoader > ImageLoader::findLoader( QIODevice* iodevice )
{
    for ( auto I=loaders().constBegin(), E=loaders().constEnd(); I!=E; ++I )
        if ( (*I)->canLoad( iodevice ) )
            return *I;

    return QSharedPointer< ImageLoader >();
}

bool ImageLoader::registerLoader( ImageLoader* loader )
{
    Q_ASSERT( loader );
    loaders().push_back( QSharedPointer< ImageLoader >( loader ) );
    qStableSort( loaders().begin(), loaders().end(),
                 []( const QSharedPointer< ImageLoader >& lhs,
                     const QSharedPointer< ImageLoader >& rhs ) -> bool
                 {
                     return lhs->priority() < rhs->priority();
                 }
        );

    qDebug() << "registered " << loader->name();
    return true;
}
