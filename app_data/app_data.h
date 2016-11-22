#ifndef APP_DATA_H
#define APP_DATA_H

#include <QVariant>
#include <vector>

using data_vector_type = std::vector<double>;

/**
 * XY data of application to work with
 */
class xy_data
{
    data_vector_type x_, y_, w_;
public:

    xy_data(){}

    xy_data(const data_vector_type& x, const data_vector_type& y, const data_vector_type& w)
        :
          x_(x), y_(y), w_(w)
    {}

    const data_vector_type& x() const { return x_; }
    const data_vector_type& y() const { return y_; }
    const data_vector_type& w() const { return w_;}
    data_vector_type& x() { return x_; }
    data_vector_type& y() { return y_; }
    data_vector_type& w() { return w_; }
};

#endif // APP_DATA_H
