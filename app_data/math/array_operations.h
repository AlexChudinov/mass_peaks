#ifndef ARRAY_OPERATIONS_H
#define ARRAY_OPERATIONS_H

#include <cstddef>

namespace math {
    /**
     * Defines operations over array using recursive templates
     */
    template<class array_type_a, class array_type_b, size_t N>
    struct array_operations
    {
        using prior = array_operations<array_type_a, array_type_b, N - 1>;

        template<class op> static inline
        void umap(op Op, array_type_a& a)
        {
            prior::umap(Op, a);
            Op(a[N]);
        }

        template<class op> static inline
        void bmap(op Op, array_type_a& a, const array_type_b& b)
        {
            prior::bmap(Op, a, b);
            Op(a[N], b[N]);
        }

        template<class add, class T> static  inline
        void ufold(add Add, T& a, const array_type_a& b)
        {
            prior::ufold(Add, a, b);
            Add(a, b[N]);
        }

        template<class add, class mul, class T> static inline
        void bfold(add Add, mul Mul, T& a, const array_type_a& b, const array_type_b& c)
        {
            prior::bfold(Add, Mul, a, b, c);
            Add(a, Mul(b[N], c[N]));
        }
    };

    template<class array_type_a, class array_type_b>
    struct array_operations<array_type_a, array_type_b, 0>
    {
        template<class op> static inline
        void umap(op Op, array_type_a& a){ Op(a[0]); }

        template<class op> static inline
        void bmap(op Op, array_type_a& a, const array_type_b& b)
        { Op(a[0], b[0]); }

        template<class add, class T> static inline
        void ufold(add Add, T& a, const array_type_a& b)
        { Add(a, b[0]); }

        template<class add, class mul, class T> static inline
        void bfold(add Add, mul Mul, T& a, const array_type_a& b, const array_type_b& c)
        { Add(a, Mul(b[0], c[0])); }
    };

    /**
     * Templates implementing recursive for-loop
     */
    template<size_t begin, size_t end, bool stop> struct For;

    template<size_t begin, size_t end>
    struct For<begin, end, true>
    {
        using next = For<begin + 1, end, (begin + 1) < end>;

        template<class operation, typename ... T>
        static inline void Do(operation Op, T ... args)
        {
            Op(begin, args...);
            next::Do(Op, args...);
        }
    };

    template<size_t begin, size_t end>
    struct For<begin, end, false>
    {
        template<class operation, typename ... T>
        static inline void Do(operation /*Op*/, T ...) {}
    };

//One value applicator to a hole array
#define DEF_OPERATION_WITH_VAL(value_type, op) \
    struct operation{ \
        const value_type& val_;\
        inline operation(const value_type& val) : val_(val) {} \
        inline void operator()(value_type& x) const { x op val_; } \
    };
#define DEF_OPERATION_WITH_VAL_1(type1, type2, op)\
    struct operation{ \
        const type1& val_;\
        inline operation(const type1& val) : val_(val) {} \
        inline void operator()(type2& x) const { x op val_; } \
    };
}

#endif // ARRAY_OPERATIONS_H
