#include "../app_data/app_data.h"

approximator* approximator_factory::create_approximator(APPROXIMATOR type,
                                                        const approximator_params *params,
                                                        const data_vector_type &x,
                                                        const data_vector_type &y)
{
    switch(type)
    {
    case CUBIC_SPLINE:
        return new cubic_spline_approximator(x,y,
                    static_cast<const cubic_spline_params*>(params)->smoothing_,
                    static_cast<const cubic_spline_params*>(params)->weightings_);
    default:
        return nullptr;
    }
}

void xy_data::set_approximator(APPROXIMATOR type, const approximator_params *params)
{
    approximator_.reset(approximator_factory::create_approximator(type, params, x_, y_));
}

cubic_spline_approximator::cubic_spline_approximator(const data_vector_type &x,
                                                     const data_vector_type &y,
                                                     double smooth_param,
                                                     const data_vector_type &w)
    :
      spline_(new spline(x,y,smooth_param,w))
{}

cubic_spline_approximator::~cubic_spline_approximator(){}

data_vector_type cubic_spline_approximator::approximate(const data_vector_type& x)
{
    return spline_->poly().estimate_y_vals(x);
}
