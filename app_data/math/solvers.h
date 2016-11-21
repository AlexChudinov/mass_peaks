#ifndef SOLVERS_H
#define SOLVERS_H

#include <memory>
#include <algorithm>

namespace math
{
    /**
     * Solves a system of equations with a symmetric diagonal matrix, no error checking supported
     */
    template <class DataType>
    int tridiagonalsolve
        (
            int n, //number of equations
            DataType* a,  //down diagonal
            DataType* b,  //main diagonal
            DataType* c,  //upper diagonal
            DataType* r,  //right-hand part
            DataType* x   //solution
        ) noexcept
    //solve Ax=b where A is a tridiagonal matrix, returns 0 if it is ok
    //It changes inserted data array b and r.
    {
        for (int i = 0; i < n-1; i++)
        {
            DataType m = a[i] / b[i];
            b[i+1] = b[i+1] - m * c[i];
            r[i+1] = r[i+1] - m * r[i];
        }
        x[n - 1] = r[n - 1] / b[n - 1];
        for (int i = n - 2; i >= 0; i--)
                x[i] = (r[i] - c[i] * x[i + 1]) / b[i];
        return 0;
    }
    /**
     * Solves five diagonal linear equation system with a symmetric matrix, no error checking supported
     */
    template<class DataType> int fivediagonalsolve
        (
            int n,          //number of equations
            const DataType* a,
            DataType* b,
            DataType* c,    //main diagonal
            DataType* d,
            DataType* e,
            DataType* r,    //right-hand part
            DataType* x     //solution
        ) noexcept
    {
        for(int i = 0; i < n-2; i++)
        {
            DataType m1 = b[i]/c[i];
            DataType m2 = a[i]/c[i];
            c[i+1] = c[i+1] - m1*d[i];
            d[i+1] = d[i+1] - m1*e[i];
            b[i+1] = b[i+1] - m2*d[i];
            c[i+2] = c[i+2] - m2*e[i];
            r[i+1] = r[i+1] - m1*r[i];
            r[i+2] = r[i+2] - m2*r[i];
        }
        DataType m3 = b[n-2]/c[n-2];
        c[n-1] = c[n-1] - m3*d[n-2];
        r[n-1] = r[n-1] - m3*r[n-2];
        x[n-1] = r[n-1] / c[n-1];
        x[n-2] = (r[n-2] - d[n-2]*x[n-1]) / c[n-2];

        for(int i = n-3; i >= 0; i--)
            x[i] = (r[i] - d[i]*x[i+1] - e[i]*x[i+2]) / c[i];

        return 0;
    }

    ///Solves equation fun(x) = 0 (abs(fun(x))<eps) on interval [a,b]
    /// Note, that it is supposed that function has only one zero at the interval
    /// \param fun Function
    /// \param a Left interval boundary
    /// \param b Right interval bounadry
    /// \param eps Small value
    template<class FunObject, class DataType>
    DataType fZero(FunObject fun, DataType a, DataType b, DataType eps)
    {
        auto abs = [](DataType x)->DataType
        {
            return x >= static_cast<DataType>(0.0) ? x : -x;
        };

        if(fun(a)*fun(b) > static_cast<DataType>(0.0)
                || (abs(fun(a)) < eps && abs(fun(b)) < eps))
            return fun(a) <= fun(b) ? a : b;

        if(fun(a) < fun(b)) std::swap(a,b);

        while(true)
        {
            DataType c = .5*(a+b), fc = fun(c);
            if (abs(fc)<eps) return c;
            if (fc < 0) b = c;
            if (fc > 0) a = c;
        }
    }

    /**
     * Calculates coefficients of a smoothing cubic spline
     * S(x) = a + b*x + c*x^2/2 + d*x^3/6
     * Note: no exceptions is not guaranteed
     */
    template<typename Float>
    void cubic_spline_coefficients
    (
            size_t N, //number of points
            Float* a,
            Float* b,
            Float* c,
            Float* d,
            const Float* const x,
            const Float* const y,
            const Float* const w
    )
    {
        Float h1, h2, h3;

        //Set boundaries:
        a[0] = a[N-1] = 1./6.;
        b[0] = b[N-2] = c[0] = c[N-3] = d[0] = d[N-1] = 0.0;
        //********************

        //Set matrix values
        for(size_t i = 1; i < N-3; ++i)
        {
            h1 = x[i] - x[i-1];
            h2 = x[i+1] - x[i];
            h3 = x[i+2] - x[i+1];

            a[i] = 1./3. * (h1 + h2) + 1./h1/h1 * w[i-1]
                    + (1./h1 + 1./h2)*(1./h1 + 1./h2) * w[i]
                    + 1./h2/h2 * w[i+1];

            d[i] = (y[i+1] - y[i]) / h2 - (y[i] - y[i-1]) / h1;

            b[i] = 1./6. * h2 - 1./h2 * ((1./h1 + 1./h2)*w[i]
                                         + (1./h2 + 1./h3)*w[i+1]);

            c[i] = 1./h2/h3 * w[i+1];
        }
        h1 = x[N-3] - x[N-4];
        h2 = x[N-2] - x[N-3];
        h3 = x[N-1] - x[N-2];
        b[N-3] = 1./6. * h2 - 1./h2 * ((1./h1 + 1./h2)*w[N-3]
                                       + (1./h2 + 1./h3)*w[N-2]);
        a[N-3] = 1./3. * (h1 + h2) + 1./h1/h1 * w[N-4]
                + (1./h1 + 1./h2)*(1./h1 + 1./h2) * w[N-3]
                + 1./h2/h2 * w[N-2];
        d[N-3] = (y[N-2] - y[N-3]) / h2 - (y[N-3] - y[N-4]) / h1;
        a[N-2] = 1./3. * (h2 + h3) + 1./h2/h2 * w[N-3]
                + (1./h2 + 1./h3)*(1./h2 + 1./h3) * w[N-2]
                + 1./h3/h3 * w[N-1];
        d[N-2] = (y[N-1] - y[N-2]) / h3 - (y[N-2] - y[N-3]) / h2;

        //duplicate values for a symmetric matrix
        std::unique_ptr<Float[]>
                cl(new Float[N-2]),
                bl(new Float[N-1]),
                c_(new Float[N]); //Preallocate to temporary keep solution
        std::copy(c, c+N-2, cl.get());
        std::copy(b, b+N-1, bl.get());
        /***************/

        //Calculates second order spline derivatives into c_
        math::fivediagonalsolve(N, cl.get(), bl.get(), a, b, c, d, c_.get());

        h1 = x[1] - x[0]; h2 = x[N-1] - x[N-2];
        a[0]  = y[0]  - (c_[1] - c_[0]) / h1 * w[0];
        a[N-1]= y[N-1]+ (c_[N-1] - c[N-2]) / h2 * w[N-1];
        d[0] = (c_[1] - c_[0]) / h1;
        for(size_t i = 1; i < N-1; ++i)
        {
            h1 = x[i] - x[i-1];
            h2 = x[i+1] - x[i];
            a[i] = y[i] - w[i] * ((c_[i+1] - c_[i]) / h2
                    - (c_[i] - c_[i-1]) / h1);
            d[i] = (c_[i+1] - c_[i]) / h2;
            b[i-1] = (a[i] - a[i-1]) / h1
                    - (c_[i-1] / 2. + d[i-1] / 6. * h1) * h1;
        }
        h1 = x[N-1] - x[N-2];
        b[N-2] = (a[N-1] - a[N-2]) / h1
                - (c_[N-2] / 2. + d[N-2] / 6. * h1) * h1;
        b[N-1] = b[N-2] + (c_[N-2] + d[N-2] * h1 / 2.) * h1;
        std::copy(c_.get(), c_.get() + N, c);
    }
}

#endif // SOLVERS_H
