#include "spline.h"
#include "solvers.h"

#include <cassert>
#include <algorithm>

void spline_base::rearrange_xy_vals(data_vector &x, data_vector &y)
{
    size_t N = x.size();
    std::vector<size_t> idxs(N);
    std::vector<bool> visited(N, false);
    //create vector of indices
    size_t idx = 0;
    std::for_each(idxs.begin(), idxs.end(), [&idx](size_t& val){ val = (idx++); });
    //sort vector of indices using comparisom between x values
    std::sort(idxs.begin(), idxs.end(), [x](size_t i, size_t j)->bool{ return x[i] < x[j]; });
    //rearrange elements in x and y vectors
    std::vector<bool>::const_iterator it = visited.cbegin();
    while((it = std::find(it, visited.cend(), false)) != visited.cend())
    {
        size_t i = std::distance(visited.cbegin(), it);
        while(!visited[i])
        {
            if(i == idxs[i] || visited[idxs[i]]) visited[i] = true;
            else
            {
                std::swap(x[i], x[idxs[i]]);
                std::swap(y[i], y[idxs[i]]);
                visited[i] = true;
                i = idxs[i];
            }
        }
    };
}

String spline_base::check_xy_vals(data_vector &x, data_vector &y)
{
    String err;

    rearrange_xy_vals(x,y);

    if(std::isnan(x[0]) || std::isnan(y[0])) return String("Nan values in the input arrays!");

    for(size_t i = 1; i < x.size(); ++i)
    {
        if(x[i-1] == x[i]) return String("Not unique x values.");
        if(std::isnan(x[i]) || std::isnan(y[i])) return String("Nan values in the input arrays!");
    }

    return err;
}

void cubic_spline::calculate_spline_(const data_vector& y, const data_vector &w)
{
    size_t N = x_.size();
    data_vector h(N - 1); //differences
    //Set differences
    for(size_t i = 0; i < h.size(); ++i) h[i] = x_[i+1] - x_[i];

    data_vector a(N), g(N), b(N-1), c(N-2);

    //boundaries:
    a[0] = a[N-1] = 1.0 / 6.0;
    b[0] = c[0] = c[N-3] = b[N-2] = g[0] = g[N-1] = 0.0;
    //********************

    for(size_t i = 1; i < N-1; ++i)
    {
        a[i] = 1./3. * (h[i-1] + h[i]) + 1./h[i-1]/h[i-1] * w[i-1]
                + (1./h[i-1] + 1./h[i])*(1./h[i-1] + 1./h[i]) * w[i]
                + 1./h[i]/h[i] * w[i+1];
        g[i] = (y[i+1] - y[i]) / h[i] - (y[i] - y[i-1]) / h[i-1];
    }
    for(size_t i = 1; i < N-2; ++i)
        b[i] = 1./6. * h[i] - 1./h[i] * ((1./h[i-1] + 1./h[i])*w[i] + (1./h[i+1] + 1./h[i])*w[i+1]);
    for(size_t i = 1; i < N-3; ++i)
        c[i] = 1./h[i]/h[i+1] * w[i+1];

    data_vector cl = c, bl = b;//duplicate values for a symmetric matrix
    math::fivediagonalsolve(N, cl.data(), bl.data(), a.data(), b.data(), c.data(), g.data(), c_.data());

    a_[0]  = y[0]  - (c_[1] - c_[0]) / h[0] * w[0];
    a_[N-1]= y[N-1]+ (c_[N-1] - c[N-2]) / h[N-2] * w[N-1];
    d_[0] = (c_[1] - c_[0]) / h[0];
    d_[N-1]=(c_[0] - c_[N-1])/h[0];
    b_[0] = (y[1] - y[0]) / h[0] - c_[0] / 2. * h[0] - d_[0] / 6. * h[0];
    b_[N-1]= (y[0] - y[N-1]) / h[0] - c_[N-1] / 2. * h[0] - d_[N-1] / 6. * h[0];
    for(size_t i = 1; i < N-1; ++i)
    {
        a_[i] = y[i] - w[i] * ((c_[i+1] - c_[i]) / h[i] - (c_[i] - c_[i-1]) / h[i-1]);
        d_[i] = (c_[i+1] - c_[i]) / h[i];
        b_[i] = (y[i+1] - y[i]) / h[i] - c_[i] / 2. * h[i] - d_[i] / 6. * h[i];
    }
}

cubic_spline::cubic_spline(
        const data_vector &x,
        const data_vector &y,
        double smooth_param,
        const data_vector &w
        )
    :
      a_(x.size()), b_(x.size()), c_(x.size()), d_(x.size()), x_(x)
{
    //Error checking of data should be done before
    data_vector w_;
    if(w.empty()) w_.assign(x.size(), 1.0);

    std::for_each(w_.begin(), w_.end(), [smooth_param](double& val){ val *= smooth_param; });

    calculate_spline_(y, w_);
}

cubic_spline::~cubic_spline(){}

size_t cubic_spline::idx_of_interval(double x) const
{
    size_t
            start = 0,
            end = x_.size() - 1;

    if(x < x_[start]) return start;
    if(x >= x_[end]) return end;
    while(end - start != 1)
    {
        size_t m = (end + start) / 2;
        if(x >= x_[m]) start = m;
        else end = m;
    }
    return start;
}

double cubic_spline::estimate_y_val(double x) const
{
    size_t idx = idx_of_interval(x);
    double h = x - x_[idx];
    return ((c_[idx] / 2. + d_[idx] / 6. * h) * h + b_[idx]) * h + a_[idx];
}

data_vector cubic_spline::estimate_y_vals(const data_vector &x) const
{
    data_vector y(x.size());
    size_t idx = 0;
    std::for_each(y.begin(), y.end(),
                  [this, x, &idx](double& y_val){ y_val = this->estimate_y_val(x[idx++]); });
    return y;
}
