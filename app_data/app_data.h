#ifndef APP_DATA_H
#define APP_DATA_H

#include <QVariant>
#include <vector>
#include "../app_data/math/spline.h"

using data_vector_type = std::vector<double>;

class approximator;
struct approximator_params;
class cubic_spline_approximator;

/**
 * Approximator type to choose
 */
enum APPROXIMATOR
{
    CUBIC_SPLINE = 0x00, ///cubic spline
    APPROXIMATOR_UNKNOWN = 0xFF
};
class approximator_factory
{
public:
    static approximator* create_approximator(APPROXIMATOR type,
                                             const approximator_params* params,
                                             const data_vector_type& x,
                                             const data_vector_type& y);
};

/**
 * XY data of application to work with
 */
class xy_data
{
    std::unique_ptr<approximator> approximator_;
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
    void set_approximator(APPROXIMATOR type, const approximator_params* params);
};

/**
 * Parameters of an approximator
 */
struct approximator_params{};
struct cubic_spline_params : public approximator_params
{
    double smoothing_;
    data_vector_type weightings_;
};

/**
 * Approximation of the xy data
 */
class approximator
{
public:

    /**
     * Returns approximated values for data array for given x values
     */
    virtual data_vector_type approximate(const data_vector_type& x) = 0;

    virtual ~approximator(){}
};

/**
 * Regular cubic spline data approximation
 */
class cubic_spline_approximator : public approximator
{
    using spline = cubic_spline<double>;
    std::unique_ptr<spline> spline_;
public:
    cubic_spline_approximator(const data_vector_type& x,
                              const data_vector_type& y,
                              double smooth_param,
                              const data_vector_type& w);
    ~cubic_spline_approximator();

    data_vector_type approximate(const data_vector_type &x);
};

#endif // APP_DATA_H
