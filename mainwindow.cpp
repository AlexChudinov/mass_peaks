#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphics/zoom_plot.h"
#include "app_data_handler/app_data_handler.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    app_data_(new app_data_handler(this))
{
    //Set widgets
    ui->setupUi(this);
    ui->progressBar->hide();
    ui->statusBar->addWidget(ui->progressBar);

    zoom_plot_window* zoom_plot = new zoom_plot_window(this);
    this->setCentralWidget(zoom_plot);

    connect(ui->open_file_action, SIGNAL(triggered()), this, SLOT(open_file_action()));
    connect(this->app_data_, SIGNAL(started()), ui->progressBar, SLOT(show()));
    connect(this->app_data_, SIGNAL(finished()), ui->progressBar, SLOT(hide()));
    connect(this->app_data_, SIGNAL(progress_val(int)), ui->progressBar, SLOT(setValue(int)));
    connect(this->app_data_, SIGNAL(busy(bool)), ui->open_file_action, SLOT(setDisabled(bool)));
    connect(this->app_data_, SIGNAL(free(bool)), ui->open_file_action, SLOT(setEnabled(bool)));
    connect(this->app_data_, SIGNAL(warning(QString)), this, SLOT(show_message(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open_file_action()
{
    QString file_name = QFileDialog::getOpenFileName
            (
                this,
                "Open file dialog",
                QString(),
                "ASCII data files (*.txt *.dat);;All files (*.*)");

    if(!file_name.isEmpty()) app_data_->load_data(file_name);
}

void MainWindow::show_message(QString msg)
{
    QMessageBox::warning(this, "Mass specs programm message", msg);
}
