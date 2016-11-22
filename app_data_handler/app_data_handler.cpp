#include "../app_data_handler/app_data_handler.h"
#include "../app_data/data_export.h"
#include "../app_data/app_data.h"
#include "../app_data_handler/approximator_factory.h"

#include <QFile>
#include <QTextStream>
#include <QVector>

app_data_handler::app_data_handler(QObject *parent)
    :
      QThread(parent)
{
    connect(this, SIGNAL(started()), this, SIGNAL(busy()));
    connect(this, SIGNAL(finished()), this, SIGNAL(free()));
    connect(this, SIGNAL(finished()), this, SLOT(get_data()));
}

app_data_handler::~app_data_handler()
{

}

void app_data_handler::load_data(QString file_name)
{
    this->data_exporter_.reset(data_export_factory::create_data_exporter(ASCII_FILE, QVariant(file_name)));
    this->start();
    connect(this->data_exporter_.data(), SIGNAL(progress_val(int)), this, SIGNAL(progress_val(int)));
    connect(this->data_exporter_.data(), SIGNAL(error(QString)), this, SIGNAL(warning(QString)));
}

void app_data_handler::run()
{
    if(this->data_exporter_)
        this->data_exporter_->run();
}

QVector<double> app_data_handler::get_approximated_vals(const QVector<double> &x)
{
    if(!xy_data_approximator_)
        return QVector<double>();
    else return QVector<double>::fromStdVector(xy_data_approximator_->approximate(x.toStdVector()));
}

void app_data_handler::get_data()
{
    if(this->data_exporter_->data_ptr())
    {
        xy_data_ = this->data_exporter_->data_ptr();
        Q_EMIT this->data_changed(
                    vector_data_type::fromStdVector(xy_data_.data()->x()),
                    vector_data_type::fromStdVector(xy_data_.data()->y()));
        Q_EMIT this->dataChanged();
    }
}

void app_data_handler::set_approximator(QString name, void *params)
{
    if(name == "Cubic spline" && xy_data_)
    {
        cubic_spline_params* app_pars = reinterpret_cast<cubic_spline_params*>(params);
        if(!xy_data_->w().empty()) app_pars->weightings_ = xy_data_->w();
        xy_data_approximator_.reset(approximator_factory::create_approximator
                                    (CUBIC_SPLINE, app_pars,
                                     xy_data_->x(), xy_data_->y()));
        Q_EMIT this->approximatorChanged();
    }
}

void app_data_handler::set_approximator(QString name, double smoothing)
{
    if(name == "Cubic spline" && xy_data_)
    {
        cubic_spline_params app_pars;
        app_pars.smoothing_ = smoothing;
        app_pars.weightings_.assign(xy_data_->w().begin(), xy_data_->w().end());
        xy_data_approximator_.reset(approximator_factory::create_approximator
                                    (CUBIC_SPLINE, &app_pars,
                                     xy_data_->x(), xy_data_->y()));
        Q_EMIT this->approximatorChanged();
    }
}
