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
#include <QString>
#include <QImage>
#include <cmath>
#include <iostream>
#include <tiffio.h>

ImageData::ImageData()
{   
    bufCreated = false;
    qImageCreated = false;
    isTiff = false;
}

ImageData::~ImageData()
{
    if (bufCreated) delete [] _buffer;
    if (qImageCreated) delete qImage;
}

// Read-write functions

bool ImageData::read(QString filename)
{
    int i, j;
    bool check;
    /* Checking to see if the supplied file is a tiff image or not based on the extension provided.  If
       the image is a tiff it is read with the libtiff library, otherwise the QImage class is used. */
    if (filename.endsWith("tif", Qt::CaseInsensitive) || filename.endsWith("tiff", Qt::CaseInsensitive))
    {
        isTiff = true;
        // Calls the routine that implements the functions of the libtiff library.
        check = readTiff(filename);
        return check;
    }
    else
    {
        isTiff = false;
        if (qImageCreated) delete qImage;
        qImage = new QImage(filename);
        qImageCreated = true;
        if (!qImage->isNull())
        {
            imageHeight = qImage->height();
            imageWidth = qImage->width();

            if (!bufCreated) createBuf();

            for (i = 0; i < imageHeight; i++)
            {
                for (j = 0; j < imageWidth; j++)
                {
                    if (qImage->allGray()) _buffer[i*imageWidth + j] = double(QColor(qImage->pixel(j,i)).red());
                    else
                    {
                        QColor color = qImage->pixel(j,i);
                        int r = color.red();
                        int g = color.green();
                        int b = color.blue();
                        _buffer[i*imageWidth + j] = toGray(r,g,b);
                    }
                }
            }
            bits = 8;
            return true;
        }
        else
        {
            if (qImageCreated) delete qImage;
            qImageCreated = false;
            std::cout << "failed to read image\n";
            return false;
        }
    }
}

double ImageData::toGray(int r, int g, int b)
{
    return (0.2989*double(r) + 0.5870*double(g) + 0.1140*double(b));
}

bool ImageData::readTiff(QString filename)
{
    int i,j;
    TIFF *tif;
    uint16 spp, bpp, photo;
    int linesize;
    char* buf;

    tif = TIFFOpen(filename.toLocal8Bit().data(),"r");

    // Checking to make sure the file was opened successfully.
    if (tif)
    {
        // Reading the data from the tiff file's header
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageHeight);
        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imageWidth);
        TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bpp);
        TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
        TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photo);
        linesize = TIFFScanlineSize(tif);

        bits = bpp;

        if (bits > 8)
        {
            // Creating the 16-bit buffer and the 8-bit buffer
            if (!bufCreated) createBuf();

            buf = new char[linesize * imageHeight];
            bool error = false;
            for (i = 0; i < imageHeight; i++)
            {
                // Reading the data line by line
                TIFFReadScanline(tif, &buf[i * linesize], i, 0);
                for (j = 0; j < imageWidth; j++)
                {
                    if (spp == 1) _buffer[i*imageWidth + j] = double(((uint16 *)buf + imageWidth*i*spp)[j*spp]);
                    else if (spp == 3)
                    {
                        int r = int(((uint16 *)buf + imageWidth*i*spp)[j*spp]);
                        int g = int(((uint16 *)buf + imageWidth*i*spp)[j*spp+1]);
                        int b = int(((uint16 *)buf + imageWidth*i*spp)[j*spp+2]);
                        _buffer[i*imageWidth + j] = toGray(r,g,b);
                    }
                    else
                    {
                        _buffer[i*imageWidth + j] = 0.0;
                        error = true;
                    }
                }
            }
            if (error) std::cout << "something weird happened\n";
            TIFFClose(tif);
            delete buf;
        }
        else
        {
            /* Performs much the same operation as the routine when bits > 8 without creating the
               16-bit buffer */

            if (!bufCreated) createBuf();

            buf = new char[linesize * imageHeight];
            bool error = false;
            for (i = 0; i < imageHeight; i++)
            {
                TIFFReadScanline(tif, &buf[i * linesize], i, 0);
                for (j = 0; j < imageWidth; j++)
                {
                    if (spp == 1) _buffer[i*imageWidth + j] = double(((uint8 *)buf + imageWidth*i*spp)[j*spp]);
                    else if (spp == 3)
                    {
                        int r = int(((uint8 *)buf + imageWidth*i*spp)[j*spp]);
                        int g = int(((uint8 *)buf + imageWidth*i*spp)[j*spp+1]);
                        int b = int(((uint8 *)buf + imageWidth*i*spp)[j*spp+2]);
                        _buffer[i*imageWidth + j] = toGray(r,g,b);
                    }
                    else
                    {
                        _buffer[i*imageWidth + j] = 0.0;
                        error = true;
                    }
                }
            }
            if (error) std::cout << "something weird happened\n";
            TIFFClose(tif);
            delete buf;
        }

        return true;
    }
    else return false;
}

QImage ImageData::toQImage()
{
    /* Converts buffer to a QImage if such an object does not already exist.
       This is necessary for display purposes even though the algorithms are all
       performed on the character buffers. */
    int i, j;
    double value;
    int scaledValue;

    if (!qImageCreated || qImage->isNull())
    {
        if (qImageCreated) delete qImage;
        qImage = new QImage(imageWidth,imageHeight,QImage::Format_ARGB32);
        qImageCreated = true;

        double min = 1000000.0; double max = -1000000.0;
        for (i = 0; i < imageHeight; i++)
        {
            for (j = 0; j < imageWidth; j++)
            {
                if (_buffer[i*imageWidth + j] > max) max = _buffer[i*imageWidth + j];
                if (_buffer[i*imageWidth + j] < min) min = _buffer[i*imageWidth + j];
            }
        }

        for (i = 0; i < imageHeight; i++)
        {
            for (j = 0; j < imageWidth; j++)
            {
                value = _buffer[i*imageWidth + j];
                scaledValue = int(ceil((value - min) / (max - min) * 255));
                qImage->setPixel(j,i,qRgb(scaledValue, scaledValue, scaledValue));
            }
        }
    }
    return *qImage;
}

void ImageData::createBuf()
{
    _buffer = new double [imageWidth * imageHeight + 1];
    bufCreated = true;
}

// Information functions
double* ImageData::buffer()
{
    return _buffer;
}

// Returns the image width
int ImageData::width()
{
    return imageWidth;
}

// Returns the image height
int ImageData::height()
{
    return imageHeight;
}

// Returns the image bit depth
int ImageData::bitDepth()
{
    return bits;
}

// Returns the gray value at (x,y) = (i,j)
double ImageData::pixel(int i, int j)
{
    if (bufCreated) return _buffer[i*imageWidth + j];
    else return -1.0;
}
