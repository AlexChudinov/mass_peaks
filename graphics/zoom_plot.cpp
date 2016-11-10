#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>

#include "zoom_plot.h"
#include "qcustomplot/qcustomplot.h"

zoom_plot::zoom_plot(QWidget *parent)
    :
      QCustomPlot(parent),
      plot_action_(NO_ACTION),
      selection_area_(nullptr)
{}

zoom_plot::~zoom_plot(){}

void zoom_plot::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton)
    {
        mouse_press_position_ = event->pos();
        selection_area_ = new QRubberBand(QRubberBand::Rectangle, this);
    }
    if(event->buttons() == Qt::RightButton)
    {
        this->set_plot_action(NO_ACTION);
    }
}

void zoom_plot::mouseMoveEvent(QMouseEvent *event)
{

}

void zoom_plot::set_hzoom(bool hzoom)
{
    if(hzoom) this->set_plot_action(HZOOM_IN);
    else if(!hzoom && this->plot_action_ == HZOOM_IN) this->set_plot_action(NO_ACTION);
}

void zoom_plot::set_vzoom(bool vzoom)
{
    if(vzoom) this->set_plot_action(VZOOM_IN);
    else if(!vzoom && this->plot_action_ == VZOOM_IN) this->set_plot_action(NO_ACTION);
}

void zoom_plot::set_zoom_out(bool zoom_out)
{
    if(zoom_out) this->set_plot_action(ZOOM_OUT);
    else if(!zoom_out && this->plot_action_ == ZOOM_OUT) this->set_plot_action(NO_ACTION);
}

void zoom_plot::set_plot_action(CURRENT_PLOT_ACTION action)
{
    if(this->plot_action_ != action)
    {
        this->plot_action_ = action;

        emit this->hzoom_changed(action == HZOOM_IN);
        emit this->vzoom_changed(action == VZOOM_IN);
        emit this->zoom_out_changed(action == ZOOM_OUT);

        //Set action parameters
        switch(action)
        {
        case HZOOM_IN:
            this->setCursor(QCursor(QBitmap(":/Icons/hzoom")));
            break;
        case VZOOM_IN:
            this->setCursor(QCursor(QBitmap(":/Icons/vzoom")));
            break;
        case ZOOM_OUT:
            this->setCursor(QCursor(QBitmap(":/Icons/zoom_out")));
            break;
        default:
            this->setCursor(QCursor(Qt::ArrowCursor));
        }
    }
}

zoom_plot_window::zoom_plot_window(QWidget *parent)
    :QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout;
    this->setLayout(layout);

    QToolBar* toolbar = new QToolBar("Plot tools");
    layout->addWidget(toolbar);

    zoom_plot* plot_area = new zoom_plot(this);
    layout->addWidget(plot_area);

    //Creates all toolbar actions
    QAction* hzoom_action = new QAction(QIcon(":/Icons/hzoom"),"Horizontal zoom", this);
    QAction* vzoom_action = new QAction(QIcon(":/Icons/vzoom"),"Vertical zoom", this);
    QAction* zoom_out_action = new QAction(QIcon(":/Icons/zoom_out"), "Zoom out", this);

    //...set them checkable
    hzoom_action->setCheckable(true);
    vzoom_action->setCheckable(true);
    zoom_out_action->setCheckable(true);

    //...put them onto toolbar
    toolbar->addAction(hzoom_action);
    toolbar->addAction(vzoom_action);
    toolbar->addAction(zoom_out_action);

    //...and connect them
    connect(hzoom_action, SIGNAL(toggled(bool)), plot_area, SLOT(set_hzoom(bool)));
    connect(plot_area, SIGNAL(hzoom_changed(bool)), hzoom_action, SLOT(setChecked(bool)));
    connect(vzoom_action, SIGNAL(toggled(bool)), plot_area, SLOT(set_vzoom(bool)));
    connect(plot_area, SIGNAL(vzoom_changed(bool)), vzoom_action, SLOT(setChecked(bool)));
    connect(zoom_out_action, SIGNAL(toggled(bool)), plot_area, SLOT(set_zoom_out(bool)));
    connect(plot_area, SIGNAL(zoom_out_changed(bool)), zoom_out_action, SLOT(setChecked(bool)));
}

zoom_plot_window::~zoom_plot_window(){}
