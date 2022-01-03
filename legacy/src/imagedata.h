/*
====================================================================================

File: imagedata.h
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

#pragma once

// std
#include <stdexcept>

// qt
#include <QString>
#include <QImage>
#include <QPixmap>
#include <QSize>

class ImageDataException : public std::runtime_error
{
public:
    ImageDataException( const std::string& what )
        : std::runtime_error( what )
    {}
};


//!  Class handling image data
/*!
  Image handling class to expand the capabilities of the QImage class.
  PIV images are now typically taken with monochrome cameras exceeding 8-bits.
  This class relies on the libtiff4 library to extend the limit of 8-bit depth
  of the QImage class to 16-bit depth for tiff images.  QImage handles all
  other compatible formats.
*/
class ImageData
{
public:
    typedef double PixelType;
    
    //! Constructor.
    ImageData();
    ImageData( const QSize& );

    //! Destructor.
    virtual ~ImageData();

    //! Converts buffer to QImage for display.
    QImage toQImage() const;

    // Information functions
    //! Returns the width of the image
    int width() const;
    //! Returns the height of the image
    int height() const;

    QSize size() const;
    
    //! Returns the bit depth of the image
    int bitDepth() const;

    //! Returns the gray value of the pixel at (x,y) = (j,i)
    double pixel(int i, int j) const;

    //! Returns a pointer to the image buffer
    const double* buffer() const;
    double* buffer();

    //! Returns a pointer to the start of a line
    const double* linebuffer( int row ) const;
    double* linebuffer( int row );

private:
    //! Allocation of the memory for the image
    void createBuf();

    //! Convert a colour image to grayscale
    double toGray(int r, int g, int b);

    double *buffer_;
    mutable QImage *qImage_;
    QSize size_;
};
