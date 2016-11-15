#ifndef SYM_DIAG_MATRIX_H
#define SYM_DIAG_MATRIX_H

#include <vector>
#include <cstdlib>
#include <algorithm>
#include <array>

/**
 * Symmetric diagonal matrices
 */
template<size_t D, size_t N>
class sym_diag_mat
{
public:
    using data_vector = std::vector<double>;

private:
    data_vector data_;

public:
    sym_diag_mat()
        :
          data_(N * (2*D + 1)) {}

    /**
     * Uniform diagonal values
     */
    sym_diag_mat(const std::array<double, D + 1>& vals)
        :
          data_(N * (2*D + 1))
    {
        for(size_t i = 0; i < N; ++i)
            for(size_t j = (i - D > 0 ? i - D : 0);
                j < (i + D <= N ? i + D : N); ++j)

    }

    /**
     * Returns pointer to a first element of a diagonal
     */
    double * diag(int n) { return &data_[N * (D + n)]; }
    const double * diag(int n) const { return &data_[N * (D + n)]; }

    /**
     * Returns matrix element
     */
    const double& operator()(size_t i, size_t j) const
    {
        int ndiag = j - i;
        return ndiag < 0 ? diag(ndiag)[i] : diag(ndiag)[i - ndiag];
    }
    double& operator () (size_t i, size_t j)
    {
        int ndiag = j - i;
        return ndiag < 0 ? diag(ndiag)[i] : diag(ndiag)[i - ndiag];
    }
};

/**
 * Matrix matrix multiplication
 */
template<size_t D1, size_t D2, size_t N>
sym_diag_mat<D1 + D2 - 1, N> mult(const sym_diag_mat<D1, N>& m1, const sym_diag_mat<D2, N>& m2)
{
    size_t ndiags = D1 + D2 - 1;
    for(size_t i = 0; i < N; ++i);
}

#endif // SYM_DIAG_MATRIX_H
