#ifndef SKETCHES_MERSENNE_H
#define SKETCHES_MERSENNE_H
#include "ttmath/ttmath.h"

// The modulus operator with Mersenne prime 'p = 2^s-1' can be simplified as:
// i = (x & p) + (k >> s); if i>=p then return i-p else return i;
template<typename T>
inline T mersenne_modulus(T value, int mersenne_exponent) {
    T p = 1U;
    p = (p << mersenne_exponent) - p;
    T i = (value & p) + (value >> mersenne_exponent);
    return i >= p ? i - p : i;
}

template<typename T>
inline int get_mersenne_exponent() {
    int mersenne_exponent;
    uint32_t space_exponent = sizeof(T) * 8;
    if (space_exponent < 13)
        mersenne_exponent = 13;
    else if (space_exponent < 17)
        mersenne_exponent = 17;
    else if (space_exponent < 31)
        mersenne_exponent = 31;
    else if (space_exponent < 61)
        mersenne_exponent = 61;
    else if (space_exponent < 89)
        mersenne_exponent = 89;
    else if (space_exponent < 521)
        mersenne_exponent = 521;
    else
        throw std::domain_error("Element type two long");
    return mersenne_exponent;
}

//Define types associated with each prime and uint128
typedef ttmath::UInt<2> uint128_t;
typedef uint32_t prime13_t;
typedef uint64_t prime17_t;
typedef uint64_t prime31_t;
typedef ttmath::UInt<2> prime61_t;
typedef ttmath::UInt<3> prime89_t;
typedef ttmath::UInt<17> prime521_t;

// Define types associated for each input space
template<typename T>
struct hash_type_for;

template<>
struct hash_type_for<uint8_t> {
    using type = uint32_t;
};

template<>
struct hash_type_for<uint16_t> {
    using type = uint64_t;
};

template<>
struct hash_type_for<unsigned int> {
    using type = long unsigned int;
};

template<>
struct hash_type_for<uint128_t> {
    using type = prime521_t;
};

template<typename T>
using hash_type_for_t = typename hash_type_for<T>::type;

#endif //SKETCHES_MERSENNE_H
