#ifndef APPROXIMATOR_FACTORY_H
#define APPROXIMATOR_FACTORY_H

#include <QScopedPointer>

#include "../app_data/math/spline.h"
#include "../new_math/peacewisepoly.h"

/**
 * @brief The Approximator class implements approximating engine
 */
class Approximator
{
public:
    using Vector = std::vector<double>;

    virtual ~Approximator(){}

    /**
     * @brief The Params class standart approximator parameters are x and y values
     */
    class Params
    {
        const Vector& m_vXVals;
        const Vector& m_vYVals;
    public:

        Params(const Vector& vXVals, const Vector& vYVals);
        virtual ~Params(){}

        const Vector& x() const;
        const Vector& y() const;
    };

    /**
     * Approximator type to choose
     */
    enum ApproximatorType
    {
        CubicSplineType = 0x00, ///cubic spline
        CubicSplineNewType = 0x01, ///cubic spline with new interface
    };

    static Approximator* create(ApproximatorType type, const Params& params);

    virtual ApproximatorType type() const = 0;

    /**
     * @brief approximate
     * @param vXVals x-values
     * @return vector of corresponding y-values
     */
    virtual Vector approximate(const Vector& vXVals) const = 0;

    /**
     * @brief getPeaks
     * @return maximums positions
     */
    virtual Vector getPeaks() const = 0;
};

/**
 * Regular cubic spline data approximation
 */
class CubicSplineApproximator : public Approximator
{
    using Spline = cubic_spline<double>;
    using PSpline = QScopedPointer<Spline>;

    PSpline m_pSpline;
public:

    ApproximatorType type() const;

    class CubicSplineParams : public Approximator::Params
    {
        const double m_fSmooth;
    public:

        CubicSplineParams(const Vector &vXVals, const Vector &vYVals, double fSmooth);

        double smooth() const;
    };

    CubicSplineApproximator(const CubicSplineParams& params);

    Vector approximate(const Vector &x) const;

    Vector getPeaks() const;
};

class CubicSplineApproximatorNew : public Approximator
{
    using Spline = StandartPeacewisePoly;
    using PSpline = QScopedPointer<Spline>;
    PSpline m_pSpline;
public:

    ApproximatorType type() const;

    CubicSplineApproximatorNew(const CubicSplineApproximator::CubicSplineParams& params);

    Vector approximate(const Vector& vXVals) const;
    Vector getPeaks() const;
};

#endif // APPROXIMATOR_FACTORY_H
