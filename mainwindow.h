#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Approximator;
class QCPRange;
class app_data_handler;
class zoom_plot_window;
using vector_data_type = QVector<double>;
class QComboBox;
class QDoubleSpinBox;
class QLabel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    Q_SLOT void open_file_action();
    Q_SLOT void show_message(QString msg);
    Q_SLOT void plot_data(const vector_data_type &x, const vector_data_type &y, bool keep_data_flag = false);
    Q_SLOT void plot_data(bool keep_data_flag = false);

    //Approximator management
    Q_SLOT void changeSmoothing(double smoothing);
    Q_SLOT void changeApproximator(QString name);

    /**
     * Shows approximatin curve
     */
    Q_SLOT void showApproxLine();

    /**
     * Shows peaks parameters
     */
    Q_SLOT void calculatePeaks();

private:
    Ui::MainWindow *ui;
    app_data_handler* app_data_;
    zoom_plot_window* app_data_view_;
    QScopedPointer<Approximator> m_pDataApproximator;

    QComboBox * m_comboChooseApproximator;
    QDoubleSpinBox * m_spinBoxSmoothVal;
    QLabel * m_labelShowStd;

    void connect_data_handler_();
    void create_data_view_();
    Q_SLOT void initApproximator();

    void calculateCurrentStd();
    Q_SIGNAL void splineStdChanged(QString msg);
    Q_SIGNAL void approximatorChanged();
};

#endif // MAINWINDOW_H
