#include "mersenne.h"
#include "random.h"

inline bool isPowerOfTwo(unsigned x)
{
  return ((x != 0) && !(x & (x - 1)));
}
/**************************Hash_CW2 implementation*****************************/

template<typename T1, typename T2>
void Hash_CW2<T1,T2>::init(unsigned B, T2 seed0, T2 seed1)
{
    mersenne_exponent = get_mersenne_exponent<T1>();
    if (!isPowerOfTwo(B)) 
        throw std::domain_error("The number of buckets should be a power of 2");
    mask = B - 1;
    seeds[0] = mersenne_modulus<T2>(seed0, mersenne_exponent);
    seeds[1] = mersenne_modulus<T2>(seed1, mersenne_exponent);
}

template<typename T1, typename T2>
Hash_CW2<T1,T2>::Hash_CW2(unsigned B, T2 seed0, T2 seed1)
{
    init(B, seed0, seed1);
}

template<typename T1, typename T2>
Hash_CW2<T1,T2>::Hash_CW2(unsigned B)
{
    mersenne_exponent = get_mersenne_exponent<T1>();
    init(B, random<T2>(mersenne_exponent), random<T2>(mersenne_exponent));
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
    Hash_CW2<T1,T2> *result = new Hash_CW2<T1,T2>(mask+1, seeds[0], seeds[1]);
    return (Hash<T1>*) result;
}


/**************************Hash_CW4 implementation*****************************/
template<typename T1, typename T2>
void Hash_CW4<T1,T2>::init(unsigned B, T2 seed0, T2 seed1, T2 seed2, T2 seed3)
{
    mersenne_exponent = get_mersenne_exponent<T1>();
    if (!isPowerOfTwo(B)) 
        throw std::domain_error("The number of buckets should be a power of 2");
    mask = B - 1;
    seeds[0] = mersenne_modulus<T2>(seed0, mersenne_exponent);
    seeds[1] = mersenne_modulus<T2>(seed1, mersenne_exponent);
    seeds[2] = mersenne_modulus<T2>(seed2, mersenne_exponent);
    seeds[3] = mersenne_modulus<T2>(seed3, mersenne_exponent);
}

template<typename T1, typename T2>
Hash_CW4<T1,T2>::Hash_CW4(unsigned B, T2 seed0, T2 seed1, T2 seed2, T2 seed3)
{
    init(B, seed0, seed1, seed2, seed3);
}

template<typename T1, typename T2>
Hash_CW4<T1,T2>::Hash_CW4(unsigned B)
{
    mersenne_exponent = get_mersenne_exponent<T1>();
    init(B, random<T2>(mersenne_exponent), random<T2>(mersenne_exponent), 
            random<T2>(mersenne_exponent), random<T2>(mersenne_exponent));
}

template<typename T1, typename T2>
Hash_CW4<T1,T2>::Hash_CW4(unsigned B, T2 *seeds)
{
    init(B,  seeds[0], seeds[1], seeds[2], seeds[3]);
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
    Hash_CW4<T1,T2> * result = new Hash_CW4<T1,T2>( mask+1, seeds[0], seeds[1], 
                                                    seeds[2], seeds[3]);
    return (Hash<T1>*) result;
}
