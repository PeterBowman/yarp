/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * Author: Andrea Ruzzenenti
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <loggerwindow.h>
#include "ui_loggerwindow.h"
#include <loggerwidget.h>

LoggerWindow::LoggerWindow(QWidget *parent) : QWidget(parent, Qt::Window), ui(new Ui::LoggerWindow)
{
    ui->setupUi(this);
    ui->tabContainer->removeTab(0);
    ui->tabContainer->removeTab(1);
    connect(ui->tabContainer, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(ui->Info, SIGNAL(stateChanged(int)), this, SLOT(updateFilter()));
    connect(ui->Error, SIGNAL(stateChanged(int)), this, SLOT(updateFilter()));
    connect(ui->Debug, SIGNAL(stateChanged(int)), this, SLOT(updateFilter()));
    connect(ui->Warning, SIGNAL(stateChanged(int)), this, SLOT(updateFilter()));
    connect(ui->Trace, SIGNAL(stateChanged(int)), this, SLOT(updateFilter()));
    connect(ui->tabContainer, SIGNAL(currentChanged(int)), this, SLOT(updateFilter()));
}

void LoggerWindow::closeTab(int index)
{

    delete ui->tabContainer->widget(index);
    ui->tabContainer->removeTab(index);
}

void LoggerWindow::showLog(yarp::manager::Executable* ex)
{
    for (int i = 0; i < ui->tabContainer->count(); i++)
    {
        LoggerWidget* lw = dynamic_cast<LoggerWidget*>(ui->tabContainer->currentWidget());
        if (lw  && lw->getExe() == ex)
        {
            ui->tabContainer->setCurrentIndex(i);
            show();
            raise();
            return;
        }
    }
    LoggerWidget* log = new LoggerWidget(ex, this);
    log->setFilter(ui->Info->isChecked(), ui->Error->isChecked(), ui->Debug->isChecked(), ui->Warning->isChecked(), ui->Trace->isChecked());
    ui->tabContainer->addTab(log, ex->getCommand());
    
    show();
    raise();
}

void LoggerWindow::updateFilter()
{
    LoggerWidget* lw = dynamic_cast<LoggerWidget*>(ui->tabContainer->currentWidget());
    if (lw)
    {
        lw->setFilter(ui->Info->isChecked(), ui->Error->isChecked(), ui->Debug->isChecked(), ui->Warning->isChecked(), ui->Trace->isChecked());
    }
}