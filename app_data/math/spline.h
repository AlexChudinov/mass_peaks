#ifndef SPLINE_H
#define SPLINE_H

#include <vector>

using data_vector = std::vector<double>;

/**
 * Cubic spline implementation
 */
class cubic_spline
{
    data_vector a, b, c, d; //cubic spline coefs S(x) = a + bx + cx^2 + dx^3;
public:
    cubic_spline();
};

#endif // SPLINE_H
