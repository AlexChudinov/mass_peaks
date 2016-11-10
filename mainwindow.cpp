#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphics/zoom_plot.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    zoom_plot* zoom_plot_ = new zoom_plot(this);
    this->setCentralWidget(zoom_plot_);
}

MainWindow::~MainWindow()
{
    delete ui;
}
