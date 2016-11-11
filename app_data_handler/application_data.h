#ifndef APP_DATA_H
#define APP_DATA_H

#include <QThread>
#include <QString>

#include <utility>
#include <vector>
#include <map>

using x_type      = double;
using y_type      = double;
using xy_data     = std::pair<std::vector<x_type>, std::vector<y_type>>;
using xy_data_map = std::map<std::string, xy_data>;

/**
 * This is an interface to an xy-data
 */
class application_data
{
    xy_data_map xy_data_map_;

public:
    application_data(){}
    virtual ~application_data(){}

    /**
     * Adds xy-data to a data map
     */
    const xy_data& add_new_xy_data(const std::string& name = QString(), const xy_data& d);
    /**
     * Gets an xy-data from a data map
     */
    const xy_data& get_xy_data(const std::string& name) const;
};

/**
 * Keeps application data and starts all data processes in a separate thread
 */
class application_data_handler : public QThread
{
    Q_OBJECT

public:
    application_data_handler(QObject* parent = 0);
    virtual ~application_data_handler(){}

    void run();

    /**
     * Sets the name for the current data under process
     */
    void set_active_data(const QString& file_name);

    /**
     * Choose which process to run in a separate thread
     * Note, the processes description is in enum PROCESS_ID
     */
    void set_process(PROCESS_ID process_id);

Q_SIGNALS:
    /**
     * Progress flow indicator
     */
    void progress_value(int val);

    /**
     * QAction setEnable(bool) support
     */
    void application_data_holder_busy(bool is_busy = true);
    void application_data_holder_free(bool is_free = true);

    /**
     * Emits some warnings from a separate thread
     */
    void warning(QString msg);

    /**
     * New xy data array was appended
     */
    void xy_data_appended(const xy_data* ms);

private:
    QString name_;
    /**
     * Current reading/loading data process
     */
    void(*process_)(xy_data*);

    application_data mass_spectrums_;
};

#endif // APP_DATA_H
