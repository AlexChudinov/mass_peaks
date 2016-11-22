#include "data_export.h"

#include <QFile>
#include <QTextStream>

load_data_from_ascii_file::load_data_from_ascii_file(QVariant params)
    :
      file_name_(params.toString())
{
    if(file_name_.isEmpty()) Q_EMIT this->error("Could not convert constructor params to string.");
    this->setAutoDelete(false);
}

void load_data_from_ascii_file::run()
{
#define DEF_READ_ASSERT(flag, msg)\
    if(!(flag))\
    {\
        Q_EMIT this->error(msg); \
        return; \
    }

    QFile file(this->file_name_);

    DEF_READ_ASSERT(file.open(QIODevice::ReadOnly), QString("Fail to open file: ") + file_name_ + ".")

    QTextStream stream(&file);
    data_ptr_ = QSharedPointer<xy_data>(new xy_data);
    int line_number = 0;

    Q_EMIT this->progress_val(0);

    while(!stream.atEnd())
    {
        QString line = stream.readLine(); line_number++;
        QRegExp check_line("^[\\d+.]"); //Checks that line contains only numbers

        if(check_line.indexIn(line) == -1)
        {
            DEF_READ_ASSERT(line_number == 1,
                (QString("Text information in file: ") + file_name_ + " on line %1.").arg(line_number));
            continue;
        }

        QStringList xy_values = line.split("\t");

        DEF_READ_ASSERT((xy_values.size() == 2 && data_ptr_->w().empty()) || xy_values.size() == 3,
                        QString("Number of columns at line %1 is %2.").arg(line_number).arg(xy_values.size()));

        data_ptr_->x().push_back(xy_values[0].toDouble());
        data_ptr_->y().push_back(xy_values[1].toDouble());
        if(xy_values.size() == 3) data_ptr_->w().push_back(xy_values[2].toDouble());
    }

    Q_EMIT this->progress_val(100);

#undef DEF_READ_ASSERT
}

data_exporter* data_export_factory::create_data_exporter(DATA_EXPORT_TYPE type, QVariant params)
{
    switch (type) {
    case ASCII_FILE: return new load_data_from_ascii_file(params);
    default: return nullptr;
    }
}
