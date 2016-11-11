#ifndef ZOOM_PLOT_H
#define ZOOM_PLOT_H

#include "qcustomplot/qcustomplot.h"
#include <utility>
#include <vector>

/**
 * Plotting of (x, y ) data with a horizontal and a vertical zoom
 */
using xy_data = std::pair<std::vector<double>, std::vector<double>>;

class zoom_plot : public QCustomPlot
{
    Q_OBJECT

    ///Current plot procedure identificator
    enum CURRENT_PLOT_ACTION
    {
        HZOOM_IN  = 0x00, ///Horizontal zoom is switched on
        VZOOM_IN  = 0x01, ///Vertical zoom is switched on
        ZOOM_OUT  = 0x02, ///Zoom out is switched on
        NO_ACTION = 0xFF  ///No any actions
    };

public:
    zoom_plot(QWidget* parent = 0);
    ~zoom_plot();

    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent* event);

signals:
    /**
     * Change toolbar buttons states
     */
    void vzoom_changed(bool);
    void hzoom_changed(bool);
    void zoom_out_changed(bool);

private slots:
    /**
     * Changes zoom state to an oposite equals to a toggle in QAction
     */
    void set_hzoom(bool hzoom);
    void set_vzoom(bool vzoom);
    void set_zoom_out(bool zoom_out);

    /**
     * Sets current plot action mode
     */
    void set_plot_action(CURRENT_PLOT_ACTION action);

private:
    /**
     * Sets the selection area view on a screen
     */
    void set_hzoom_selection_area_(const QPoint& pos);
    void set_vzoom_selection_area_(const QPoint& pos);

    /**
     * Zooming out
     */
    void zoom_out_();

    /**
     * Current plot action state
     */
    CURRENT_PLOT_ACTION plot_action_;
    /**
     * Position of a first mouse press
     */
    QPoint mouse_press_position_;
    QRubberBand* selection_area_;
};

/**
 * Creates the main plot window with a toolbar
 */
class zoom_plot_window : public QWidget
{
    Q_OBJECT
public:
     zoom_plot_window(QWidget* parent = 0);
     ~zoom_plot_window();

signals:
     void add_graph(const xy_data* xy);

private:
};

#endif // ZOOM_PLOT_H
