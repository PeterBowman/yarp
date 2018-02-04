/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * Author: Andrea Ruzzenenti
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <QTableWidget>
#include <QAction>
#include <QMenu>
#include <yarp/manager/executable.h>
#include <QTimer>
#include <yarp/logger/YarpLogger.h>
#include <map>
class LoggerWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit LoggerWidget(yarp::manager::Executable* app, QWidget *parent = 0);
    ~LoggerWidget();
    void setFilter(bool info, bool error, bool debug, bool warning, bool trace)
    {
        filter.clear();
        if (info) filter.push_back(yarp::yarpLogger::LOGLEVEL_INFO);
        if (error) filter.push_back(yarp::yarpLogger::LOGLEVEL_ERROR);
        if (debug) filter.push_back(yarp::yarpLogger::LOGLEVEL_DEBUG);
        if (warning) filter.push_back(yarp::yarpLogger::LOGLEVEL_WARNING);
        if (trace) filter.push_back(yarp::yarpLogger::LOGLEVEL_TRACE);
    }

    yarp::manager::Executable* getExe() { return exe; }

private:

    yarp::manager::Executable*              exe{nullptr};
    QTimer*                                 timer{nullptr};
    std::vector<yarp::yarpLogger::LogLevel> filter;
    
    std::map<int, std::pair<std::string, QBrush>> levels;
signals:


private slots:
    void update();


};
