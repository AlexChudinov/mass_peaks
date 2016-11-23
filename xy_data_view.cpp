#include "xy_data_view.h"

XyDataTableView::XyDataTableView(QObject *parent)
    :
      QStandardItemModel(parent)
{

}

XyDataTableView::~XyDataTableView()
{

}

MassPeaksTable::MassPeaksTable(QObject *parent)
    :
      XyDataTableView(parent)
{

}

MassPeaksTable::~MassPeaksTable()
{

}

void MassPeaksTable::setXyData(const QVector<double> &x, const QVector<double> &y)
{
    this->clear();

    this->setColumnCount(2);
    this->setRowCount(std::min(x.size(), y.size()));

    this->setHorizontalHeaderItem(0, new QStandardItem("Peak position"));
    this->setHorizontalHeaderItem(1, new QStandardItem("Peak intensity"));

    for(int i = 0; i < this->rowCount(); ++i)
    {
        this->setItem(i, 0, new QStandardItem(QString("%1").arg(x[i])));
        this->setItem(i, 1, new QStandardItem(QString("%1").arg(y[i])));
    }
}
