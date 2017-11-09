#include "peacewisepoly.h"

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
