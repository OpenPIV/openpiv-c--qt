/*
====================================================================================

File: pivdata.cpp
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

#include "pivdata.h"
#include <iostream>
#include <QFile>
#include <QTextStream>

PivData::PivData()
{
    gridsCreated = false;
    maxComputed = false;
    _index = -2;
    _width = 0;
    _height = 0;
}

PivData::PivData(int widthPass, int heightPass)
{
    _width = widthPass;
    _height = heightPass;
    gridsCreated = false;
    maxComputed = false;
    // If width and height are known we can create and initialize grids
    createGrids();
    initializeToZero();
    _index = -2;
}

PivData::~PivData()
{
    deleteGrids();
}

void PivData::createGrids()
{
    // If the grids have already been created, delete previous grids
    if (gridsCreated) deleteGrids();
    grid = new PivPointData [_width*_height + 1];
    gridsCreated = true;
}

void PivData::initializeToZero()
{
    // Initializing over the entire grid
    if (gridsCreated)
    {
        int i,j;
        for (i = 0; i < _height; i++)
        {
            for (j = 0; j < _width; j++)
            {
                grid[i*_width + j] = oneToZero();
            }
        }
    }
}

PivPointData PivData::oneToZero()
{
    // Initializes a single point
    PivPointData pointData;
    pointData.x = 0.0;
    pointData.y = 0.0;
    pointData.u = 0.0;
    pointData.v = 0.0;
    pointData.snr = 0.0;
    pointData.valid = false;
    pointData.filtered = false;
    pointData.intensity = 0.0;
    return pointData;
}

void PivData::deleteGrids()
{
    // Checks to see if grids are created before delete dynamic grid data
    if (gridsCreated)
    {
        delete [] grid;
        gridsCreated = false;
    }
}

void PivData::setList(QList<PivPointData> _data)
{
    _list = _data;
    toGrids(_data);
}

QList<PivPointData> PivData::list()
{
    return _list;
}

int PivData::numValid()
{
    // Calculates the number of points which are valid (i.e., not masked)
    int i, j;
    int count = 0;
    for (i = 0; i < _height; i++)
    {
        for (j = 0; j < _width; j++)
        {
            if (grid[i*_width + j].valid) count++;
        }
    }

    return count;
}

void PivData::clear()
{
    deleteGrids();
}

bool PivData::isEmpty()
{
    return !gridsCreated;
}

int PivData::width()
{
    return _width;
}

int PivData::height()
{
    return _height;
}

int PivData::index()
{
    return _index;
}

void PivData::setIndex(int indexPass)
{
    _index = indexPass;
}

QString PivData::name()
{
    return _name;
}

void PivData::setName(QString filename)
{
    // Removes the suffix (e.g., "tif") from the end of the file
    int dotIndex = filename.length() - filename.lastIndexOf(".");
    filename.chop(dotIndex);
    _name = filename;
}

void PivData::read(int indexPass, QString filename, int imageHeight)
{
    // Function that reads in ASCII file data

    QFile file;
    QString in;
    PivPointData pointData;
    QList<PivPointData> pointList;

    _index = indexPass;

    file.setFileName(filename);

    // Currently here for debugging purposes
    std::cout << "Reading file: " << filename.toLocal8Bit().data() << std::endl;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream inStream(&file);
        in = inStream.readLine(); // Taking care of the header
        in = inStream.readLine();
        while(!(in.isNull()))
        {
            // Reads in first column: x
            pointData.x = in.section("\t",0,0).toDouble();
            // Reads in second column: y
            pointData.y = imageHeight - in.section("\t",1,1).toDouble();
            // Reads in third column: u
            pointData.u = in.section("\t",2,2).toDouble();
            // Reads in fourth column: v
            pointData.v = 0.0 - in.section("\t",3,3).toDouble();
            // Reads in fifth column: signal-to-noise ratio
            pointData.snr = in.section("\t",4,4).toDouble();
            // Reads in sixth column: validity of the point
            if (in.section("\t",5,5).toInt() == 1) pointData.valid = true;
            else pointData.valid = false;
            // Reads in seventh column: if the point has been filtered or not
            if (in.section("\t",6,6).toInt() == 1) pointData.filtered = true;
            else pointData.filtered = false;
            // Reads in the eigth column: average image intensity of the interrogation window
            pointData.intensity = in.section("\t",7,7).toDouble();

            // Data point is appended to the point list
            pointList.append(pointData);

            in = inStream.readLine();
        }
        file.close();
        // Point list is sent to be gridded
        toGrids(pointList);

    }
    // Do some proper debugging here at some point...
    else std::cout << "Problem reading file: " << filename.toLocal8Bit().data() << std::endl;
}

void PivData::toGrids(QList<PivPointData> _data)
{
    int i, j, m, removeX, removeY;
    int xCount, yCount;
    double xVal, yVal;
    double minX, minY;
    double *xTemp;
    double *yTemp;
    bool found;
    QList<double> xList;
    QList<double> yList;

    // First need to delete the grids if they exist
    deleteGrids();

    // Loop through the list data
    for (i = 0; i < _data.size(); i++)
    {
        xVal = _data.value(i).x;
        yVal = _data.value(i).y;
        xCount = 0; yCount = 0;
        /* Check to see how many unique values of x there are and create a list
           of these values */
        for (j = 0; j < xList.size(); j++) if (xVal == xList.value(j)) xCount++;
        if (xCount == 0) xList.append(xVal);
        /* Check to see how many unique values of y there are and create a list
           of these values */
        for (j = 0; j < yList.size(); j++) if (yVal == yList.value(j)) yCount++;
        if (yCount == 0) yList.append(yVal);
    }
    // Width is defined by the number of unique x values
    _width = xList.size();
    // Height is defined by the number of unique y values
    _height = yList.size();

    xTemp = new double [_width + 1];
    yTemp = new double [_height + 1];

    // Assign the memory for the grid
    createGrids();

    for (i = 0; i < _height; i++)
    {
        // Putting y values into the grid
        minY = 100000000.0;
        for (m = 0; m < yList.size(); m++)
        {
            if (yList.value(m) < minY)
            {
                minY = yList.value(m);
                removeY = m;
            }
        }
        yTemp[i] = minY;
        yList.removeAt(removeY);
        for (j = 0 ; j < _width; j++)
        {
            // Putting x values into the grid
            if (i == 0)
            {
                minX = 100000000.0;
                for (m = 0; m < xList.size(); m++)
                {
                    if (xList.value(m) < minX)
                    {
                        minX = xList.value(m);
                        removeX = m;
                    }
                }
                xTemp[j] = minX;
                xList.removeAt(removeX);
            }

            // Putting PivPointData onto the grid
            xVal = xTemp[j];
            yVal = yTemp[i];
            m = 0; found = false;
            while (m < _data.size() && !found)
            {
                /* If there is a data point in the list that corresponds to the
                    gridded value */
                if (xVal == _data.value(m).x && yVal == _data.value(m).y)
                {
                    grid[_width*i + j].u = _data.value(m).u;
                    grid[_width*i + j].v = _data.value(m).v;
                    grid[_width*i + j].x = xVal;
                    grid[_width*i + j].y = yVal;
                    grid[_width*i + j].snr = _data.value(m).snr;
                    grid[_width*i + j].valid = true;
                    grid[_width*i + j].filtered = _data.value(m).filtered;
                    grid[_width*i + j].intensity = _data.value(m).intensity;
                    found = true;
                }
                // If not, make the point blank and invalid
                else
                {
                    grid[_width*i + j].u = 0.0;
                    grid[_width*i + j].v = 0.0;
                    grid[_width*i + j].x = xVal;
                    grid[_width*i + j].y = yVal;
                    grid[_width*i + j].snr = 0.0;
                    grid[_width*i + j].valid = false;
                    grid[_width*i + j].filtered = false;
                    grid[_width*i + j].intensity = 0.0;
                }
                m++;
            }
        }
    }

    // Delete temporary grids
    delete [] xTemp;
    delete [] yTemp;
}

PivPointData PivData::min()
{
    if (maxComputed) return _min;
    else
    {
        computeMax();
        if (maxComputed) return _min;
        else return oneToZero();
    }
}

PivPointData PivData::max()
{
    if (maxComputed) return _max;
    else
    {
        computeMax();
        if (maxComputed) return _max;
        else return oneToZero();
    }
}


/*------- Positions -------------------*/

PivPointData PivData::data(int i, int j)
{
    PivPointData pointData;
    if (gridsCreated && i >= 0 && i < _height && j >= 0 && j < _width)
    {
        pointData = grid[i*_width+j];
    }
    else
    {
        pointData = oneToZero();
    }
    return pointData;
}

void PivData::setData(int i, int j, PivPointData dataPass)
{
    if (gridsCreated && i >= 0 && i < _height && j >= 0 && j < _width)
    {
        grid[i*_width+j] = dataPass;
    }
}

/* ---------- Validity ---------*/
bool PivData::isValid(int i, int j)
{
    if (gridsCreated && i >= 0 && i < _height && j >= 0 && j < _width)
    {
        return grid[_width*i + j].valid;
    }
    else return false;
}

///*------- Filtering -------------*/
bool PivData::filtered(int i, int j)
{
    if (gridsCreated && i >= 0 && i < _height && j >= 0 && j < _width)
    {
        return grid[i*_width + j].filtered;
    }
    else return false;
}

void PivData::setFilter(int i, int j, bool filterPass)
{
    if (gridsCreated && i >= 0 && i < _height && j >= 0 && j < _width)
    {
        grid[i*_width+j].filtered = filterPass;
    }
}

void PivData::computeMax()
{
    int i, j;
    if (gridsCreated)
    {
        _min.x = 1000000.0;
        _min.y = 1000000.0;
        _min.u = 1000000.0;
        _min.v = 1000000.0;
        _min.snr = 1000000.0;
        _min.intensity = 1000000.0;
        _min.filtered = false;
        _min.valid = true;

        _max.x = -10000000.0;
        _max.y = -10000000.0;
        _max.u = -10000000.0;
        _max.v = -10000000.0;
        _max.snr = -10000000.0;
        _max.intensity = -10000000.0;
        _max.filtered = false;
        _max.valid = true;

        for (i = 0; i < _height; i++)
        {
            for (j = 0; j < _width; j++)
            {
                if (grid[i*_width + j].x < _min.x) _min.x = grid[i*_width + j].x;
                if (grid[i*_width + j].y < _min.y) _min.y = grid[i*_width + j].y;
                if (grid[i*_width + j].u < _min.u) _min.u = grid[i*_width + j].u;
                if (grid[i*_width + j].v < _min.v) _min.v = grid[i*_width + j].v;
                if (grid[i*_width + j].snr < _min.snr) _min.snr = grid[i*_width + j].snr;
                if (grid[i*_width + j].intensity < _min.intensity) _min.intensity = grid[i*_width + j].intensity;

                if (grid[i*_width + j].x > _max.x) _max.x = grid[i*_width + j].x;
                if (grid[i*_width + j].y > _max.y) _max.y = grid[i*_width + j].y;
                if (grid[i*_width + j].u > _max.u) _max.u = grid[i*_width + j].u;
                if (grid[i*_width + j].v > _max.v) _max.v = grid[i*_width + j].v;
                if (grid[i*_width + j].snr > _max.snr) _max.snr = grid[i*_width + j].snr;
                if (grid[i*_width + j].intensity > _max.intensity) _max.intensity = grid[i*_width + j].intensity;
            }
        }

        maxComputed = true;
    }
}
