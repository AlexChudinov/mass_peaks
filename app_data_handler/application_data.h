#ifndef APP_DATA_H
#define APP_DATA_H

#include <QThread>
#include <QString>

#include <utility>
#include <vector>
#include <map>

/**
 * This application is supposed to work with a mass spectrum
 */
using mz_type            = double;
using intensity_type     = double;
using mass_spectrum      = std::pair<std::vector<mz_type>, std::vector<intensity_type>>;
using mass_spectrums_map = std::map<std::string, mass_spectrum>;

class application_data;

/**
 * Keeps application data and starts all data processes in a separate thread
 */
class application_data_handler : public QThread
{
    Q_OBJECT

public:
    /**
     * Processes that can be run in a separate thread
     */
    enum PROCESS_ID
    {
        LOAD_DATA     ///Load data from a file
    };

    application_data_handler(QObject* parent = 0);
    virtual ~application_data_handler(){}

    void run();

    /**
     * Sets the file name for data
     */
    void set_file_name(const QString& file_name)
    { file_name_ = file_name; }

    /**
     * Choose which process to run in a separate thread
     * Note, the processes description is in enum PROCESS_ID
     */
    void set_process(PROCESS_ID process_id);

signals:
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
     * New mass spectrum was appended
     */
    void mass_spectrum_appended(const mass_spectrum* ms);

public slots:
    /**
     * Loads data from a file
     */
    void load_data_file(const QString& filename);

private:
    QString file_name_;
    /**
     * Current reading/loading data process
     */
    void (application_data_handler::*process_)(const QString&);

    mass_spectrums_map mass_spectrums_;
};

#endif // APP_DATA_H
