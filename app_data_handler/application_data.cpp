#include "../app_data_handler/application_data.h"

#include <QFile>
#include <QTextStream>

const xy_data& application_data::add_new_xy_data(const std::string &name, const xy_data &d)
{
    xy_data_map_[name] = d;
    return xy_data_map.at(name);
}

const xy_data& application_data::get_xy_data(const std::string &name) const
{
    return xy_data_map_.at(name);
}

application_data_handler::application_data_handler(QObject* parent)
    :
      QThread(parent),
      process_(&application_data_handler::load_data_file)
{
    connect(this, SIGNAL(started()), this, SIGNAL(application_data_holder_busy()));
    connect(this, SIGNAL(finished()), this, SIGNAL(application_data_holder_free()));
}

void application_data_handler::run()
{
    (this->*(this->process_))(this->file_name_);
}

void application_data_handler::set_process(application_data_handler::PROCESS_ID process_id)
{
    switch(process_id)
    {
    case LOAD_DATA:
        this->process_ = &application_data_handler::load_data_file;
        break;
    default:
        return;
    }
}

void application_data_handler::load_data_file(const QString& file_name)
{
    QFile file(file_name);
    if(file.open(QIODevice::ReadOnly))
    {
        size_t progress_val = 0;
        QTextStream stream(&file);

        //Count a numbers of lines
        size_t number_of_lines = 0;
        while(!stream.atEnd())
        {
            stream.readLine();
            ++number_of_lines;
        }

        //We are skipping first line because it contains some text data
        stream.seek(0); stream.readLine();

        mass_spectrums_[file_name.toStdString()] = mass_spectrum();
        mass_spectrums_[file_name.toStdString()].first.reserve(number_of_lines-1);
        mass_spectrums_[file_name.toStdString()].second.reserve(number_of_lines-1);

        for(size_t i = 0; i < number_of_lines-1; ++i)
        {
            stream >> mass_spectrums_[file_name.toStdString()].first[i]
                   >> mass_spectrums_[file_name.toStdString()].second[i];
            progress_val += 100*i/number_of_lines;
            emit this->progress_value(progress_val);
        }

        emit this->mass_spectrum_appended(&mass_spectrums_[file_name.toStdString()]);
    }
    else
    {
        emit this->warning("Could not open file!");
    }
}
