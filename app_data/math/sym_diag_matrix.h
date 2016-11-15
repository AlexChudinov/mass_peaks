#ifndef SYM_DIAG_MATRIX_H
#define SYM_DIAG_MATRIX_H

#include <vector>
#include <cstdlib>
#include <algorithm>

/**
 * Symmetric diagonal matrices
 */
template<std::size_t D, std::size_t N>
class sym_diag_mat
{
public:
    using data_vector = std::vector<double>;
    using std::size_t;

private:
    data_vector data_;

public:
    sym_diag_mat()
        :
          data_(N * (2*D + 1)) {}

    /**
     * Returns pointer to a first element of a diagonal
     */
    double * diag(int n) { return &data_[N * (D + n)]; }

    const double * diag(int n) const { return &data_[N * (D + n)]; }

    /**
     * Returns matrix element
     */
    double operator()(size_t i, size_t j) const
    {
        if(std::abs(i-j) > D) return 0.0;
        size_t n = std::min(i, j);

    }
};

/**
 * Matrix matrix multiplication
 */
template<std::size_t D1, std::size_t D2, std::size_t N>
sym_diag_mat<D1 + D2 - 1, N> mult(const sym_diag_mat<D1, N>& m1, const sym_diag_mat<D2, N>& m2)
{
    using std::size_t;
    size_t ndiags = D1 + D2 - 1;
    for(size_t i = 0; i < N; ++i)
        for(j = i )
}

#endif // SYM_DIAG_MATRIX_H
