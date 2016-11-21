#ifndef ARRAY_OPERATIONS_H
#define ARRAY_OPERATIONS_H

#include <cstddef>

namespace math {
    /**
     * Defines operations over array using recursive templates
     */
    template<typename array_type_a, typename array_type_b, size_t N>
    struct array_operations
    {
        using prior = array_operations<array_type_a, array_type_b, N - 1>;

        template<typename op> static inline
        void umap(op Op, array_type_a& a)
        {
            prior::umap(Op, a);
            Op(a[N]);
        }

        template<typename op> static inline
        void bmap(op Op, array_type_a& a, const array_type_b& b)
        {
            prior::bmap(Op, a, b);
            Op(a[N], b[N]);
        }

        template<typename add, typename T> static inline
        void ufold(add Add, T& a, const array_type_a& b)
        {
            prior::ufold(Add, a, b);
            Add(a, b[N]);
        }

        template<typename add, typename mul, typename T> static inline
        void bfold(
                add Add,
                mul Mul,
                T& a,
                const array_type_a& b,
                const array_type_b& c)
        {
            prior::bfold(Add, Mul, a, b, c);
            Add(a, Mul(b[N], c[N]));
        }
    };

    template<typename array_type_a, typename array_type_b>
    struct array_operations<array_type_a, array_type_b, 0>
    {
        template<typename op> static inline
        void umap(op Op, array_type_a& a) { Op(a[0]); }

        template<typename op> static inline
        void bmap(op Op, array_type_a& a, const array_type_b& b)
        { Op(a[0], b[0]); }

        template<typename add, typename T> static inline
        void ufold(add Add, T& a, const array_type_a& b)
        { Add(a, b[0]); }

        template<typename add, typename mul, typename T> static inline
        void bfold(
                add Add,
                mul Mul,
                T& a,
                const array_type_a& b,
                const array_type_b& c)
        {
            Add(a, Mul(b[0], c[0]));
        }
    };

    /**
     * Templates implementing recursive for-loop
     */
    template<size_t begin, size_t end, bool stop> struct For;

    template<size_t begin, size_t end>
    struct For<begin, end, true>
    {
        using next = For<begin + 1, end, (begin + 1) < end>;

        template<typename operation, typename ... T>
        static inline void Do(operation Op, T ... args)
        {
            Op(begin, args...);
            next::Do(Op, args...);
        }
    };

    template<size_t begin, size_t end>
    struct For<begin, end, false>
    {
        template<typename operation, typename ... T>
        static inline void Do(operation /*Op*/, T ...){}
    };

} //end of math namespace
#endif // ARRAY_OPERATIONS_H
