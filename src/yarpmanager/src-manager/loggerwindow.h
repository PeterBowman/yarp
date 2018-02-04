/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * Author: Andrea Ruzzenenti
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <QWidget>
#include <QTimer>
#include <yarp/logger/YarpLogger.h>
#include <yarp/manager/executable.h>
#include <map>
class LoggerWidget;
namespace Ui
{
    class LoggerWindow;
}

class LoggerWindow : public QWidget
{
    Q_OBJECT
public:
    static LoggerWindow& getInstance(){static LoggerWindow instance; return instance;}

    void showLog(yarp::manager::Executable* ex);

private:
    explicit LoggerWindow(QWidget *parent = 0);
    std::vector<LoggerWidget*> logs;
    Ui::LoggerWindow *ui;
signals:

    private slots :
        void closeTab(int index);
    void updateFilter();
};
