#ifndef SPLINE_H
#define SPLINE_H

#include <memory>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

#include "solvers.h"
#include "array_operations.h"

/**
 * Peacewise polynomial
 */
template<size_t n, typename Float = double>
class peacewise_poly
{
public:
    using poly_coef_type  = std::array<Float, n+1>;
    using poly_coefs_type = std::map<Float, poly_coef_type>;

private:
    poly_coefs_type poly_coefs_;

public:
    /**
     * Preallocates poly coefficients in memory
     */
    peacewise_poly(size_t N = 0){ poly_coefs_.get_allocator().allocate(N); }
    virtual ~peacewise_poly(){}

    /**
     * Returns the order of polynomial
     */
    constexpr size_t order() const { return n; }

    /**
     * Sets the polynomial coefficients
     */
    poly_coef_type& operator[](Float xval) { return poly_coefs_[xval]; }

    /**
     * Estimates y-value that corresponds to a given x-value
     */
    Float estimate_y_val(const Float& xval) const
    {
        auto xyval = poly_coefs_.lower_bound(xval);
        if(xyval != poly_coefs_.begin()) --xyval;
        const poly_coef_type& coefs = xyval->second;
        Float dx = xval - xyval->first;
        Float res = coefs[0];

        math::For<1, n+1, true>::Do([&res, dx, coefs](size_t idx)
        {
            (res *= dx) += coefs[idx];
        });

        return res;
    }

    /**
     * Estimates a vector of y values that correspond to a vector of x values
     */
    std::vector<Float> estimate_y_vals(const std::vector<Float>& x) const
    {
        std::vector<Float> y(x.size());
        std::transform(x.cbegin(), x.cend(),
                       y.begin(), [this](Float xval)->Float
        {
            return this->estimate_y_val(xval);
        });
        return y;
    }

    /**
     * Differentiate poly
     */
    peacewise_poly<n-1, Float> diff() const
    {
        peacewise_poly<n-1, Float> res(this->poly_coefs_.size());

        for(const auto& coef : this->poly_coefs_)
        {
            auto& diff_coefs = res[coef.first];
            math::For<0, n, true>::Do([&diff_coefs, coef](size_t j)
            {
                diff_coefs[j] = (n - j) * coef.second[j];
            });
        }

        return res;
    }

    /**
     * Get right hand side polynomial zero
     */
    Float rhzero(const Float& x0) const
    {
        Float y0 = this->estimate_y_val(x0);
        auto itxy = poly_coefs_.lower_bound(x0);
        if(itxy == poly_coefs_.end())
            return x0;
        Float y1 = itxy->second[n];
        while(y0 * y1 >= 0.0)
        {
            y0 = y1;
            if(++itxy == poly_coefs_.end()) return x0;
            y1 = itxy->second[n];
        }
        Float x11 = itxy->first;
        Float x00 = (--itxy)->first;
        auto fun = [this](Float x)->Float
        {
            return this->estimate_y_val(x);
        };

        return math::fZero(fun, x00, x11, fabs(y1 - y0)*1e-10);
    }

    /**
     * Get all polynomial zeros between begin()->first and end()->first
     */
    std::vector<Float> get_zeros() const
    {
        std::vector<Float> zs;
        Float x0 = poly_coefs_.begin()->first;
        Float x1 = this->rhzero(x0);
        while(x1 != x0)
        {
            zs.push_back(x1);
            x0 = poly_coefs_.lower_bound(x1)->first;
            x1 = this->rhzero(x0);
        }
        return zs;
    }

    /**
     * Get all maximums
     */
    std::vector<Float> get_maxs() const
    {
        if(order() < 2) return std::vector<Float>(); //No maximums for this order

        peacewise_poly<n - 1, Float> diff = this->diff();
        std::vector<Float> ps = diff.get_zeros();

        auto pred = [this](Float xval)
        {
            auto it1 = this->poly_coefs_.lower_bound(xval), it2 = it1--;
            return !(it1->second[n-1] > 0.0 && it2->second[n-1] < 0.0);
        };

        typename std::vector<Float>::iterator end =
                std::remove_if(ps.begin(), ps.end(), pred);

        ps.assign(ps.begin(), end);

        return ps;
    }
};

/**
 * Cubic spline implementation
 */
template<typename Float = double>
class cubic_spline
{
    using xy_values_type = std::map<Float, Float>;
    using data_vector_type = std::vector<Float>;
    using diff_poly = peacewise_poly<2, Float>;
    using Poly = peacewise_poly<3, Float>;

    std::unique_ptr<Poly> poly_;

    /**
     * Calculates spline coefs
     */
    void calculate_spline_(const xy_values_type& xy_vals, const data_vector_type& w)
    {
        size_t N = xy_vals.size();
        data_vector_type x(N), y(N);
        auto it = xy_vals.cbegin();
        for(size_t i = 0; it != xy_vals.cend(); ++it, ++i)
        {
            x[i] = it->first;
            y[i] = it->second;
        }
        calculate_spline_(x,y,w);
    }

    /**
     * Calculates spline using two vectors instead of map
     */
    void calculate_spline_(const data_vector_type& x, const data_vector_type& y, const data_vector_type& w)
    {
        size_t N = std::min(x.size(), y.size());
        std::unique_ptr<Float[]>
                a(new Float[N]),
                b(new Float[N]),
                c(new Float[N]),
                d(new Float[N]);
        math::cubic_spline_coefficients(N,a.get(),b.get(),c.get(),d.get(),x.data(),y.data(),w.data());
        poly_.reset(new Poly(N));
        auto& refPoly = *poly_;
        for(size_t i = 0; i < N; ++i)
        {
            refPoly[x[i]][0] = d[i]/6.;
            refPoly[x[i]][1] = c[i]/2.;
            refPoly[x[i]][2] = b[i];
            refPoly[x[i]][3] = a[i];
        }
    }

public:
    /**
     * Creates cubic spline from an initial data
     */
    cubic_spline(const xy_values_type& xy_vals,
            double smooth_param = 0.0,
            const data_vector_type& w = data_vector_type())
    {
        data_vector_type w_(xy_vals.size(), smooth_param);
        for(size_t i = 0; i < std::min(w_.size(), w.size()); ++i)
            w_[i] *= w[i];
        calculate_spline_(xy_vals, w_);
    }

    cubic_spline(const data_vector_type& x,
                 const data_vector_type& y,
                 double smooth_param = 0.0,
                 const data_vector_type& w = data_vector_type())
    {
        data_vector_type w_(std::min(x.size(), y.size()), smooth_param);
        for(size_t i = 0; i < std::min(w_.size(), w.size()); ++i)
            w_[i] *= w[i];
        calculate_spline_(x,y,w_);
    }

    virtual ~cubic_spline(){}

    /**
     * Returns ref to a peacewise polynomial
     */
    const Poly& poly() const { return *poly_; }
};

#endif // SPLINE_H
