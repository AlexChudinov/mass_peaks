#ifndef MATHUTILITY_H
#define MATHUTILITY_H

/*!
 * Defines some frequently used math functions
 */

namespace math {

/*!
 * Something in square it could be a vector or a number whatsever
 */
template<class sqT, class T> inline sqT sqr(const T& t) { return t*t; }

/*!
 * Weighting mean using vector::iterator interface
 * accumulator should be initialized with zero value of type T1
 * lengths of v and w are supposed to be equal
 */
template<class T1, class T2, template<class ...> class vector>
inline void mean
(
        const vector<T1>& v,
        const vector<T2>& w,
        T1& accumulator)
{
    typename vector<T1>::const_iterator it1 = v.begin();
    typename vector<T2>::const_iterator it2 = w.begin();
    T2 total(0.0); //Total usually has a numeric type hence it can be initialized with 0.0

    for(; it1 != v.end(); ++it1, ++it2)
    {
        total += *it2;
        accumulator += (*it1 * *it2);
    }

    accumulator /= total;
}

/**
 * In place summation
 */
template<class T> struct in_place_plus
{ inline T& operator()(T& x, T const& y) const { return x+=y;} };

/**
 * In place subtraction
 */
template<class T> struct in_place_sub
{ inline T& operator()(T& x, T const& y) const { return x-=y; } };

/**
 * In place multiplication
 */
template<class T> struct in_place_mul
{ inline T& operator()(T& x, T const& y) const { return x*=y;} };

/**
 * In place division
 */
template<class T> struct in_place_div
{ inline T& operator()(T& x, T const& y) const { return x/=y; } };

/**
 * Sets value
 */
template<class T> struct set_val
{ inline T& operator()(T& x, T const& y) const { return x = y; } };

}

#endif // MATHUTILITY_H
