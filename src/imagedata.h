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

#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <QString>
#include <QImage>
#include <QPixmap>

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

    //! Constructor.
    ImageData();

    //! Destructor.
    virtual ~ImageData();

    // Read-write functions
    //! Function to read images.
    /*!
        Function to read images that takes the filename as an argument and returns the outcome.

        \param filename a QString object containing the full path of the image
        \return success of the operation
    */
    bool read(QString filename);

    //! Converts buffer to QImage for display.
    QImage toQImage();

    // Information functions
    //! Returns the width of the image
    int width();
    //! Returns the height of the image
    int height();
    //! Returns the bit depth of the image
    int bitDepth();

    //! Returns the gray value of the pixel at (x,y) = (j,i)
    double pixel(int i, int j);

    //! Returns a pointer to the image buffer
    double* buffer();

protected:
    //! Function to handle the special case of tiff files using libtiff4
    bool readTiff(QString filename);
    //! Allocation of the memory for the image
    void createBuf();
    //! Convert a colour image to grayscale
    double toGray(int r, int g, int b);

private:
    double *_buffer;
    int imageWidth, imageHeight;
    bool bufCreated, qImageCreated;
    bool isTiff;
    int bits;
    QImage *qImage;
    bool windowCreated;
};

#endif // IMAGEDATA_H
