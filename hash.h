#ifndef SKETCHES_HASH_H
#define SKETCHES_HASH_H

#include <stdint.h>

#include "mersenne.h"
#include "cauchy.h"
/* Generating schemes for different types of {B} random variables (or hashes to
 the output space B={0..B-1}). B should be representable with an unsigned.
 Based on the implementation by F. Rusu at:
 http://www.cise.ufl.edu/~frusu/code.html
 More details can be found on:
 1) "Fast Range-Summable Random Variables for Efficient Aggregate Estimation"
 by F. Rusu and A. Dobra
 2) "Pseudo-Random Number Generation for Sketch-Based Estimations" by F. Rusu
 and A. Dobra. */

/* Base class that should be extended by every hash. The type T represents the 
 type of the variables on the input space (I). The following
 operations should be defined for T:
 * operator+
 * operator>>
 * operator unsigned
 * operator ^=
 * operator &
 * operator *
 * operator %
 */
template<typename T>
class Hash {
public:
    virtual unsigned element(T j) = 0;
    virtual Hash<T>* copy() {
        return NULL;
    }
    ;
    virtual ~Hash() {
    }
    ;
};

/* Polynomials over a Merssenne prime (p) scheme using the Carter and Wegman
 trick for generating 2-wise independent random variables from {B}. The type
 T1 refers to the input space, whereas T2 refers to the space where the
 polynomial operations take place. Should be at least twice the space of the
 used Mersenne prime.*/
template<typename T1, typename T2 = hash_type_for_t<T1>>
//template<typename T1, typename T2>
class Hash_CW2: public Hash<T1> {
protected:
    T2 seeds[2];
    unsigned mask;
    unsigned mersenne_exponent;
    void init(unsigned B, T2 seed0, T2 seed1);

public:
    Hash_CW2(unsigned B, T2 seed0, T2 seed1);
    Hash_CW2(unsigned B);

    virtual unsigned element(T1 j);
    virtual Hash<T1>* copy();

    virtual ~Hash_CW2();
};

/* Polynomials over a Merssenne prime (p) scheme using the Carter and Wegman 
 trick for generating 4-wise independent random variables from {B}.*/
template<typename T1, typename T2 = hash_type_for_t<T1>>
//template<typename T1, typename T2>
class Hash_CW4: public Hash<T1> {
protected:
    T2 seeds[4];
    unsigned mask;
    unsigned mersenne_exponent;
    void init(unsigned B, T2 seed0, T2 seed1, T2 seed2, T2 seed3);

public:
    Hash_CW4(unsigned B, T2 seed0, T2 seed1, T2 seed2, T2 seed3);
    Hash_CW4(unsigned B);
    Hash_CW4(unsigned B, T2 *seeds);

    virtual unsigned element(T1 j);
    virtual Hash<T1>* copy();

    virtual ~Hash_CW4();
};

/* Tabulated Hashing as presented by Mikkel Thorup and Yin Zhang in "Tabulation
 Based 4-Universal Hashing with Applications to Second Moment Estimation" for
 generating 4-wise independent random variables from {B}. We expect B to be a
 power of 2 below 2^16, so that the pre-computed tables can be kept smaller.*/
template<typename T>
class Hash_Tab: public Hash<T> {
public:
    Hash_Tab(unsigned B);

    virtual unsigned element(T j);
    virtual Hash<T>* copy();

    virtual ~Hash_Tab();
};

template<>
class Hash_Tab<uint8_t> : public Hash<uint8_t> {
protected:
    uint16_t * table;
    void init(unsigned B, prime13_t seed0, prime13_t seed1, prime13_t seed2,
            prime13_t seed3);

public:
    Hash_Tab(unsigned B, prime13_t seed0, prime13_t seed1, prime13_t seed2,
            prime13_t seed3);
    Hash_Tab(unsigned B);
    Hash_Tab();

    virtual unsigned element(uint8_t j);
    virtual Hash<uint8_t>* copy();

    virtual ~Hash_Tab();
};

template<>
class Hash_Tab<uint16_t> : public Hash<uint16_t> {
protected:
    uint16_t* table;
    void init(unsigned B, prime17_t s0, prime17_t s1, prime17_t s2,
            prime17_t s3);

public:
    Hash_Tab(unsigned B, prime17_t s0, prime17_t s1, prime17_t s2,
            prime17_t s3);
    Hash_Tab(unsigned B);
    Hash_Tab();

    virtual unsigned element(uint16_t j);
    virtual Hash<uint16_t>* copy();

    virtual ~Hash_Tab();
};

template<>
class Hash_Tab<uint32_t> : public Hash<uint32_t> {
protected:
    unsigned mask;
    uint16_t *T0, *T1, *T2;
    void init(unsigned B, prime31_t* s0, prime31_t* s1, prime31_t* s2);

public:
    Hash_Tab(unsigned B, prime31_t* s0, prime31_t* s1, prime31_t* s2);
    Hash_Tab(unsigned B);
    Hash_Tab();

    virtual unsigned element(uint32_t j);
    virtual Hash<uint32_t>* copy();

    virtual ~Hash_Tab();
};

template<>
class Hash_Tab<uint64_t> : public Hash<uint64_t> {
protected:
    unsigned mask;
    uint16_t *T0, *T1, *T2, *T3, *T4, *T5, *T6;
    uint64_t **cauchy;
    uint64_t** getCauchy();

public:
    Hash_Tab(unsigned B, prime31_t* seeds0, prime31_t* seeds1,
            prime31_t* seeds2, prime31_t* seeds3, prime31_t* seeds4,
            prime31_t* seeds5, prime31_t* seeds6);
    Hash_Tab(unsigned B);
    Hash_Tab();

    virtual unsigned element(uint64_t j);
    virtual Hash<uint64_t>* copy();

    virtual ~Hash_Tab();
};

template<>
class Hash_Tab<uint128_t> : public Hash<uint128_t> {
protected:
    uint16_t **T;
    unsigned mask;
    CauchyRow* cauchy;
    CauchyRow* getCauchy();

public:
    Hash_Tab(unsigned B, prime31_t** seeds = NULL);
    Hash_Tab() {
    }
    ;

    unsigned element(uint128_t j);
    Hash<uint128_t>* copy();

    ~Hash_Tab();
};

#include "hash.tpp"

#endif // SKETCHES_HASH_H
