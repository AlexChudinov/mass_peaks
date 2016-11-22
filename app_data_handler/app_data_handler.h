#ifndef APP_DATA_HANDLER_H
#define APP_DATA_HANDLER_H

#include <QThread>
#include <QMap>
#include <QSharedPointer>

class approximator;
class data_exporter;
class xy_data;
using vector_data_type = QVector<double>;

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

    const xy_data& data() const { return *this->xy_data_; }

    /**
     * Get approximated vals
     */
    QVector<double> get_approximated_vals(const QVector<double>& x);

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
    void data_changed(const vector_data_type& x, const vector_data_type& y);

    /**
     * Notifies that data is ready
     */
    void dataChanged();

public Q_SLOTS:
    /**
     * Load data from a file
     */
    void load_data(QString file_name);

    /**
     * Get data from exporter
     */
    void get_data();

    /**
     * Choose approximator type
     */
    void set_approximator(QString name, void *params);
    void set_approximator(QString name, double smoothing);

private:
    QSharedPointer<xy_data> xy_data_;
    QScopedPointer<data_exporter> data_exporter_;
    QScopedPointer<approximator> xy_data_approximator_;
};

#endif // APP_DATA_HANDLER_H
