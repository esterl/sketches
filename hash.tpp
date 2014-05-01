#include "mersenne.h"
#include "random.h"

/**************************Hash_CW2 implementation*****************************/

template<typename T1, typename T2>
Hash_CW2<T1,T2>::Hash_CW2(T2 I1, T2 I2, unsigned int B)
{
    mersenne_exponent = get_mersenne_exponent<T1>();
    num_buckets = B;
    seeds[0] = mersenne_modulus<T1>(I1, mersenne_exponent);
    seeds[1] = mersenne_modulus<T1>(I2, mersenne_exponent);
}

template<typename T1, typename T2>
Hash_CW2<T1,T2>::Hash_CW2(unsigned int B)
{
    mersenne_exponent = get_mersenne_exponent<T1>();
    num_buckets = B;
    seeds[0] = mersenne_modulus<T1>(random<T2>(), mersenne_exponent);
    seeds[1] = mersenne_modulus<T1>(random<T2>(), mersenne_exponent);
}

template<typename T1, typename T2>
Hash_CW2<T1,T2>::~Hash_CW2()
{
}

template<typename T1, typename T2>
unsigned int Hash_CW2<T1,T2>::element(T1 j){
    T2 result = j;
    result = mersenne_modulus(result*seeds[1], mersenne_exponent) + seeds[0];
    result = mersenne_modulus(result, mersenne_exponent);
    T2 temp = num_buckets;
    unsigned int res = result % temp;
    return res;
}


/**************************Hash_CW4 implementation*****************************/

template<typename T1, typename T2>
Hash_CW4<T1,T2>::Hash_CW4(T2 I1, T2 I2, T2 I3, T2 I4, unsigned int B)
{
    mersenne_exponent = get_mersenne_exponent<T1>();
    num_buckets = B;
    seeds[0] = mersenne_modulus<T1>(I1, mersenne_exponent);
    seeds[1] = mersenne_modulus<T1>(I2, mersenne_exponent);
    seeds[2] = mersenne_modulus<T1>(I3, mersenne_exponent);
    seeds[3] = mersenne_modulus<T1>(I4, mersenne_exponent);
}

template<typename T1, typename T2>
Hash_CW4<T1,T2>::Hash_CW4(unsigned int B)
{
    mersenne_exponent = get_mersenne_exponent<T1>();
    num_buckets = B;
    seeds[0] = mersenne_modulus<T1>(random<T2>(), mersenne_exponent);
    seeds[1] = mersenne_modulus<T1>(random<T2>(), mersenne_exponent);
    seeds[2] = mersenne_modulus<T1>(random<T2>(), mersenne_exponent);
    seeds[3] = mersenne_modulus<T1>(random<T2>(), mersenne_exponent);
}

template<typename T1, typename T2>
Hash_CW4<T1,T2>::~Hash_CW4()
{
}

template<typename T1, typename T2>
unsigned int Hash_CW4<T1,T2>::element(T1 j)
{
    T2 result;
    T2 exp1 = j;
    T2 exp2 = mersenne_modulus<T2>(j*j, mersenne_exponent);
    T2 exp3 = mersenne_modulus<T2>(exp2*j, mersenne_exponent);
    result = mersenne_modulus<T2>(exp3*seeds[3], mersenne_exponent) +
                mersenne_modulus<T2>(exp2*seeds[2], mersenne_exponent);
    result = mersenne_modulus<T2>(result, mersenne_exponent) +
                mersenne_modulus<T2>(exp1*seeds[1], mersenne_exponent);
    result = mersenne_modulus<T2>(result, mersenne_exponent) + seeds[0];
    result = mersenne_modulus<T2>(result, mersenne_exponent);
    T2 temp = num_buckets;
    unsigned int res = result % temp;
    return res;
}

