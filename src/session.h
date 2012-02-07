#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include "settings.h"

class Session : public QObject
{
    Q_OBJECT

    public:
        Session(Settings *settingsPass);
        virtual ~Session();

        void blank();
        void save(QString filename);
        bool open(QString fliename);


    protected:
        void read(QString filename);
        void write(QString filename);

    private:
        QString _name;
        Settings *settings;

};

#endif // SESSION_H
