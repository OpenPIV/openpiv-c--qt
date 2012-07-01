/*
====================================================================================

File: pivengine.h
Description: The base class for current and future correlation methods.  The class
    is designed to be thread safe and future developers are encouraged to build on
    this class by inheritance and by reimplementing the crossCorrelate function.
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


#ifndef PIVENGINE_H
#define PIVENGINE_H

#include <QPoint>
#include <QList>
#include <QMutex>

#include "settings.h"
#include "datacontainer.h"
#include "pivdata.h"
#include "imagedata.h"

//!  Base PIV engine class to be inherited by cross-correlation classes.
/*!
    The base class for current and future correlation methods.  The class
    is designed to be thread safe and future developers are encouraged to build on
    this class by inheritance and by reimplementing the crossCorrelate function.

    Parallelization in OpenPIV is accomplished through distribution of the image
    pairs to be cross-correlated rather than dividing each image across multiple
    cores.
*/

class PivEngine
{
    public:
        //! Constructor
        /*!
            The basic PivEngine class requires the global settings object and the list of
            grid points to be calculated.

            \param settingsPass is a pointer to the global Settings objeccct
            \param gridPass is a copy of the list of grid points to be computed in the image
        */
        PivEngine(Settings *settingsPass, QList<QPoint> gridPass);

        //! Virtual Destructor
        virtual ~PivEngine();

        //! The type of result expected from the public operator() function
        typedef PivData* result_type;

        //! Given a MetaData object returns the result of the PIV calculation
        PivData* operator()(MetaData dataPass);

    protected:
        //! Basic steps used in all single-iteration PIV schemes (i.e., not multi-pass)
        /*!
            Taking as input the top-left image co-ordinates this function calls the
            crossCorrelate function to obtain a correlation map.  The correlation map
            is then passed to the sub-pixel estimator and pixel displacement data
            are obtained and returned through a PivPointData object.

            \param Integer co-ordinate of the top left row (i.e., vertical position) of the interrogation window
            \param Integer co-ordinate of the top left column (i.e., horizontal position) of the interrogation window
            \returns PivPointData object containing co-ordinate, velocity, etc.

            \sa PivPointData
        */
        PivPointData velocity(int topLeftRow, int topLeftColumn);
        //! Protected function that is altered upon inheritance of the PivEngine Class
        /*!
            The PivEngine class assumes no correlation scheme itself.  In order to calculate the velocity
            one must create a class inheriting PivEngine and re-implementing the crossCorrelate function.
            The correlation map is created by the PivEngine class and must be written to by the child class
            using the access function cmap().

            \param Integer co-ordinate of the top left row (i.e., vertical position) of the interrogation window
            \param Integer co-ordinate of the top left column (i.e., horizontal position) of the interrogation window
            \returns True if successful, false otherwise.

            \sa cmap()
        */
        virtual bool crossCorrelate(int topLeftRow, int topLeftColumn);
        //! Computes the average intensity of the interrogation window.
        /*!
            \param One of the images of the current image pair (either A or B)
            \param Integer co-ordinate of the top left row (i.e., vertical position) of the interrogation window
            \param Integer co-ordinate of the top left column (i.e., horizontal position) of the interrogation window
        */
        double imageMean(ImageData *image, int topLeftRow, int topLeftColumn);
        //! Function to load the current image pair (A and B) into memory
        void loadImages();

        // Access functions provided for classes inheriting PivEngine

        //! Access for inheriting classes to the MetaData of the image pair
        MetaData data();
        //! Access for inheriting classes to the list of interrogation windows
        QList<QPoint> grid();
        //! Access for inheriting classes to the horizontal interrogation length
        int intLengthX();
        //! Access for inheriting classes to the vertical interrogation length
        int intLengthY();

        //! Access for inheriting classes to see if images have been loaded properly
        bool imagesAvailable();
        //! Access for inheriting classes to the ImageData for image A
        ImageData* imageA();
        //! Access for inheriting classes to the ImageData for image B
        ImageData* imageB();
        //! Access for inheriting classes to the mean of interrogation window A
        double meanImageA();
        //! Access for inheriting classes to the mean of interrogation window B
        double meanImageB();

        //! Access for inheriting classes to the cmap object through a pointer
        double* cmap();

    private:
        int _intLengthX, _intLengthY;
        int _detector;
        QList<QPoint> _grid;

        MetaData _data;

        ImageData *_imageA;
        ImageData *_imageB;
        double _meanImageA;
        double _meanImageB;
        bool imagesCreated;

        double *_cmap;
        bool cmapCreated;

        QMutex mutex;
};

#endif // PIVENGINE_H
