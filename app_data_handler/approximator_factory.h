#ifndef APPROXIMATOR_FACTORY_H
#define APPROXIMATOR_FACTORY_H

#include "../app_data/math/spline.h"
#include "/new_math/peacewisepoly.h"

using data_vector_type = std::vector<double>;

/**
 * Creates sutable approximator for a given data
 */
class approximator;
struct approximator_params;
class cubic_spline_approximator;

/**
 * Approximator type to choose
 */
enum APPROXIMATOR
{
    CUBIC_SPLINE = 0x00, ///cubic spline
    APPROXIMATOR_UNKNOWN = 0xFF ///unknown type of an approximator
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

    /**
     * Calculates all peak positions in a spectrum
     */
    virtual data_vector_type getPeaks() = 0;

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

    data_vector_type getPeaks();
};

class NewApproximator : public approximator
{
    std::un
};

#endif // APPROXIMATOR_FACTORY_H
