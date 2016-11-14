#include "../app_data_handler/app_data_handler.h"
#include "../app_data/data_export.h"

#include <QFile>
#include <QTextStream>

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
    this->data_exporter_ = data_export_factory::create_data_exporter(ASCII_FILE, QVariant(file_name));
    this->start();
    connect(this->data_exporter_.data(), SIGNAL(progress_val(int)), this, SIGNAL(progress_val(int)));
    connect(this->data_exporter_.data(), SIGNAL(error(QString)), this, SIGNAL(warning(QString)));
}

void app_data_handler::run()
{
    if(this->data_exporter_)
        this->data_exporter_->run();
}

void app_data_handler::get_data()
{
    if(this->data_exporter_->data_ptr()) xy_data_ = this->data_exporter_->data_ptr();
    Q_EMIT this->data_changed(xy_data_.data());
}
