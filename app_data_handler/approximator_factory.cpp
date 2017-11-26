#include "../app_data_handler/approximator_factory.h"

Approximator::Params::Params(const Vector &vXVals, const Vector &vYVals)
    :
      m_vXVals(vXVals),
      m_vYVals(vYVals)
{
}


const Approximator::Vector& Approximator::Params::x() const
{
    return m_vXVals;
}

const Approximator::Vector& Approximator::Params::y() const
{
    return m_vYVals;
}

Approximator* Approximator::create(ApproximatorType type, const Params& params)
{
    switch(type)
    {
    case CubicSplineType:
        return new CubicSplineApproximator
        (
            static_cast<const CubicSplineApproximator::CubicSplineParams&>(params)
        );
    case CubicSplineNewType:
        return new CubicSplineApproximatorNew
        (
            static_cast<const CubicSplineApproximator::CubicSplineParams&>(params)
        );
    case CubicSplineEqualStepSizeType:
        return new CubicSplineEqualStepSizeApproximator
        (
            static_cast<const CubicSplineApproximator::CubicSplineParams&>(params)
        );
    default:
        return Q_NULLPTR;
    }
}

Approximator::ApproximatorType CubicSplineApproximator::type() const
{
    return CubicSplineType;
}

CubicSplineApproximator::CubicSplineParams::CubicSplineParams(const Vector &vXVals, const Vector &vYVals, double fSmooth)
    :
      Approximator::Params(vXVals, vYVals),
      m_fSmooth(fSmooth)
{}

double CubicSplineApproximator::CubicSplineParams::smooth() const
{
    return m_fSmooth;
}

CubicSplineApproximator::CubicSplineApproximator(const CubicSplineParams& params)
    :
      m_pSpline(new Spline(params.x(), params.y(), params.smooth()))
{}

Approximator::Vector CubicSplineApproximator::approximate(const Vector& vXVals) const
{
    return m_pSpline->poly().estimate_y_vals(vXVals);
}

Approximator::Vector CubicSplineApproximator::getPeaks() const
{
    return m_pSpline->poly().get_maxs();
}

Approximator::ApproximatorType CubicSplineApproximatorNew::type() const
{
    return CubicSplineNewType;
}

CubicSplineApproximatorNew::CubicSplineApproximatorNew
(
    const CubicSplineApproximator::CubicSplineParams &params
)
    :
      m_pSpline(new StandartPeacewisePoly(params.x(), params.y(), params.smooth()))
{}

Approximator::Vector CubicSplineApproximatorNew::approximate(const Vector& vXVals) const
{
    Vector y(vXVals.size());
    for(size_t i = 0; i < y.size(); ++i){
        y[i] = (*m_pSpline)(vXVals[i]);
    }
    return y;
}

Approximator::Vector CubicSplineApproximatorNew::getPeaks() const
{
    return Vector();
}

Approximator::ApproximatorType CubicSplineEqualStepSizeApproximator::type() const
{
    return CubicSplineEqualStepSizeType;
}

CubicSplineEqualStepSizeApproximator::CubicSplineEqualStepSizeApproximator
(
    const CubicSplineApproximator::CubicSplineParams &params
)
{
    double h = params.x()[1] - params.x()[0];
    for(size_t i = 1; i < params.x().size() - 1; ++i)
    {
        h = qMin(h, qAbs(params.x()[i+1] - params.x()[i]));
    }
    StandartPeacewisePoly tSpline(params.x(), params.y(), params.smooth());
    m_pSpline.reset(new EqualStepPeacewisePoly(tSpline, h));
}

Approximator::Vector CubicSplineEqualStepSizeApproximator::approximate(const Vector &vXVals) const
{
    Vector vYVals(vXVals.size());
    for(size_t i = 0; i < vXVals.size(); ++i)
    {
        vYVals[i] = (*m_pSpline)(vXVals[i]);
    }
    return vYVals;
}

Approximator::Vector CubicSplineEqualStepSizeApproximator::getPeaks() const
{
    return Vector();
}
