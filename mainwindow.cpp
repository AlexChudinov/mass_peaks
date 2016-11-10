#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphics/zoom_plot.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    zoom_plot_window* zoom_plot = new zoom_plot_window(this);
    this->setCentralWidget(zoom_plot);
}

MainWindow::~MainWindow()
{
    delete ui;
}
