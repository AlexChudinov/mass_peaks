#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphics/zoom_plot.h"
#include "app_data_handler/app_data_handler.h"
#include "app_data/math/spline.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    app_data_(new app_data_handler(this)),
    app_data_view_(new zoom_plot_window(this))
{
    //Set standard widgets
    ui->setupUi(this);
    ui->progressBar->hide();
    ui->statusBar->addWidget(ui->progressBar);

    //Test:
    using xy_data = std::map<double, double>;
    xy_data xy;
    xy[1.1] = 2; xy[2.4] = 3; xy[3] = 4;
    xy[4] = 15; xy[5.3] = 6; xy[6] = 7;
    cubic_spline<double> spline(xy, 0.1);
    std::vector<double> x(100);
    size_t idx = 0;
    for(double& x_val : x) x_val = 1.0 + 0.05 * (idx++);
    std::vector<double> y = spline.poly().estimate_y_vals(x);
    //*****

    this->connect_data_handler_();
    this->create_data_view_();

    app_data_view_->plot_area()->addGraph()->addData({1.1,2.4,3,4,5.3,6},{2,3,4,15,6,7});
    QCPGraph* g = app_data_view_->plot_area()->addGraph();
    g->addData(QVector<double>::fromStdVector(x), QVector<double>::fromStdVector(y));
    g->setPen(QPen(Qt::red));
    app_data_view_->plot_area()->xAxis->setTickLabelFont(QFont("Times",20));
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

void MainWindow::plot_data(const vector_data_type& x, const vector_data_type& y, bool keep_data_flag)
{
    if(!keep_data_flag) app_data_view_->plot_area()->clearGraphs();
    app_data_view_->plot_area()->addGraph()->addData(x,y);
    app_data_view_->plot_area()->rescaleAxes();
    app_data_view_->plot_area()->replot();
}

void MainWindow::plot_data(bool keep_data_flag)
{
    vector_data_type
            x = vector_data_type::fromStdVector(app_data_->data().x()),
            y = vector_data_type::fromStdVector(app_data_->data().y());
    this->plot_data(x,y,keep_data_flag);
}

void MainWindow::connect_data_handler_()
{
    connect(ui->open_file_action, SIGNAL(triggered()), this, SLOT(open_file_action()));
    connect(this->app_data_, SIGNAL(started()), ui->progressBar, SLOT(show()));
    connect(this->app_data_, SIGNAL(finished()), ui->progressBar, SLOT(hide()));
    connect(this->app_data_, SIGNAL(progress_val(int)), ui->progressBar, SLOT(setValue(int)));
    connect(this->app_data_, SIGNAL(busy(bool)), ui->open_file_action, SLOT(setDisabled(bool)));
    connect(this->app_data_, SIGNAL(free(bool)), ui->open_file_action, SLOT(setEnabled(bool)));
    connect(this->app_data_, SIGNAL(warning(QString)), this, SLOT(show_message(QString)));
}

void MainWindow::create_data_view_()
{
    this->setCentralWidget(this->app_data_view_);
    connect(this->app_data_, SIGNAL(data_changed(vector_data_type,vector_data_type)),
            this, SLOT(plot_data(vector_data_type,vector_data_type)));
}
