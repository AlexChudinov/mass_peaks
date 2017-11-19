#ifndef PEACEWISEPOLY_H
#define PEACEWISEPOLY_H

#include <cstdint>
#include <vector>

#define MAX_SPLINE_STEPS

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
    inline double operator()(double x) const
    {
        size_t idx = findInterval(x);
        return estimateSpline(idx, findDxValue(x, idx));
    }

    /**
     * @brief nSteps
     * @return Number of intervals in polynomial
     */
    size_t nSteps() const { return m_vCoefs.size() / (m_nDegree + 1); }

    /**
     * @brief coefs
     * @return set/get spline coefficients
     */
    inline const Vector& coefs() const { return m_vCoefs; }
    inline Vector& coefs() { return m_vCoefs; }

protected:

    /**
     * @brief findInterval looks for interval for which x belongs to
     * @param x
     * @return index of interval
     */
    virtual size_t findInterval(double x) const = 0;

    /**
     * @brief findDxValue
     * @param x Value at which spline needs to be estimated
     * @param idx Idx of spline interval to which given x value belongs to
     * @return Delta x value for given interval
     */
    virtual double findDxValue(double x, size_t idx) const = 0;

    /**
     * @brief intervalCoefs returns coefficients for interval index idx
     * @param idx
     * @return
     */
    PeaceCoefs intervalCoefs(size_t idx) const;

    /**
     * @brief estimateSpline estimates spline at inerval idx
     * @param idx
     * @param t interval x coordinate
     * @return spline value at x0 + t coordinate
     */
    inline double estimateSpline(size_t idx, double t) const {
        PeaceCoefs coefs = intervalCoefs(idx);
        double res = *coefs.first;
        for(auto it = coefs.first + 1; it != coefs.second; ++it)
        {
            res *= t;
            res += *it;
        }
        return res;
    }

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
    StandartPeacewisePoly(const Vector& xVals,
                          const Vector& yVals,
                          double fSmoothParam = 0.0);

    virtual PolyType type() const;

    inline double xMin() const { return *m_xVals.begin(); }

    inline double xMax() const { return *m_xVals.rbegin(); }

protected:
    size_t findInterval(double x) const;

    double findDxValue(double x, size_t idx) const;

private:
    Vector m_xVals;
};

/**
 * @brief The EqualStepPeacewisePoly class implements peacewise poly on equal x-intervals
 */
class EqualStepPeacewisePoly : public PeacewisePoly
{
public:
    /**
     * @brief EqualStepPeacewisePoly creates EqualStepPeacewisePoly from StandartPeacewisePoly
     * @param poly
     * @param h step between subsequent x-values
     */
    EqualStepPeacewisePoly(const StandartPeacewisePoly& poly, double h);

    virtual PolyType type() const;

protected:
    size_t findInterval(double x) const;

    double findDxValue(double x, size_t idx) const;

private:
    double m_fH;
    double m_fXMin, m_fXMax;

    /**
     * @brief estimateSplineSteps estimates number of steps in spline
     * @param min min point of the spline
     * @param max max point of the spline
     * @param h spline step
     * @return  number of steps in spline
     */
    static inline int estimateSplineSteps(double min, double max, double h){
        int n = int((max-min)/h);
        return (max-min) - n*h == 0 ? n : n + 1;
    }
};


#endif // PEACEWISEPOLY_H
