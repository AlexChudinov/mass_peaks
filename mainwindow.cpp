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
    connect(app_data_, SIGNAL(finished()), this, SLOT(initApproximator()));
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
    changeApproximator(m_comboChooseApproximator->currentText());
}

void MainWindow::changeApproximator(QString name)
{
    double fSmooth = m_spinBoxSmoothVal->text().toDouble();
    if (name == "Cubic spline")
    {
        CubicSplineApproximator::CubicSplineParams
                params(app_data_->data().x(), app_data_->data().y(), fSmooth);
        m_pDataApproximator.reset(Approximator::create(Approximator::CubicSplineType, params));
    }
    else if (name == "Cubic spline (new)")
    {
        CubicSplineApproximator::CubicSplineParams
                params(app_data_->data().x(), app_data_->data().y(), fSmooth);
        m_pDataApproximator.reset(Approximator::create(Approximator::CubicSplineNewType, params));
    }
    else if (name == "Cubic spline with equal steps")
    {
        CubicSplineApproximator::CubicSplineParams
                params(app_data_->data().x(), app_data_->data().y(), fSmooth);
        m_pDataApproximator.reset
        (
            Approximator::create(Approximator::CubicSplineEqualStepSizeType, params)
        );
    }
    calculateCurrentStd();
    Q_EMIT approximatorChanged();
}

void MainWindow::showApproxLine()
{
    QCPRange range = app_data_view_->plot_area()->xAxis->range();
    double xmin = range.lower, xmax = range.upper, dx = (xmax-xmin) / 100;
    QVector<double> x(100), y;
    std::for_each(x.begin(), x.end(), [&xmin, dx](double& xval){ xval = (xmin+=dx);});
    y = QVector<double>::fromStdVector(m_pDataApproximator->approximate(x.toStdVector()));
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
    if(!this->app_data_->data().x().empty())
    {
        QVector<double> peak_positions   =
                QVector<double>::fromStdVector(m_pDataApproximator->getPeaks());
        QVector<double> peak_intensities =
                QVector<double>::fromStdVector(m_pDataApproximator->approximate(peak_positions.toStdVector()));

        XyDataTableView* peaksTable = new MassPeaksTable(this);

        peaksTable->setXyData(peak_positions, peak_intensities);

        this->ui->tableView->setModel(peaksTable);
        this->ui->tableView->update();
    }
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
    connect(ui->actionPeaks, SIGNAL(triggered()), this, SLOT(calculatePeaks()));
}

void MainWindow::initApproximator()
{
    disconnect(app_data_, SIGNAL(finished()), this, SLOT(initApproximator()));

    if(!m_pDataApproximator)
    {
        //Create approximator chooser
        m_comboChooseApproximator = new QComboBox(this);
        m_comboChooseApproximator->addItem({"Cubic spline"});
        m_comboChooseApproximator->addItem({"Cubic spline (new)"});
        m_comboChooseApproximator->addItem({"Cubic spline with equal steps"});
        ui->mainToolBar->addWidget(m_comboChooseApproximator);
        connect(m_comboChooseApproximator, SIGNAL(activated(QString)),
                this, SLOT(changeApproximator(QString)));

        //Show spline parameters
        m_spinBoxSmoothVal = new QDoubleSpinBox(this);
        m_spinBoxSmoothVal->setRange(1.0E-10, 1.0E10);
        m_spinBoxSmoothVal->setDecimals(10);
        m_spinBoxSmoothVal->setLocale(QLocale::English);
        ui->mainToolBar->addWidget(m_spinBoxSmoothVal);
        connect(m_spinBoxSmoothVal, SIGNAL(valueChanged(double)),
                this, SLOT(changeSmoothing(double)));

        //Spline standars deviation from an experimental data
        m_labelShowStd = new QLabel(" std = 0.0", this);
        ui->mainToolBar->addWidget(m_labelShowStd);
        connect(this, SIGNAL(splineStdChanged(QString)),
                m_labelShowStd, SLOT(setText(QString)));

        connect(this, SIGNAL(approximatorChanged()),
                this, SLOT(showApproxLine()));
        connect(app_data_view_->plot_area(), SIGNAL(xrangeNotify()),
                this, SLOT(showApproxLine()));

        changeApproximator("Cubic spline");
        showApproxLine();
    }
}

void MainWindow::calculateCurrentStd()
{
    double yy = 0.0, yy2 = 0.0, norm = 0.0;
    QVector<double> y = QVector<double>::fromStdVector(app_data_->data().y());
    QVector<double> x = QVector<double>::fromStdVector(app_data_->data().x());
    QVector<double> w = QVector<double>::fromStdVector(app_data_->data().w());
    QVector<double> yt =
            QVector<double>::fromStdVector(m_pDataApproximator->approximate(x.toStdVector()));
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
    Q_EMIT splineStdChanged(QString(" std = %1").arg((yy2 - yy*yy)/norm));
}
