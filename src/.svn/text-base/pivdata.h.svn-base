#ifndef PIVDATA_H
#define PIVDATA_H

#include <QPointF>
#include <QList>
#include <QString>

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

class PivData
{

    public:
        PivData();
        PivData(int widthPass, int heightPass);
        virtual ~PivData();

        void setLists(QList<PivPointData> _data);
        QList<PivPointData> list();

        PivPointData data(int i, int j);
        void setData(int i, int j, PivPointData dataPass);

        PivPointData min();
        PivPointData max();
//        QPointF position(int i, int j);
//        void setPosition(int i, int j, QPointF positionPass);
//        QPointF velocity(int i, int j);
//        void setVelocity(int i, int j, QPointF velocityPass);       

        void clear();
        bool isEmpty();
        int width(); // Columns
        int height(); // Rows
        int index();
        int numValid();

        bool isValid(int i, int j);
        bool filtered(int i, int j);
        void setFilter(int i, int j, bool filterPass);
        void setIndex(int indexPass);
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

};

#endif // PIVDATA_H
