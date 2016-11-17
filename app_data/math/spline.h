#ifndef SPLINE_H
#define SPLINE_H

#include <cstddef>
#include <vector>

using data_vector = std::vector<double>;

/**
 * Cubic spline implementation
 */
class cubic_spline
{
    data_vector a_, b_, c_, d_; //cubic spline coefs S(x) = a + bx + cx^2 + dx^3;
    data_vector x_; //cubic spline breaks
    /**
     * Calculates spline coefs
     */
    void calculate_spline_(const data_vector& y, const data_vector& w);
public:
    /**
     * Creates cubic spline from an initial data
     */
    cubic_spline(const data_vector& x,
            const data_vector& y,
            double smooth_param = 0.0,
            const data_vector& w = data_vector());

    virtual ~cubic_spline();

    /**
     * Returns number of breaks in the spline
     */
    size_t size() const { return x_.size(); }
private:
    /**
     * Checks that supplied x values are ok sorts them rearranging y vector accordingly
     */
    static void rearrange_xy_vals_(data_vector& x, data_vector& y);
};

#endif // SPLINE_H
