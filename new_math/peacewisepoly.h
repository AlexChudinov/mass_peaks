#ifndef PEACEWISEPOLY_H
#define PEACEWISEPOLY_H

#include <cstdint>
#include <vector>

/**
 * Interface to peacewise polynomial class
 */
class PeacewisePoly
{
    using uint8_t = std::uint8_t;
    using size_t = std::size_t;
    using Vector = std::vector<double>;
    using PeaceCoefs = std::pair<Vector::const_iterator, Vector::const_iterator>;

public:
    virtual ~PeacewisePoly(){}

    ///Types of polynomials that can be created
    enum PolyTypes
    {
        PolyStandartType, ///<Creates peacewise polynomial of a standart type
        PolyEqualType ///<Creates peacewise polynomial with equally distant steps
    };

    inline uint8_t degree() const { return m_nDegree;}
    inline void degree(uint8_t nDegree) { m_nDegree = nDegree; }

    /**
     * @brief diff differentiates the polynomial
     */
    void diff();

protected:
    inline const Vector& coefs() const { return m_vCoefs; }
    inline Vector& coefs() { return m_vCoefs; }

    /**
     * @brief findInterval looks for interval for which x belongs to
     * @param x
     * @return index of interval
     */
    virtual size_t findInterval(double x) = 0;

    /**
     * @brief intervalCoefs returns coefficients for interval index idx
     * @param idx
     * @return
     */
    PeaceCoefs intervalCoefs(size_t idx) const;

private:
     uint8_t m_nDegree;
     Vector m_vCoefs;
};

#endif // PEACEWISEPOLY_H
