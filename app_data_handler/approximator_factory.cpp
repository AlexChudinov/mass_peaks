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
                (static_cast<const CubicSplineApproximator::CubicSplineParams&>(params));
    case CubicSplineNewType:
        return new CubicSplineApproximatorNew
                (static_cast<const CubicSplineApproximator::CubicSplineParams&>(params));
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

CubicSplineApproximatorNew::CubicSplineApproximatorNew(const CubicSplineApproximator::CubicSplineParams &params)
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
