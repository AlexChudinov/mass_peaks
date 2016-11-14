#ifndef APP_DATA_HANDLER_H
#define APP_DATA_HANDLER_H

#include <QThread>
#include <QMap>
#include <QSharedPointer>

#include "../app_data/app_data.h"

class data_exporter;

/**
 * Keeps application data and starts all data processes in a separate thread
 */
class app_data_handler : public QThread
{
    Q_OBJECT

public:
    app_data_handler(QObject* parent = 0);
    virtual ~app_data_handler();

    void run();

Q_SIGNALS:
    /**
     * Progress flow indicator
     */
    void progress_val(int val);

    /**
     * QAction setEnable(bool) support
     */
    void busy(bool is_busy = true);
    void free(bool is_free = true);

    /**
     * Emits some warnings from a separate thread
     */
    void warning(QString msg);

    /**
     * Emits new data
     */
    void data_changed(const xy_data*);

public Q_SLOTS:
    /**
     * Load data from a file
     */
    void load_data(QString file_name);

    /**
     * Get data from exporter
     */
    void get_data();
private:
    QSharedPointer<xy_data> xy_data_;
    QSharedPointer<data_exporter> data_exporter_;
};

#endif // APP_DATA_HANDLER_H
