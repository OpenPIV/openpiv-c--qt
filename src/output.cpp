#include <QFile>
#include <QString>
#include <cmath>
#include <iostream>
#include "output.h"
#include "pivdata.h"

Output::Output(Settings *settingsPass, DataContainer *filedataPass, QObject *parent) : QObject(parent)
{
    _settings = settingsPass;
    _filedata = filedataPass;
}

Output::~Output()
{

}

void Output::outputCurrent()
{
    switch(_settings->outputFormat())
    {
    case OpenPIV::Text:
        outputASCII(_filedata->currentPivData());
        break;
    case OpenPIV::HDF5:
        outputHDF5();
        break;
    default:
        break;
    }
}

void Output::write(PivData *pivData)
{
    outputASCII(pivData);
}

void Output::outputASCII(PivData *pivData)
{
    int i, j;
    int current = pivData->index();
    QFile file;
    QString filename;
    QString outputString;
    PivPointData data;

    int imageHeight = _settings->imageSize().height();

    filename = pivData->name();

    filename.append(".txt");
    file.setFileName(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        file.write("x [pixels], y [pixels], u [pixels], v [pixels], ");
        file.write("SNR [peak/mean], valid [0 if masked], filtered [1 if filtered], avg. intensity\n");
        for (i = 0; i < pivData->height(); i++)
        {
            for (j = 0; j < pivData->width(); j++)
            {
                data = pivData->data(i,j);
                outputString.clear();
                if (data.valid)
                {
                    outputString.append(QString("%1\t").arg(data.x,10,'g',10));
                    outputString.append(QString("%1\t").arg(imageHeight - data.y,10,'g',10));
                    outputString.append(QString("%1\t").arg(data.u,10,'g',10));
                    outputString.append(QString("%1\t").arg(-data.v,10,'g',10));
                    outputString.append(QString("%1\t").arg(data.snr,10,'g',10));
                    outputString.append("1\t");
                    if (data.filtered) outputString.append("1\t");
                    else outputString.append("0\t");
                    outputString.append(QString("%1").arg(data.intensity,10,'g',10));
                }
                else
                {
                    outputString.append(QString("%1\t").arg(data.x,10,'g',10));
                    outputString.append(QString("%1\t").arg(imageHeight - data.y,10,'g',10));
                    outputString.append(QString("%1\t").arg(0.0,10,'g',10));
                    outputString.append(QString("%1\t").arg(0.0,10,'g',10));
                    outputString.append(QString("%1\t").arg(0.0,10,'g',10));
                    outputString.append("0\t");
                    if (data.filtered) outputString.append("1\t");
                    else outputString.append("0\t");
                    outputString.append(QString("%1").arg(0.0,10,'g',10));
                }

                outputString.append("\n");
                file.write(outputString.toLocal8Bit());
            }
        }
        _filedata->setVectorFile(current,filename);
        file.close();
    }
    else std::cout << "Problem opening file: " << filename.toLocal8Bit().data() << std::endl;
}

void Output::outputHDF5()
{
    // Does nothing right now
}
