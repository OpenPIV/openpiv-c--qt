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
    \param x is the horizontal dimension of type double
    \param y is the vertical dimension of type double
    \param u is the horizontal displacement of type double
    \param v is the vertical displacement of type double
    \param snr is the signal-to-noise ratio of type double
    \param valid is a boolean defining the validity of a grid point.
    A grid point is invalid if it is masked.
    \param filtered is a boolean specifying if the data point has
    been flagged during the filtering process as being erroneous.
    \param intensity is the average gray scale of the interrogation window
    represented by the grid point (stored as a double)
*/
struct PivPointData
{
    double x;
    double y;
    double u;
    double v;
    double snr;
    bool valid;
    bool filtered;
    double intensity;
};

//! PivData Class
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

        void setLists(QList<PivPointData> _data);
        QList<PivPointData> list();

        PivPointData data(int i, int j);
        void setData(int i, int j, PivPointData dataPass);

        PivPointData min();
        PivPointData max();

        void clear();
        bool isEmpty();
        int width(); // Columns
        int height(); // Rows
        int index();
        QString name();
        int numValid();

        bool isValid(int i, int j);
        bool filtered(int i, int j);
        void setFilter(int i, int j, bool filterPass);
        void setIndex(int indexPass);
        void setName(QString filename);
        void read(int indexPass, QString filename, int imageHeight);

    protected:
        void toGrids(QList<PivPointData> _data);
        void createGrids();
        void initializeToZero();
        PivPointData oneToZero();
        void deleteGrids();
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
