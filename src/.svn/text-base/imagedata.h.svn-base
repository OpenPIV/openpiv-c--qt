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

class ImageData
{
    public:
        ImageData();
        virtual ~ImageData();

        // Read-write functions
        bool read(QString filename);
        bool write(QString filename, unsigned char* data, int width, int height);
        QImage toQImage();

        // Information functions
        int width();
        int height();
        int bitDepth();

        //int pixel(int i, int j);
        double pixel(int i, int j);

        //unsigned char* buffer();
        double* buffer();

    protected:
        bool readTiff(QString filename);
        void createBuf();
        double toGray(int r, int g, int b);
//        void createBuf16();
//        void createBuf8();
//        void createBuf8(QImage qImage);

    private:
        double *_buffer;
        //unsigned char* imageBuf8;
        //const unsigned char* constBuf8;
        //unsigned short* imageBuf16;
        int imageWidth, imageHeight;
        //bool buf8created, buf16created, qImageCreated;
        bool bufCreated, qImageCreated;
        bool isTiff;
        int bits;
        QImage *qImage;
        bool windowCreated;
};

#endif // IMAGEDATA_H
