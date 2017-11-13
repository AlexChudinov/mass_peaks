#include <cassert>
#include <map>

#include "peacewisepoly.h"
#include "app_data/math/solvers.h"

PeacewisePoly::PeacewisePoly(uint8_t nDegree, size_t nCoefsSize)
    :
      m_nDegree(nDegree),
      m_vCoefs(nCoefsSize)
{

}

void PeacewisePoly::diff()
{
    Vector vCoefsNew(m_vCoefs.size()/m_nDegree * (m_nDegree - 1));
    size_t cntr = 0;
    for(size_t idx = 0; idx < m_vCoefs.size()/m_nDegree; idx++)
    {
        PeaceCoefs pCoefs = intervalCoefs(idx);
        int factor = m_nDegree;
        for(auto it = pCoefs.first; it != pCoefs.second-1; ++it){
            vCoefsNew[cntr++] = (factor--)**it;
        }
    }
    m_vCoefs = vCoefsNew;
    m_nDegree--;
}

PeacewisePoly::PeaceCoefs PeacewisePoly::intervalCoefs(size_t idx) const
{
    return std::make_pair(m_vCoefs.cbegin() + idx * m_nDegree,
                          m_vCoefs.cbegin() + (idx + 1) * m_nDegree);
}

StandartPeacewisePoly::StandartPeacewisePoly(const Vector &xVals, const Vector &yVals, double fSmoothParam)
    :
      PeacewisePoly(3, 4*xVals.size()),
      m_xVals(xVals.size())
{
    assert(xVals.size() == yVals.size());
    //Check data sorting or do data sort whatever
    Vector tempYVals(xVals.size());
    std::map<double, double> mapXYData;
    for(size_t i = 0; i < xVals.size(); ++i) mapXYData[xVals[i]] = yVals[i];
    assert(xVals.size() == mapXYData.size());
    size_t i = 0;
    for(const auto& xy : mapXYData)
    {
        m_xVals[i]    = xy.first;
        tempYVals[i++]= xy.second;
    }

    Vector  w(xVals.size(), fSmoothParam),
            a(xVals.size()),
            b(xVals.size()),
            c(xVals.size()),
            d(xVals.size());
    math::cubic_spline_coefficients(xVals.size(), a.data(),
                                    b.data(), c.data(), d.data(),
                                    xVals.data(), tempYVals.data(), w.data());
    for(size_t idx = 0; idx < xVals.size(); ++idx)
    {
        coefs()[4*idx]     = d[idx]/6.;
        coefs()[4*idx + 1] = c[idx]/2.;
        coefs()[4*idx + 2] = b[idx];
        coefs()[4*idx + 3] = a[idx];
    }
}

PeacewisePoly::PolyType StandartPeacewisePoly::type() const
{
    return PolyStandartType;
}

double StandartPeacewisePoly::operator ()(double x) const
{
    size_t idx = findInterval(x);
    PeaceCoefs coefs = intervalCoefs(idx);
    double t = x - m_xVals[idx];
    double res = *coefs.first;
    for(auto it = coefs.first + 1; it != coefs.second; ++it)
    {
        res *= t;
        res += *it;
    }
    return res;
}

std::size_t StandartPeacewisePoly::findInterval(double x) const
{
    auto it = std::lower_bound(m_xVals.begin(), m_xVals.end(), x);
    if(it == m_xVals.begin()) return 0;
    else return std::distance(m_xVals.begin(), std::prev(it));
}
