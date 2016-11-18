#ifndef SPLINE_H
#define SPLINE_H

#include <cstddef>
#include <vector>
#include <string>

using data_vector = std::vector<double>;
using String = std::string;

/**
 * Base for an every spline
 */
class spline_base
{
public:
    virtual ~spline_base(){}
    virtual size_t idx_of_interval(double x) const = 0;
    virtual double estimate_y_val(double x) const = 0;
    virtual data_vector estimate_y_vals(const data_vector& x) const = 0;
    /**
     * Checks that supplied x values are ok sorts them rearranging y vector accordingly
     */
    static void rearrange_xy_vals(data_vector& x, data_vector& y);
    /**
     * Check that rearranged x and y values are ok
     * Returns string with an error if it occured or an empty string
     */
    static String check_xy_vals(data_vector &x, data_vector &y);
};

/**
 * Cubic spline implementation
 */
class cubic_spline : public spline_base
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

    ~cubic_spline();

    /**
     * Returns number of breaks in the spline
     */
    size_t size() const { return x_.size(); }

    /**
     * Finds an interval to which x-value belongs to
     */
    size_t idx_of_interval(double x) const;

    /**
     * Finds y value that corresponds to a given x value
     */
    double estimate_y_val(double x) const;

    /**
     * Finds a vector of y values that corresponds to given x values
     */
    data_vector estimate_y_vals(const data_vector& x) const;
private:

};

#endif // SPLINE_H
