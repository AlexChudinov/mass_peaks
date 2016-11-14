#ifndef APP_DATA_VIEW_H
#define APP_DATA_VIEW_H

#include "../graphics/zoom_plot.h"

class app_data_view : public zoom_plot_window
{
    Q_OBJECT

public:
    app_data_view(QWidget* parent = 0);
    ~app_data_view();

    /**
     * Set xy data to a view
     */
private:
};

#endif // APP_DATA_VIEW_H
