#include "mersenne.h"
#include "random.h"

inline bool isPowerOfTwo(unsigned x)
{
  return ((x != 0) && !(x & (x - 1)));
}
/**************************Hash_CW2 implementation*****************************/

template<typename T1, typename T2>
void Hash_CW2<T1,T2>::init(unsigned B, unsigned exponent, T2 seed0, T2 seed1)
{
    mersenne_exponent = exponent;
    if (!isPowerOfTwo(B)) 
        throw std::domain_error("The number of buckets should be a power of 2");
    mask = B - 1;
    seeds[0] = mersenne_modulus<T2>(seed0, exponent);
    seeds[1] = mersenne_modulus<T2>(seed1, exponent);
}

template<typename T1, typename T2>
Hash_CW2<T1,T2>::Hash_CW2(unsigned B, T2 seed0, T2 seed1, unsigned exponent)
{
    init(B, exponent, seed0, seed1);
}

template<typename T1, typename T2>
Hash_CW2<T1,T2>::Hash_CW2(unsigned B, unsigned exponent)
{
    init(B, exponent, random<T2>(exponent), random<T2>(exponent));
}

template<typename T1, typename T2>
Hash_CW2<T1,T2>::~Hash_CW2()
{
}

template<typename T1, typename T2>
unsigned Hash_CW2<T1,T2>::element(T1 j)
{
    T2 result = j;
    result = mersenne_modulus<T2>(result*seeds[1], mersenne_exponent) + seeds[0];
    result = mersenne_modulus<T2>(result, mersenne_exponent);
    T2 temp = mask;
    unsigned res = result & temp;
    return res;
}

template<typename T1, typename T2>
Hash<T1>* Hash_CW2<T1,T2>::copy()
{
    Hash_CW2<T1,T2> *result = new Hash_CW2<T1,T2>(mask+1, seeds[0], seeds[1], 
                                                    mersenne_exponent);
    return (Hash<T1>*) result;
}


/**************************Hash_CW4 implementation*****************************/
template<typename T1, typename T2>
void Hash_CW4<T1,T2>::init(unsigned B, unsigned exponent, T2 seed0, T2 seed1, 
                            T2 seed2, T2 seed3)
{
    mersenne_exponent = exponent;
    if (!isPowerOfTwo(B)) 
        throw std::domain_error("The number of buckets should be a power of 2");
    mask = B - 1;
    seeds[0] = mersenne_modulus<T2>(seed0, exponent);
    seeds[1] = mersenne_modulus<T2>(seed1, exponent);
    seeds[2] = mersenne_modulus<T2>(seed2, exponent);
    seeds[3] = mersenne_modulus<T2>(seed3, exponent);
}

template<typename T1, typename T2>
Hash_CW4<T1,T2>::Hash_CW4(unsigned B, T2 seed0, T2 seed1, T2 seed2, T2 seed3, 
                            unsigned exponent)
{
    init(B, exponent, seed0, seed1, seed2, seed3);
}

template<typename T1, typename T2>
Hash_CW4<T1,T2>::Hash_CW4(unsigned B, unsigned exponent)
{
    init(B, exponent, random<T2>(exponent), random<T2>(exponent), 
            random<T2>(exponent), random<T2>(exponent));
}

template<typename T1, typename T2>
Hash_CW4<T1,T2>::Hash_CW4(unsigned B, T2 *seeds, unsigned exponent)
{
    init(B, exponent, seeds[0], seeds[1], seeds[2], seeds[3]);
}

template<typename T1, typename T2>
Hash_CW4<T1,T2>::~Hash_CW4()
{
}

template<typename T1, typename T2>
unsigned Hash_CW4<T1,T2>::element(T1 j)
{
    T2 result;
    T2 exp1 = j;
    T2 exp2 = mersenne_modulus<T2>(exp1*exp1, mersenne_exponent);
    T2 exp3 = mersenne_modulus<T2>(exp2*exp1, mersenne_exponent);
    result = mersenne_modulus<T2>(exp3*seeds[3], mersenne_exponent) +
                mersenne_modulus<T2>(exp2*seeds[2], mersenne_exponent);
    result = mersenne_modulus<T2>(result, mersenne_exponent) +
                mersenne_modulus<T2>(exp1*seeds[1], mersenne_exponent);
    result = mersenne_modulus<T2>(result, mersenne_exponent) + seeds[0];
    result = mersenne_modulus<T2>(result, mersenne_exponent);
    T2 temp = mask;
    unsigned res = result & temp;
    return res;
}

template<typename T1, typename T2>
Hash<T1>* Hash_CW4<T1,T2>::copy()
{
    Hash_CW4<T1,T2> * result = new Hash_CW4<T1,T2>( mask+1, seeds, 
                                                    mersenne_exponent);
    return (Hash<T1>*) result;
}
