/*
====================================================================================

File: imagedata.cpp
Description: Image handling class to expand the capabilities of the QImage class.
    PIV images are now typically taken with monochrome cameras exceeding 8-bits.
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

#include "imagedata.h"

// std
#include <cmath>
#include <iostream>
#include <limits>
#include <algorithm>

// qt
#include <QString>
#include <QImage>

ImageData::ImageData()
    : buffer_( nullptr ),
      qImage_( nullptr )
{}

ImageData::ImageData( const QSize& s )
    : buffer_( nullptr ),
      qImage_( nullptr ),
      size_( s )
{
    createBuf();
}

ImageData::~ImageData()
{
    delete [] buffer_;
    delete qImage_;
}

QImage ImageData::toQImage() const
{
    // Converts buffer to a QImage if such an object does not already exist.
    // This is necessary for display purposes even though the algorithms are all
    // performed on the image buffers.
    if (!qImage_)
    {
        qImage_ = new QImage( size_, QImage::Format_RGB32 );
        const size_t height = size_.height();
        const size_t width = size_.width();
        
        double min = std::numeric_limits< double >::max();
        double max = std::numeric_limits< double >::min();
        for ( size_t i=0; i<height; ++i )
        {
            for ( size_t j=0; j<width; ++j )
            {
                max = std::max( buffer_[ i*width + j ], max );
                min = std::min( buffer_[ i*width + j ], min );
            }
        }

        for ( size_t i=0; i<height; ++i )
        {
            QRgb* line = (QRgb*)(qImage_->scanLine( i ));
            for ( size_t j=0; j<width; ++j )
            {
                double value = buffer_[ i*width + j ];
                int scaledValue = int( ceil((value - min) / (max - min) * 255));
                *(line + j) = qRgb(scaledValue, scaledValue, scaledValue);
            }
        }
    }
    
    return *qImage_;
}

void ImageData::createBuf()
{
    // Allocate memory
    delete [] buffer_;
    buffer_ = new double[ width() * height() ];
}

const double* ImageData::buffer() const
{
    return buffer_;
}

double* ImageData::buffer()
{
    return buffer_;
}

const double* ImageData::linebuffer( int row ) const
{
    if ( row < 0 || row >= size_.height() )
        throw ImageDataException( "row out of limits" );

    return buffer_ + ( row*width() );
}

double* ImageData::linebuffer( int row )
{
    return const_cast< ImageData* >( this )->linebuffer( row );
}

int ImageData::width() const
{
    return size_.width();
}

int ImageData::height() const
{
    return size_.height();
}

// Returns the gray value at (x,y) = (i,j)
double ImageData::pixel(int i, int j) const
{
    if (buffer_)
        return buffer_[i*size_.width() + j];
    else
        return -1.0;
}
