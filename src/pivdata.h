/*
====================================================================================

File: pivdata.h
Description: The PivData class hold the results of the cross-correlation analysis.
  The data is cast to a regular grid and is made up of position and velocity, as
  well as other features such as: signal-to-noise ratio, validity, etc.
Copyright (C) 2012  OpenPIV (http://www.openpiv.net)

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

#ifndef PIVDATA_H
#define PIVDATA_H

#include <QPointF>
#include <QList>
#include <QString>

//! Struct describing the PIV data which varies from point to point
/*!
    Co-ordinate system (x,y) = (0,0) is located in the lower left
    of the image.
*/
struct PivPointData
{
    //! Horizontal dimension in pixels
    double x;
    //! Vertical dimension in pixels
    double y;
    //! Horizontal displacement in pixels
    double u;
    //! Vertical displacement in pixels
    double v;
    //! Signal-to-noise (peak to mean) ratio
    double snr;
    //! False if masked and true otherwise
    bool valid;
    //! True if marked as erroneous and false otherwise
    bool filtered;
    //! Average gray scale of the interrogation window
    double intensity;
};

//! Class that holds PIV data
/*!
  The PivData class hold the results of the cross-correlation analysis.  The
  data is cast to a regular grid and is made up of position and velocity, as
  well as other features such as: signal-to-noise ratio, validity, etc.
*/
class PivData
{

    public:
        //! Empty constructor.
        /*!
          The empty constructor allows for the creation of a PivData() object
          without knowledge of the size (width and height).
        */
        PivData();
        //! Overloaded constructor.
        /*!
          An overloaded constructor that allocates the size of the PivData()
          object upon creation.
        */
        PivData(int widthPass, int heightPass);
        //! Destructor.
        virtual ~PivData();

        //! Takes as argument a list of PivPointData and casts it to a regular grid.
        void setList(QList<PivPointData> _data);
        //! Returns the list of PivPointData
        /*!
          This function is provided for convenience and is not the main method
          for getting PIV data from an object.  Use data() instead.

          \sa data();
        */
        QList<PivPointData> list();

        //! Returns the data at (i,j) = (row, column) = (y, x)
        /*!
          \sa setData();
        */
        PivPointData data(int i, int j);
        //! Sets the data by passing PivPointData at (i,j) = (row, column) = (y, x)
        /*!
          \sa data();
        */
        void setData(int i, int j, PivPointData dataPass);

        //! Returns the minimum of each member in the struct PivPointData
        /*!
            The minimum value for each member does not necessarily occur at the same
            grid point.  In fact, it most likely does not.

            \sa max()
        */
        PivPointData min();
        //! Returns the maximum of each member in the struct PivPointData
        /*!
            The maximum value for each member does not necessarily occur at the same
            grid point.  In fact, it most likely does not.

            \sa min()
        */
        PivPointData max();

        /*! Deletes the gridded data.  \sa isEmpty() */
        void clear();
        /*! Returns true if the grids have been initialized with data.  \sa clear() */
        bool isEmpty();

        /*! Returns the number of columns in the grid. \sa height() */
        int width();
        /*! Returns the number of rows in the grid. \sa width() */
        int height();

        //! Returns the integer index of the data within the larger dataset.
        int index();
        //! Returns a QString of the filename of the object.
        QString name();
        //! Returns the number of valid grid points (i.e., those which have not been masked).
        int numValid();

        //! Returns true if the point (i,j) = (y,x) has not been masked.
        bool isValid(int i, int j);
        //! Retruns true if the point (i,j) = (y,x) has been filtered.
        bool filtered(int i, int j);
        //! Set the value of the filter flag at point (i,j) = (y,x).
        void setFilter(int i, int j, bool filterPass);

        //! Takes as argument the value of the index for the object.
        void setIndex(int indexPass);
        //! Takes a QString as argument for the filename of the object.
        void setName(QString filename);
        //! Function to read data from an ASCII text file written by OpenPIV.
        void read(int indexPass, QString filename, int imageHeight);

    protected:
        //! Assigns list data privately and fills grids
        void toGrids(QList<PivPointData> _data);
        //! Creates dynamic grid data
        void createGrids();
        //! Initialization function for the gridded data
        void initializeToZero();
        //! Function to set all values in the PivPointData struct to zero
        PivPointData oneToZero();
        //! Deletes the dynamic grid data
        void deleteGrids();
        //! Computes min and max values
        /*!
          /sa max()
          /sa min()
          */
        void computeMax();

    private:
        int _index;
        int _width;
        int _height;
        bool gridsCreated;
        bool maxComputed;
        PivPointData _max;
        PivPointData _min;
        PivPointData *grid;
        QList<PivPointData> _list;
        QString _name;
};

#endif // PIVDATA_H
