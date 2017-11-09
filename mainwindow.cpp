#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphics/zoom_plot.h"
#include "app_data/app_data.h"
#include "app_data_handler/app_data_handler.h"
#include "app_data_handler/approximator_factory.h"
#include "xy_data_view.h"

#include <QFileDialog>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    app_data_(new app_data_handler(this)),
    app_data_view_(new zoom_plot_window(this))
{
    approximatorParams_.smoothing_ = 0.0;
    approximatorParams_.name_ = "Cubic spline";
    approximatorParams_.nsteps = 1000;

    //Set standard widgets
    ui->setupUi(this);
    ui->progressBar->hide();
    ui->statusBar->addWidget(ui->progressBar);
    ui->xyTableContents->setLayout(ui->xyTableLayout);
    ui->xyTableLayout->addWidget(ui->tableView);

    this->connect_data_handler_();
    this->create_data_view_();

    //Set plot fonts
    app_data_view_->plot_area()->xAxis->setTickLabelFont(QFont("Times", 14));
    app_data_view_->plot_area()->yAxis->setTickLabelFont(QFont("Times", 14));

    QAction* peaks_table_toggle_action = ui->xyTable->toggleViewAction();
    peaks_table_toggle_action->setIcon(QIcon(":/Icons/table_icon"));
    ui->mainToolBar->addAction(peaks_table_toggle_action);
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
                "ASCII data files (*.txt *.dat);; "
                "CSV data files (*.csv);;All files (*.*)");

    if(!file_name.isEmpty()) app_data_->load_data(file_name);
    connect(app_data_, SIGNAL(finished()), this, SLOT(init_approximator_handles_()));
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

void MainWindow::changeSmoothing(double smoothing)
{
    approximatorParams_.smoothing_ = smoothing;
    cubic_spline_params params;
    params.smoothing_ = smoothing;
    app_data_->set_approximator(approximatorParams_.name_, reinterpret_cast<void*>(&params));
    this->calculate_std_();
}

void MainWindow::changeApproximator(QString name)
{
    approximatorParams_.name_ = name;
    cubic_spline_params params;
    params.smoothing_ = approximatorParams_.smoothing_;
    app_data_->set_approximator(name, reinterpret_cast<void*>(&params));
    this->calculate_std_();
}

void MainWindow::showApproxLine()
{
    QCPRange range = app_data_view_->plot_area()->xAxis->range();
    double xmin = range.lower, xmax = range.upper, dx = (xmax-xmin) / approximatorParams_.nsteps;
    QVector<double> x(approximatorParams_.nsteps), y;
    std::for_each(x.begin(), x.end(), [&xmin, dx](double& xval){ xval = (xmin+=dx);});
    y = app_data_->get_approximated_vals(x);
    QCPGraph* g;
    if(app_data_view_->plot_area()->graphCount() == 2)
        g = app_data_view_->plot_area()->graph();
    else
        g = app_data_view_->plot_area()->addGraph();
    g->setData(x,y,true);
    g->setPen(QPen(Qt::red));
    app_data_view_->plot_area()->replot();
}

void MainWindow::calculatePeaks()
{
    QVector<double> peak_positions   = this->app_data_->getPeakPositions();
    QVector<double> peak_intensities = this->app_data_->get_approximated_vals(peak_positions);

    XyDataTableView* peaksTable = new MassPeaksTable(this);

    peaksTable->setXyData(peak_positions, peak_intensities);

    this->ui->tableView->setModel(peaksTable);
    this->ui->tableView->update();
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
    connect(this->app_data_, SIGNAL(approximatorChanged()), this, SLOT(calculatePeaks()));
}

void MainWindow::init_approximator_handles_()
{
    disconnect(app_data_, SIGNAL(finished()), this, SLOT(init_approximator_handles_()));

    if(!app_data_->hasApproximator())
    {
        //Create approximator chooser
        QComboBox* set_approxmtr_ = new QComboBox(this);
        set_approxmtr_->addItem({"Cubic spline"});
        ui->mainToolBar->addWidget(set_approxmtr_);
        connect(set_approxmtr_, SIGNAL(activated(QString)),
                this, SLOT(changeApproximator(QString)));

        //Show spline parameters
        QDoubleSpinBox* set_smoothing_ = new QDoubleSpinBox(this);
        set_smoothing_->setRange(1.0E-10, 1.0E10);
        set_smoothing_->setDecimals(10);
        set_smoothing_->setLocale(QLocale::English);
        ui->mainToolBar->addWidget(set_smoothing_);
        connect(set_smoothing_, SIGNAL(valueChanged(double)),
                this, SLOT(changeSmoothing(double)));

        //Spline standars deviation from an experimental data
        QLabel* show_std_ = new QLabel(" std = 0.0", this);
        ui->mainToolBar->addWidget(show_std_);
        connect(this, SIGNAL(stdChanged_(QString)), show_std_, SLOT(setText(QString)));

        connect(app_data_, SIGNAL(approximatorChanged()), this, SLOT(showApproxLine()));
        connect(app_data_view_->plot_area(), SIGNAL(xrangeNotify()),
                this, SLOT(showApproxLine()));
    }

    app_data_->set_approximator(approximatorParams_.name_,approximatorParams_.smoothing_);
}

void MainWindow::calculate_std_()
{
    double yy = 0.0, yy2 = 0.0, norm = 0.0;
    QVector<double> y = QVector<double>::fromStdVector(app_data_->data().y());
    QVector<double> x = QVector<double>::fromStdVector(app_data_->data().x());
    QVector<double> w = QVector<double>::fromStdVector(app_data_->data().w());
    QVector<double> yt = app_data_->get_approximated_vals(x);
    if(w.empty())
    {
        QVector<double>::const_iterator it1 = y.constBegin(), it2 = yt.constBegin();
        for(; it1 != y.constEnd(); ++it1, ++it2)
        {
            yy += (*it1 - *it2); yy2 += (*it1 - *it2) * (*it1 - *it2);
        }
        norm = double(y.size());
    }
    else
    {
        QVector<double>::const_iterator it1 = y.constBegin(), it2 = yt.constBegin(), it3 = w.constBegin();
        for(; it1 != y.constEnd(); ++it1, ++it2, ++it3)
        {
            yy += *it3 * (*it1 - *it2); yy2 += *it3 * (*it1 - *it2) * (*it1 - *it2);
            norm += *it3;
        }
    }
    Q_EMIT stdChanged_(QString(" std = %1").arg((yy2 - yy*yy)/norm));
}
