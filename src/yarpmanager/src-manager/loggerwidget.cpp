/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * Author: Andrea Ruzzenenti
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <loggerwidget.h>
#include <yarp/manager/yarpbroker.h>
#include <qsizepolicy.h>
#include <qheaderview.h>
using namespace yarp::manager;


LoggerWidget::LoggerWidget(yarp::manager::Executable* app, QWidget* parent) : QTableWidget(0, 3, parent), exe(app)
{
    timer = new QTimer(this);
    timer->connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);

    levels[yarp::yarpLogger::LOGLEVEL_INFO] = std::make_pair("INFO", QBrush(QColor(125, 125, 255)));
    levels[yarp::yarpLogger::LOGLEVEL_ERROR] = std::make_pair("ERROR", QBrush(QColor(255, 50, 50)));
    levels[yarp::yarpLogger::LOGLEVEL_DEBUG] = std::make_pair("DEBUG", QBrush(QColor(50, 255, 50)));
    levels[yarp::yarpLogger::LOGLEVEL_WARNING] = std::make_pair("WARNING", QBrush(QColor(50, 255, 255)));
    levels[yarp::yarpLogger::LOGLEVEL_TRACE] = std::make_pair("TRACE", QBrush(QColor(125, 0, 125)));
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    setHorizontalHeaderLabels(QString("Log Type;Message;Time Stamp").split(";"));
    horizontalHeader()->setStretchLastSection(true);
    
}

LoggerWidget::~LoggerWidget()
{ 
    exe = nullptr;
    timer->stop();
    delete timer; 
    timer = nullptr;
    
    for (int i = 0; i < rowCount(); i++)
    {
        delete item(i, 0);
        delete item(i, 1);
        delete item(i, 2);
    }
    clear();
}

void LoggerWidget::update()
{
    
    YarpBroker* b;
    if(exe && exe->getBroker())
    {
        b = dynamic_cast<YarpBroker*>(exe->getBroker());
    }
    else
    {
        return;
    }

    

    for (int i = 0; i < rowCount(); i++)
    {
        delete item(i, 0);
        delete item(i, 1);
        delete item(i, 2);
    }
    clear();
    setRowCount(0);

    if (b)
    {
        auto reverseLogList = b->getLog();
        std::reverse(reverseLogList.begin(), reverseLogList.end());
        for(auto s : reverseLogList)
        {

            if(std::find(filter.begin(), filter.end(), s.level) != filter.end())
            {
                insertRow(0);
                setRowHeight(0, 20);
                setItem(0, 0, new QTableWidgetItem(levels[s.level.toInt()].first.c_str()));
                item(0, 0)->setBackground(levels[s.level.toInt()].second);
                setItem(0, 1, new QTableWidgetItem(s.text.c_str()));
                setItem(0, 2, new QTableWidgetItem(s.local_timestamp.c_str()));
            }
        }
    }
    setHorizontalHeaderLabels(QString("Log Type;Message;Time Stamp").split(";"));
    horizontalHeader()->setStretchLastSection(true);
}
