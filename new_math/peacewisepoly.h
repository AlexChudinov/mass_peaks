#ifndef PEACEWISEPOLY_H
#define PEACEWISEPOLY_H

#include <cstdint>
#include <vector>

/**
 * Interface to peacewise polynomial class
 */
class PeacewisePoly
{
public:
    using uint8_t = std::uint8_t;
    using size_t = std::size_t;
    using Vector = std::vector<double>;
    using PeaceCoefs = std::pair<Vector::const_iterator, Vector::const_iterator>;

    /**
     * @brief PeacewisePoly sets degree and allocates storage for coefficients
     * @param nDegree
     * @param nCoefsSize
     */
    PeacewisePoly(uint8_t nDegree, size_t nCoefsSize);
    virtual ~PeacewisePoly(){}

    ///Types of polynomials that can be created
    enum PolyType
    {
        PolyStandartType, ///<Creates peacewise polynomial of a standart type
        PolyEqualType ///<Creates peacewise polynomial with equally distant steps
    };

    virtual PolyType type() const = 0;

    inline uint8_t degree() const { return m_nDegree;}
    inline void degree(uint8_t nDegree) { m_nDegree = nDegree; }

    /**
     * @brief diff differentiates the polynomial
     */
    void diff();

    /**
     * @brief operator ()
     * @param x
     * @return Y-value correspondent to a given X-value
     */
    virtual double operator()(double x) const = 0;

protected:
    inline const Vector& coefs() const { return m_vCoefs; }
    inline Vector& coefs() { return m_vCoefs; }

    /**
     * @brief findInterval looks for interval for which x belongs to
     * @param x
     * @return index of interval
     */
    virtual size_t findInterval(double x) const = 0;

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

/**
 * @brief The StandartPeacewisePoly class
 */
class StandartPeacewisePoly : public PeacewisePoly
{
public:
    /**
     * @brief StandartPeacewisePoly calculates spline
     * @param xVals
     * @param yVals
     * @param fSmoothParam smoothing parameter for the spline line
     * @param dataSorted if xVals is sorted then it should be true
     */
    StandartPeacewisePoly(const Vector& xVals, const Vector& yVals, double fSmoothParam = 0.0);

    virtual PolyType type() const;

    virtual double operator ()(double x) const;

protected:
    size_t findInterval(double x) const;

private:
    Vector m_xVals;
};

#endif // PEACEWISEPOLY_H
