#ifndef SYM_DIAG_MATRIX_H
#define SYM_DIAG_MATRIX_H

#include <vector>
#include <cstdlib>
#include <algorithm>


#include "matrixtemplate.h"

namespace math {

/**
 * Symmetric diagonal matrices
 */
template<size_t D>
class sym_diag_mat
{
public:
    using row_vals = math::vector_c<double, 2*D + 1>;
    using data_vector = std::vector<row_vals>;

private:
    data_vector data_;

public:
    /**
     * Preallocates data storage
     */
    sym_diag_mat(size_t size)
        :
          data_(size) {}

    /**
     * Uniform diagonal values
     */
    sym_diag_mat(const math::vector_c<double, D + 1>& vals, size_t size)
        :
          data_(size)
    {
        sym_diag_mat& M = *this;

        for(size_t i = 0; i < size; ++i) M.data_[i][D] = vals[0];

        for(size_t i = 1; i <= D; ++i)
            for(size_t j = 0; j < size-D; ++j)
                M.data_[j][2*D + 1 - i] = M.data_[j + i][i - 1] = vals[i];
    }

    size_t size() const { return data_.size(); }


};

template<size_t D1, size_t D2>
sym_diag_mat< D1 + D2 > matrix_multiplication(const sym_diag_mat<D1>& md1, const sym_diag_mat<D2>& md2)
{
    using vectorD1plusD2 = math::vector_c<double, 2 * (D1 + D2) + 1>;
    assert(md1.size() == md2.size());
    size_t N = md1.size();
    constexpr size_t D = D1 + D2;
    sym_diag_mat< D1 + D2 > m(N);


    for(size_t i = 0; i < D; ++i)
        for(size_t j = D - i; j < 2*D + 1; ++j)
        {
            vectorD1plusD2 v1, v2;
            v1 = math::put<double, 2 * (D1 + D2) + 1, D1>(md1[]);
        }
}

} //end of namespace math
#endif // SYM_DIAG_MATRIX_H
