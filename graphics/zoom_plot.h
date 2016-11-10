#ifndef ZOOM_PLOT_H
#define ZOOM_PLOT_H

#include <QWidget>

/**
 * Plotting of (x, y ) data with a horizontal and a vertical zoom
 */

class zoom_plot : public QWidget
{
    Q_OBJECT

public:
    zoom_plot(QWidget* parent = 0);
    ~zoom_plot();

signals:
    void hzoom_changed(bool);
    void vzoom_changed(bool);

public slots:

    void set_hzoom(bool hzoom);
    void set_vzoom(bool vzoom);
    void set_hzoom_cursor();

private:
    /**
     * Flags that horizontal or vertical zoom were switched on
     */
    bool hzoom_, vzoom_;
};

#endif // ZOOM_PLOT_H
