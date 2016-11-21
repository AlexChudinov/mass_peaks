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
        xyval = xyval == poly_coefs_.begin() ? xyval : --xyval;
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
            auto& diff_coefs = res[coef->first];
            math::For<0, n, true>::Do([&diff_coefs, coef](size_t j)
            {
                diff_coefs[j] = (n - j) * coef->second[j];
            });
        }

    }

    virtual ~peacewise_poly(){}
};

/**
 * Cubic spline implementation
 */
template<typename Float = double>
class cubic_spline
{
    using xy_values_type = std::map<Float, Float>;
    using data_vector_type = std::vector<Float>;
    using peacewise_poly = peacewise_poly<3, Float>;

    std::unique_ptr<peacewise_poly> poly_;

    /**
     * Calculates spline coefs
     */
    void calculate_spline_(const xy_values_type& xy_vals, const data_vector_type& w)
    {
        size_t N = xy_vals.size();
        std::unique_ptr<Float[]>
                a(new Float[N]),
                b(new Float[N]),
                c(new Float[N]),
                d(new Float[N]),
                x(new Float[N]),
                y(new Float[N]);

        auto it = xy_vals.cbegin();
        for(size_t i = 0; it != xy_vals.cend(); ++it, ++i)
        {
            x[i] = it->first;
            y[i] = it->second;
        }

        math::cubic_spline_coefficients(
                    N,
                    a.get(),
                    b.get(),
                    c.get(),
                    d.get(),
                    x.get(),
                    y.get(),
                    w.data());

        poly_.reset(new peacewise_poly(xy_vals.size()));
        auto& refPoly = *poly_;

        for(size_t i = 0; i < N; ++i)
        {
            std::array<Float, 4>& coefs = refPoly[x[i]];
            coefs[0] = d[i]/6.;
            coefs[1] = c[i]/2.;
            coefs[2] = b[i];
            coefs[3] = a[i];
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
        if(!w.empty())
        {
            for(size_t i = 0; i < std::min(w_.size(), w.size()); ++i)
                w_[i] *= w[i];
        }
        calculate_spline_(xy_vals, w_);
    }

    virtual ~cubic_spline(){}

    /**
     * Returns ref to a peacewise polynomial
     */
    const peacewise_poly& poly() const { return *poly_; }

private:

};

#endif // SPLINE_H
