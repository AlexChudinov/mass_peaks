#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>

#include "zoom_plot.h"
#include "qcustomplot/qcustomplot.h"

zoom_plot::zoom_plot(QWidget *parent)
    :
      QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    this->setLayout(layout);

    //Creates graphic toolbar
    QToolBar* toolbar = new QToolBar("Plot tools", this);
    layout->addWidget(toolbar);

    //Creates and connects horizontal zoom action
    QAction* hzoom_action = new QAction(QIcon(":/Icons/hzoom"),"Horizontal zoom", this);
    toolbar->addAction(hzoom_action);
    connect(hzoom_action, SIGNAL(toggled(bool)), this, SLOT(set_hzoom(bool)));
    connect(this, SIGNAL(hzoom_changed(bool)), hzoom_action, SLOT(setChecked(bool)));

    QCustomPlot* qcustomplot = new QCustomPlot(this);
    layout->addWidget(qcustomplot);
}

zoom_plot::~zoom_plot(){}

void zoom_plot::set_hzoom(bool hzoom)
{
    this->hzoom_ = hzoom;
}

void zoom_plot::set_vzoom(bool vzoom)
{
    this->vzoom_ = vzoom;
}
