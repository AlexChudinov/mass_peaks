#include <QHBoxLayout>
#include <QToolBar>
#include <QAction>

#include "../app_data/app_data.h"
#include "zoom_plot.h"
#include "qcustomplot/qcustomplot.h"

zoom_plot::zoom_plot(QWidget *parent)
    :
      QCustomPlot(parent),
      plot_action_(NO_ACTION),
      selection_area_(new QRubberBand(QRubberBand::Rectangle, this))
{}

zoom_plot::~zoom_plot(){}

void zoom_plot::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton)
    {
        this->mouse_press_position_ = event->pos();

        switch(this->plot_action_)
        {
        case HZOOM_IN:
        case VZOOM_IN:
            break;
        case ZOOM_OUT:
            this->zoom_out_();
            break;
        default:
            break;
        }
    }
    if(event->buttons() == Qt::RightButton)
    {
        this->set_plot_action(NO_ACTION);
    }
}

void zoom_plot::mouseReleaseEvent(QMouseEvent * /*event*/)
{
    switch(this->plot_action_)
    {
    case HZOOM_IN:
        hzoom_in_();
        this->selection_area_->hide();
        return;
    case VZOOM_IN:
        vzoom_in_();
        this->selection_area_->hide();
        return;
    case ZOOM_OUT: return;
    case NO_ACTION:return;
    }

    this->selection_area_->hide();
}

void zoom_plot::mouseMoveEvent(QMouseEvent *event)
{
    switch(this->plot_action_)
    {
    case HZOOM_IN:
        if(event->buttons() == Qt::LeftButton) set_hzoom_selection_area_(event->pos());
        break;
    case VZOOM_IN:
        if(event->buttons() == Qt::LeftButton) set_vzoom_selection_area_(event->pos());
        break;
    case ZOOM_OUT:  return;
    case NO_ACTION: return;
    }
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

        Q_EMIT this->hzoom_changed(action == HZOOM_IN);
        Q_EMIT this->vzoom_changed(action == VZOOM_IN);
        Q_EMIT this->zoom_out_changed(action == ZOOM_OUT);

        //Set action parameters
        switch(action)
        {
        case HZOOM_IN:
            this->setCursor(QCursor(QPixmap(":/Icons/hzoom")));
            break;
        case VZOOM_IN:
            this->setCursor(QCursor(QPixmap(":/Icons/vzoom")));
            break;
        case ZOOM_OUT:
            this->setCursor(QCursor(QPixmap(":/Icons/zoom_out")));
            break;
        default:
            this->setCursor(QCursor(Qt::ArrowCursor));
        }
    }
}

void zoom_plot::set_hzoom_selection_area_(const QPoint &pos)
{
    QPoint lower, upper;
    lower.setX(this->mouse_press_position_.x());
    lower.setY(this->yAxis->coordToPixel(this->yAxis->range().lower));
    upper.setX(pos.x());
    upper.setY(this->yAxis->coordToPixel(this->yAxis->range().upper));
    this->selection_area_->setGeometry(QRect(lower, upper).normalized());
    this->selection_area_->show();
}

void zoom_plot::set_vzoom_selection_area_(const QPoint &pos)
{
    QPoint lower, upper;
    lower.setX(this->xAxis->coordToPixel(this->xAxis->range().lower));
    lower.setY(this->mouse_press_position_.y());
    upper.setX(this->xAxis->coordToPixel(this->xAxis->range().upper));
    upper.setY(pos.y());
    this->selection_area_->setGeometry(QRect(lower, upper).normalized());
    this->selection_area_->show();
}

void zoom_plot::zoom_out_()
{
    double new_lower, new_upper;

    QCPRange
            xrange = this->xAxis->range(),
            yrange = this->yAxis->range();

    new_lower = xrange.lower - (xrange.center() - xrange.lower);
    new_upper = xrange.upper + (xrange.upper - xrange.center());
    this->xAxis->setRange(new_lower, new_upper);

    new_lower = yrange.lower - (yrange.center() - yrange.lower);
    new_upper = yrange.upper + (yrange.upper - yrange.center());
    this->yAxis->setRange(new_lower, new_upper);

    this->replot();
    Q_EMIT this->xrangeNotify();
    Q_EMIT this->yrangeNotify();
}

void zoom_plot::hzoom_in_()
{
    QRect selection = this->selection_area_->geometry();
    double new_lower = this->xAxis->pixelToCoord(selection.left());
    double new_upper = this->xAxis->pixelToCoord(selection.right());
    this->xAxis->setRange(new_lower, new_upper);
    this->replot();
    Q_EMIT this->xrangeNotify();
}

void zoom_plot::vzoom_in_()
{
    QRect selection = this->selection_area_->geometry();
    double new_lower = this->yAxis->pixelToCoord(selection.bottom());
    double new_upper = this->yAxis->pixelToCoord(selection.top());
    this->yAxis->setRange(new_lower, new_upper);
    this->replot();
    Q_EMIT this->yrangeNotify();
}

zoom_plot_window::zoom_plot_window(QWidget *parent)
    :
      QWidget(parent),
      plot_area_(new zoom_plot(this))
{
    QHBoxLayout* layout = new QHBoxLayout;
    this->setLayout(layout);

    QToolBar* toolbar = new QToolBar("Plot tools");
    toolbar->setOrientation(Qt::Vertical);
    layout->addWidget(toolbar);

    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(plot_area_);

    //Creates all toolbar actions
    QAction* hzoom_action    = new QAction(QIcon(":/Icons/hzoom"),"Horizontal zoom", this);
    QAction* vzoom_action    = new QAction(QIcon(":/Icons/vzoom"),"Vertical zoom", this);
    QAction* zoom_out_action = new QAction(QIcon(":/Icons/zoom_out"), "Zoom out", this);
    QAction* rescale_action  = new QAction(QIcon(":/Icons/rescale"), "Set default axis", this);

    //...set them checkable
    hzoom_action->setCheckable(true);
    vzoom_action->setCheckable(true);
    zoom_out_action->setCheckable(true);

    //...put them onto toolbar
    toolbar->addAction(hzoom_action);
    toolbar->addAction(vzoom_action);
    toolbar->addAction(zoom_out_action);
    toolbar->addAction(rescale_action);

    //...and connect them
    connect(hzoom_action, SIGNAL(toggled(bool)), this->plot_area_, SLOT(set_hzoom(bool)));
    connect(this->plot_area_, SIGNAL(hzoom_changed(bool)), hzoom_action, SLOT(setChecked(bool)));
    connect(vzoom_action, SIGNAL(toggled(bool)), this->plot_area_, SLOT(set_vzoom(bool)));
    connect(this->plot_area_, SIGNAL(vzoom_changed(bool)), vzoom_action, SLOT(setChecked(bool)));
    connect(zoom_out_action, SIGNAL(toggled(bool)), this->plot_area_, SLOT(set_zoom_out(bool)));
    connect(this->plot_area_, SIGNAL(zoom_out_changed(bool)), zoom_out_action, SLOT(setChecked(bool)));
    connect(rescale_action, SIGNAL(triggered()), this->plot_area_, SLOT(rescaleAxes()));
    connect(rescale_action, SIGNAL(triggered()), this->plot_area_, SLOT(replot()));
    connect(rescale_action, SIGNAL(triggered()), this->plot_area_, SIGNAL(xrangeNotify()));
    connect(rescale_action, SIGNAL(triggered()), this->plot_area_, SIGNAL(yrangeNotify()));
}

zoom_plot_window::~zoom_plot_window(){}
