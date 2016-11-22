#ifndef DATA_EXPORT_H
#define DATA_EXPORT_H

#include "app_data.h"

#include <QRunnable>
#include <QVariant>
#include <QSharedPointer>

enum DATA_EXPORT_TYPE
{
    ASCII_FILE = 0x00,
    DATA_EXPORT_UNKNOWN = 0xFF
};

class data_exporter : public QObject, public QRunnable
{
    Q_OBJECT
public:
    virtual QSharedPointer<xy_data> data_ptr() = 0;

    Q_SIGNAL void error(QString error_msg);
    Q_SIGNAL void progress_val(int val);
};

/**
 * Loads delimeted cols and rows from a text file. Where '\t' is a col delimiter and '\n' is a row delimiter
 */
class load_data_from_ascii_file : public data_exporter
{
    QString file_name_; //file for loading
    QSharedPointer<xy_data> data_ptr_;     //pointer where loaded data will be stored

public:
    load_data_from_ascii_file(QVariant params);
    ~load_data_from_ascii_file(){}

    /**
     * Get loaded data
     */
    QSharedPointer<xy_data> data_ptr() { return data_ptr_; }

    /**
     * Runs file loading process
     */
    void run();
};

/**
 * Data export factory
 */
class data_export_factory
{
public:
    static data_exporter* create_data_exporter(DATA_EXPORT_TYPE type, QVariant params);
};


#endif // DATA_EXPORT_H
