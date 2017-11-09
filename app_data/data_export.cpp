#include "data_export.h"

#include <QFile>
#include <QTextStream>

#define DEF_ASSERT_FILE_NAME(name)\
    if(name.isEmpty()) Q_EMIT this->error("Could not convert constructor params to string.");

#define DEF_READ_ASSERT(flag, msg)\
    if(!(flag))\
    {\
        Q_EMIT this->error(msg); \
        return; \
    }

load_data_from_ascii_file::load_data_from_ascii_file(QVariant params)
    :
      file_name_(params.toString())
{
    DEF_ASSERT_FILE_NAME(file_name_)
    this->setAutoDelete(false);
}

void load_data_from_ascii_file::run()
{
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
}

LoadCsv::LoadCsv(QVariant params)
    :
      m_strFileName(params.toString())
{
    DEF_ASSERT_FILE_NAME(m_strFileName)
    this->setAutoDelete(false);
}

void LoadCsv::run()
{
    QFile file(m_strFileName);
    DEF_READ_ASSERT(file.open(QIODevice::ReadOnly), QString("Failed to open file!"))

    QTextStream stream(&file);
    m_DataPtr.reset(new xy_data);
    int nLineNum = 0;

    while (!stream.atEnd())
    {
        QString line = stream.readLine(); nLineNum ++;
        QRegExp check("^[\\d+.,]");
        if(check.indexIn(line) == -1)
        {
            DEF_READ_ASSERT(nLineNum == 1,
                (QString("Text information in file: ") + m_strFileName + " on line %1.").arg(nLineNum));
            continue;
        }

        QStringList xy_values = line.split(",");

        DEF_READ_ASSERT((xy_values.size() == 2 && m_DataPtr->w().empty()) || xy_values.size() == 3,
                        QString("Number of columns at line %1 is %2.").arg(nLineNum).arg(xy_values.size()));

        m_DataPtr->x().push_back(xy_values[0].toDouble());
        m_DataPtr->y().push_back(xy_values[1].toDouble());
        if(xy_values.size() == 3) m_DataPtr->w().push_back(xy_values[2].toDouble());
    }
}

data_exporter* data_export_factory::create_data_exporter(DATA_EXPORT_TYPE type, QVariant params)
{
    switch (type) {
    case ASCII_FILE: return new load_data_from_ascii_file(params);
    case CSV_FILE: return new LoadCsv(params);
    default: return Q_NULLPTR;
    }
}

#undef DEF_ASSERT_FILE_NAME
#undef DEF_READ_ASSERT
