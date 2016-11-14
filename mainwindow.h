#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class app_data_handler;
class zoom_plot_window;
using vector_data_type = QVector<double>;

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
private:
    Ui::MainWindow *ui;
    app_data_handler* app_data_;
    zoom_plot_window* app_data_view_;

    void connect_data_handler_();
    void create_data_view_();
};

#endif // MAINWINDOW_H
