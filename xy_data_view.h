#ifndef XY_DATA_VIEW_H
#define XY_DATA_VIEW_H

#include <QStandardItemModel>

/**
 * Creates table model for an xy data view
 */
class XyDataTableView : public QStandardItemModel
{
    Q_OBJECT

public:
    XyDataTableView(QObject *parent = 0);
    ~XyDataTableView();

    virtual void setXyData(const QVector<double>& x, const QVector<double>& y) = 0;
};

/**
 * Creates table model for peak and intensities view
 */
class MassPeaksTable : public XyDataTableView
{
    Q_OBJECT

public:
    MassPeaksTable(QObject* parent = 0);
    ~MassPeaksTable();

    void setXyData(const QVector<double>& x, const QVector<double>& y);
};

#endif // XY_DATA_VIEW_H
