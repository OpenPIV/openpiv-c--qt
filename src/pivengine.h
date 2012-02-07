#ifndef PIVENGINE_H
#define PIVENGINE_H

#include <QPoint>
#include <QList>
#include <QMutex>

#include "settings.h"
#include "datacontainer.h"
#include "pivdata.h"
#include "imagedata.h"

class PivEngine
{
    public:
        PivEngine(Settings *settingsPass, QList<QPoint> gridPass);
        virtual ~PivEngine();

        typedef PivData* result_type;

        PivData* operator()(MetaData dataPass);

    protected:
        PivPointData velocity(int topLeftRow, int topLeftColumn);
        virtual bool crossCorrelate(int topLeftRow, int topLeftColumn);
        double imageMean(ImageData *image, int topLeftRow, int topLeftColumn);
        void loadImages();

        // Access functions
        MetaData data();
        //Settings* settings();
        QList<QPoint> grid();
        int intLengthX();
        int intLengthY();
        int detector();

        bool imagesAvailable();
        ImageData* imageA();
        ImageData* imageB();
        double meanImageA();
        double meanImageB();

        double* cmap();

    private:
        //Settings *_settings;
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
