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
    if (gridsCreated) deleteGrids();
    grid = new PivPointData [_width*_height + 1];
    gridsCreated = true;
}

void PivData::initializeToZero()
{
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
    if (gridsCreated)
    {
        delete [] grid;
        gridsCreated = false;
    }
}

void PivData::setLists(QList<PivPointData> _data)
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
    int dotIndex = filename.length() - filename.lastIndexOf(".");
    filename.chop(dotIndex);
    _name = filename;
}

void PivData::read(int indexPass, QString filename, int imageHeight)
{
    QFile file;
    QString in;
    PivPointData pointData;
    QList<PivPointData> pointList;


    _index = indexPass;

    file.setFileName(filename);
    std::cout << "Reading file: " << filename.toLocal8Bit().data() << std::endl;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream inStream(&file);
        in = inStream.readLine();
        while(!(in.isNull()))
        {
            pointData.x = in.section("\t",0,0).toDouble();
            pointData.y = imageHeight - in.section("\t",1,1).toDouble();
            pointData.u = in.section("\t",2,2).toDouble();
            pointData.v = 0.0 - in.section("\t",3,3).toDouble();
            pointData.snr = in.section("\t",4,4).toDouble();
            if (in.section("\t",5,5).toInt() == 1) pointData.valid = true;
            else pointData.valid = false;
            if (in.section("\t",6,6).toInt() == 1) pointData.filtered = true;
            else pointData.filtered = false;
            pointData.intensity = in.section("\t",7,7).toDouble();

            pointList.append(pointData);

            in = inStream.readLine();

        }
        file.close();
        std::cout << "About to call toGrids" << std::endl;
        toGrids(pointList);

    }
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

    deleteGrids();
    for (i = 0; i < _data.size(); i++)
    {
        xVal = _data.value(i).x;
        yVal = _data.value(i).y;
        xCount = 0; yCount = 0;
        for (j = 0; j < xList.size(); j++) if (xVal == xList.value(j)) xCount++;
        if (xCount == 0) xList.append(xVal);
        for (j = 0; j < yList.size(); j++) if (yVal == yList.value(j)) yCount++;
        if (yCount == 0) yList.append(yVal);
    }
    _width = xList.size();
    _height = yList.size();

    xTemp = new double [_width + 1];
    yTemp = new double [_height + 1];

    createGrids();
    std::cout << "width: " << _width << " height: " << _height << std::endl;

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

            // Putting u and v onto the grid
            xVal = xTemp[j];
            yVal = yTemp[i];
            m = 0; found = false;
            while (m < _data.size() && !found)
            {
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

    std::cout << "read the data entirely" << std::endl;

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

/*-------- Velocity ----------------*/

//QPointF PivData::velocity(int i, int j)
//{
//    QPointF _velocity;
//    if (gridsCreated && i >= 0 && i < _height && j >= 0 && j < _width)
//    {
//        _velocity.setX(_u[_width*i + j]);
//        _velocity.setY(_v[_width*i + j]);
//    }
//    else
//    {
//        _velocity.setX(0.0);
//        _velocity.setY(0.0);
//    }
//    return _velocity;
//}

//void PivData::setVelocity(int i, int j, QPointF velocityPass)
//{
//    if (gridsCreated && i >= 0 && i < _height && j >= 0 && j < _width)
//    {
//        _u[i*_width+j] = velocityPass.x();
//        _v[i*_width+j] = velocityPass.y();
//    }
//}

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


