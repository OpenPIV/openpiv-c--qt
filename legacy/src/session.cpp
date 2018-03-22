#include "session.h"
#include "settings.h"
#include "processing.h"
#include "output.h"
#include "datacontainer.h"
#include "analysis.h"

#include <QtXml>
#include <iostream>

Session::Session()
{
    initialize();
}

Session::~Session()
{
    destroy();
}

void Session::destroy()
{
    delete _settings;
    delete _filedata;
    delete _process;
    delete _analysis;
    delete _output;
}

void Session::initialize()
{
    _settings = new Settings;
    _filedata = new DataContainer(_settings);
    _process = new Processing(_settings,_filedata);
    _analysis = new Analysis(_settings,_filedata);
    _output = new Output(_settings,_filedata);
}

Settings* Session::settings() { return _settings; }
DataContainer* Session::filedata() { return _filedata; }
Processing* Session::process() { return _process; }
Analysis* Session::analysis() { return _analysis; }
Output* Session::output() { return _output; }

void Session::save()
{
    this->write("filename");
}

void Session::fresh()
{

}

bool Session::open()
{
    this->read("filename");
}

void Session::write(QString filename)
{
    QString _filename = "D:/temp/trial.xml";

    QFile file(_filename);
    std::cout << "In here" << std::endl;

    const int Indent = 4;
    QDomDocument doc;
    QDomElement data = doc.createElement("Data");
    QDomElement settings = doc.createElement("Settings");

    QDomElement images = doc.createElement("Images");
    QDomElement vectorFiles = doc.createElement("VectorFiles");
    doc.appendChild(data);
    data.appendChild(images);
    data.appendChild(vectorFiles);

    QDomElement imageRoot = doc.createElement("ImageRoot");
    QDomText imageRootText = doc.createTextNode("/image/root/directory/");
    imageRoot.appendChild(imageRootText);
    images.appendChild(imageRoot);
    QDomElement imageList = doc.createElement("ImageList");
    images.appendChild(imageList);
    QDomText imageFile;
    QDomElement imageIndex;
    for (int i = 0; i < 100; i++)
    {
        imageIndex = doc.createElement(QString("image%1").arg(i));
        imageList.appendChild(imageIndex);
        imageFile = doc.createTextNode(QString("sample_image_file%1.TIF").arg(i));
        imageIndex.appendChild(imageFile);
    }

    QDomElement vectorRoot = doc.createElement("VectorRoot");
    QDomText vectorRootText = doc.createTextNode("/vector/root/directory/");
    vectorRoot.appendChild(vectorRootText);
    vectorFiles.appendChild(vectorRoot);
    QDomElement vectorList = doc.createElement("VectorList");
    vectorFiles.appendChild(vectorList);
    QDomText vectorFile;
    QDomElement vectorIndex;
    for (int i = 0; i < 100; i++)
    {
        vectorIndex = doc.createElement(QString("vector%1").arg(i));
        vectorList.appendChild(vectorIndex);
        vectorFile = doc.createTextNode(QString("sample_vector_file%1.txt").arg(i));
        vectorIndex.appendChild(vectorFile);
    }

    doc.appendChild(settings);

    if (file.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream out(&file);
        doc.save(out, Indent);
    }
}

void Session::read(QString filename)
{
    QString _filename = "D:/temp/trial.xml";
    QFile file(_filename);

    QString str;
    if (file.open(QFile::ReadOnly))
    {
        QDomDocument doc;
        doc.setContent(&file);
        QDomNode docNode;
        QDomNode dataNode;
        QDomNode imagesNode;

        docNode = doc.firstChild();
        QDomElement data = docNode.toElement();
        QDomElement settings = docNode.nextSibling().toElement();

        dataNode = data.firstChild();
        QDomElement images = dataNode.toElement();
        QDomElement vectorFiles = dataNode.nextSibling().toElement();

        // Deal with image list
        QDomNode imageListNode;
        imagesNode = images.firstChild();
        QDomElement imageRoot = imagesNode.toElement();
        std::cout << "Image root: " << imageRoot.text().toLocal8Bit().data() << std::endl;
        QDomElement imageList = imagesNode.nextSibling().toElement();
        imageListNode = imageList.firstChild();
        while (!imageListNode.isNull())
        {
            QDomElement imageFile = imageListNode.toElement();
            std::cout << imageFile.text().toLocal8Bit().data() << std::endl;
            imageListNode = imageListNode.nextSibling();
        }

        std::cout << str.toLocal8Bit().data() << std::endl;
    }
}
