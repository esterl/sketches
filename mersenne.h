#ifndef SKETCHES_MERSENNE_H
#define SKETCHES_MERSENNE_H
// The modulus operator with Mersenne prime 'p = 2^s-1' can be simplified as:
// i = (x & p) + (k >> s); if i>=p then return i-p else return i;
template<typename T>
inline T mersenne_modulus(T value, int mersenne_exponent){
    T p = 1U;
    p = (p << mersenne_exponent) - p;
    T i = (value & p) + (value >> mersenne_exponent);
    return i >= p ? i - p : i;
}

template<typename T>
inline int get_mersenne_exponent(){
    int mersenne_exponent;
    uint32_t space_exponent = sizeof(T)*8;
    if (space_exponent < 17)
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
#endif //SKETCHES_MERSENNE_H
