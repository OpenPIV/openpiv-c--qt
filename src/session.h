/*
====================================================================================

File: session.h
Description: Handles initialization of a PIV session.  A session can also be thought
    of as a separate experiment (i.e., image size, etc. could change).
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

#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include "settings.h"
#include "processing.h"
#include "output.h"
#include "datacontainer.h"
#include "analysis.h"

//! Handles saving, opening and initialization of PIV sessions.
/*!
    Handles saving, opening and initialization of PIV sessions.  A session can also
    be thought of as a separate experiment (i.e., image size, etc. could change).
*/

class Session : public QObject
{

Q_OBJECT

public:
    //! Constructor
    // Not sure the settings object should be passed this way.  Most likely a session
    // holds the settings for that session and MainWindow receives this information
    // from the session.
    Session();
    //! Destructor
    virtual ~Session();

    Settings* settings();
    DataContainer* filedata();
    Processing* process();
    Analysis* analysis();
    Output* output();

public slots:

    //! Providing new session functionality
    void fresh();
    //! Save function
    void save();
    //! Open function
    bool open();

protected:
    void initialize();
    void destroy();

    //! Protected function to write session details
    void read(QString filename);
    //! Protected function to read session details
    void write(QString filename);

private:
    QString _name;
    Settings *_settings;
    DataContainer *_filedata;
    Processing *_process;
    Analysis *_analysis;
    Output *_output;
};

#endif // SESSION_H
