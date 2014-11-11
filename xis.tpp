/* Implementation of "xis.h" */
//#include "xis2.h"
#include <stdexcept>

#include "ttmath/ttmath.h"
#include "mersenne.h"
#include "random.h"



/*
  Computes parity bit of the bits of an integer
*/
template<typename T>
inline unsigned int seq_xor(T x) 
{
    for (int i = sizeof(T)*8/2; i >= 1; i = i/2){
        x ^= (x >> i);
    }
    T temp = 1U;
    return x & temp;
}

/**************************Xi_BCH3 implementation******************************/

template<typename T>
Xi_BCH3<T>::Xi_BCH3(T I1, T I2){
    T temp = 1UL;
    seeds[0] = I1 & temp;
    seeds[1] = I2;
}

template<typename T>
Xi_BCH3<T>::Xi_BCH3(){
    T temp = 1UL;
    seeds[0] = random<T>() & temp;
    seeds[1] = random<T>();
}

template<typename T>
Xi_BCH3<T>::~Xi_BCH3()
{
}

template<typename T>
int Xi_BCH3<T>::element(T j)
{
    unsigned int res = int(seeds[0]) ^ seq_xor(seeds[1]&j);
    if (res == 0) return -1;
    return 1;
}


template<typename T>
Xi<T>* Xi_BCH3<T>::copy(){
    Xi_BCH3<T> *result = new Xi_BCH3(seeds[0], seeds[1]);
    return (Xi<T>*) result;
}

/**************************Xi_EH3 implementation*******************************/

template<typename T>
Xi_EH3<T>::Xi_EH3(T I1, T I2)
{
    T temp = 1UL;
    seeds[0] = I1 & temp;
    seeds[1] = I2;
}

template<typename T>
Xi_EH3<T>::Xi_EH3()
{
    T temp = 1UL;
    seeds[0] = random<T>() & temp;
    seeds[1] = random<T>();
}

template<typename T>
Xi_EH3<T>::~Xi_EH3()
{
}

template<typename T>
int Xi_EH3<T>::element(T j)
{
    uint64_t maskA = 0xAAAAAAAAAAAAAAAA;
    uint64_t mask32 = 0xFFFFFFFFFFFFFFFF;
    unsigned int p_res = 0;
    T temp = j;
    for (int i = 0; i <= sizeof(T)*8/64; i++){
        T current = temp & (T) mask32;
        p_res ^= seq_xor((seeds[1]&current) ^ (current & (current<<1) & (T) maskA));
        temp = temp >> 64;
    }
    unsigned int res = int(seeds[0]) ^ p_res;
    if (res == 0) return -1;
    return 1;
} 

template<typename T>
Xi<T>* Xi_EH3<T>::copy()
{
    Xi_EH3<T> * result = new Xi_EH3<T>(seeds[0], seeds[1]);
    return (Xi<T>*) result;
}

/**************************Xi_CW2 implementation*******************************/

template<typename T1, typename T2>
Xi_CW2<T1,T2>::Xi_CW2(T2 I1,T2 I2)
{
    // Decide which Mersenne prime to use.
    mersenne_exponent = get_mersenne_exponent<T1>();
    seeds[0] = mersenne_modulus<T2>(I1, mersenne_exponent);
    seeds[1] = mersenne_modulus<T2>(I2, mersenne_exponent);
}

template<typename T1, typename T2>
Xi_CW2<T1,T2>::Xi_CW2()
{
    // Decide which Mersenne prime to use.
    mersenne_exponent = get_mersenne_exponent<T1>();
    seeds[0] = mersenne_modulus<T2>(random<T2>(), mersenne_exponent);
    seeds[1] = mersenne_modulus<T2>(random<T2>(), mersenne_exponent);
}

template<typename T1, typename T2>
Xi_CW2<T1,T2>::~Xi_CW2()
{
}

template<typename T1, typename T2>
int Xi_CW2<T1,T2>::element(T1 j)
{
    T2 result = j;
    result = mersenne_modulus<T2>(result*seeds[1], mersenne_exponent) + seeds[0];
    result = mersenne_modulus<T2>(result, mersenne_exponent);
    T2 temp = 1U;
    int res = result & temp;
    if (res == 0) return -1;
    return 1; 
}

template<typename T1, typename T2>
Xi<T1>* Xi_CW2<T1,T2>::copy()
{
    Xi_CW2<T1,T2> * result = new Xi_CW2<T1,T2>(seeds[0], seeds[1]);
    return (Xi<T1>*) result;
}


/**************************Xi_CW2 implementation*******************************/

template<typename T1, typename T2>
Xi_CW4<T1,T2>::Xi_CW4(T2 I1, T2 I2, T2 I3, T2 I4)
{
    // Decide which Mersenne prime to use
    mersenne_exponent = get_mersenne_exponent<T1>();
    seeds[0] = mersenne_modulus<T2>(I1, mersenne_exponent);
    seeds[1] = mersenne_modulus<T2>(I2, mersenne_exponent);
    seeds[2] = mersenne_modulus<T2>(I3, mersenne_exponent);
    seeds[3] = mersenne_modulus<T2>(I4, mersenne_exponent);
}

template<typename T1, typename T2>
Xi_CW4<T1,T2>::Xi_CW4()
{
    // Decide which Mersenne prime to use
    mersenne_exponent = get_mersenne_exponent<T1>();
    seeds[0] = mersenne_modulus<T2>(random<T2>(), mersenne_exponent);
    seeds[1] = mersenne_modulus<T2>(random<T2>(), mersenne_exponent);
    seeds[2] = mersenne_modulus<T2>(random<T2>(), mersenne_exponent);
    seeds[3] = mersenne_modulus<T2>(random<T2>(), mersenne_exponent);
}

template<typename T1, typename T2>
Xi_CW4<T1,T2>::~Xi_CW4()
{
}

template<typename T1, typename T2>
int Xi_CW4<T1,T2>::element(T1 j)
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
    T2 temp = 1U;
    int res = result & temp;
    if (res == 0) return -1;
    return 1;
}

template<typename T1, typename T2>
Xi<T1>* Xi_CW4<T1,T2>::copy()
{
    Xi_CW4<T1,T2> * result = new Xi_CW4<T1,T2>(seeds[0], seeds[1], seeds[2], 
                                                seeds[3]);
    return (Xi<T1>*) result;
}

/**************************Xi_BCH3 implementation******************************/

template<typename T>
Xi_BCH5<T>::Xi_BCH5(T I1, T I2, T I3){
    T temp = 1U;
    seeds[0] = I1&temp;
    seeds[1] = I2;
    seeds[2] = I3;
}

template<typename T>
Xi_BCH5<T>::Xi_BCH5(){
    T temp = 1U;
    seeds[0] = random<T>()&temp;
    seeds[1] = random<T>();
    seeds[2] = random<T>();
}

template<typename T>
Xi_BCH5<T>::~Xi_BCH5()
{
}

template<typename T>
int Xi_BCH5<T>::element(T j){
    T p_res = (seeds[1]&j) ^ (seeds[2] & (j*j*j));
    unsigned int res = int(seeds[0]) ^ seq_xor(p_res);
    if (res == 0) return -1;
    return 1;
}

template<typename T>
Xi<T>* Xi_BCH5<T>::copy(){
    Xi_BCH5<T> *result = new Xi_BCH5(seeds[0], seeds[1], seeds[2]);
    return (Xi<T>*) result;
}
