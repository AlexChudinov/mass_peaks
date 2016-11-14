#ifndef APP_DATA_H
#define APP_DATA_H

#include <vector>

using data_vector_type = std::vector<double>;

/**
 * XY data of application to work with
 */
class xy_data
{
    data_vector_type x_, y_;
public:

    xy_data(){}

    xy_data(const data_vector_type& x, const data_vector_type& y)
        :
          x_(x), y_(y)
    {}

    const data_vector_type& x() const { return x_; }
    const data_vector_type& y() const { return y_; }
    data_vector_type& x() { return x_; }
    data_vector_type& y() { return y_; }
};

/**
 * Approximation of the xy data
 */
class approximation
{
public:

    /**
     * Returns approximated values for data array on a [xmin, xmax] interval
     */
    virtual data_vector_type approximate(double xmin, double xmax, int nsteps) = 0;
};



#endif // APP_DATA_H
