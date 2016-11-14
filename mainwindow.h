#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class app_data_handler;

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

private:
    Ui::MainWindow *ui;
    app_data_handler* app_data_;

};

#endif // MAINWINDOW_H
