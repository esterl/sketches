#ifndef SKETCHES_HASH_H
#define SKETCHES_HASH_H

#include <stdint.h>

#include "mersenne.h"
/* Generating schemes for different types of {B} random variables (or hashes to
   the output space B={0..B-1}). B should be representable with an unsigned int.
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
    * operator unsigned int
    * operator ^=
    * operator &
    * operator *
    * operator %
    */
template<typename T>
class Hash
{
    public:
        virtual unsigned int element(T j) = 0;
        virtual Hash<T>* copy() {return NULL;};
        virtual ~Hash(){};
};


/* Polynomials over a Merssenne prime (p) scheme using the Carter and Wegman 
   trick for generating 2-wise independent random variables from {B}. The type 
   T1 refers to the input space, whereas T2 refers to the space where the 
   polynomial operations take place. Should be at least twice the space of the
   used Mersenne prime.*/
template<typename T1, typename T2>
class Hash_CW2: Hash<T1>{
    protected:
        T2 seeds[2];
        unsigned int num_buckets;
        unsigned int mersenne_exponent;

    public:
        Hash_CW2(T2 I1, T2 I2, unsigned int B);
        Hash_CW2(unsigned int B);

        virtual unsigned int element(T1 j);
        virtual Hash<T1>* copy();
        
        virtual ~Hash_CW2();
};


/* Polynomials over a Merssenne prime (p) scheme using the Carter and Wegman 
   trick for generating 4-wise independent random variables from {B}.*/
template<typename T1, typename T2>
class Hash_CW4: Hash<T1> {
    protected:
        T2 seeds[4];
        unsigned int num_buckets;
        unsigned int mersenne_exponent;

    public:
        Hash_CW4(T2 I1, T2 I2, T2 I3, T2 I4, unsigned int B);
        Hash_CW4(unsigned int B);

        virtual unsigned int element(T1 j);
        virtual Hash<T1>* copy();

        virtual ~Hash_CW4();
};

/* Tabulated Hashing as presented by Mikkel Thorup and Yin Zhang in "Tabulation
   Based 4-Universal Hashing with Applications to Second Moment Estimation" for
   generating 4-wise independent random variables from {B}.*/
template<typename T>
class Hash_Tab: Hash<T>{
    protected:
        unsigned int num_buckets;

    public:
        Hash_Tab(unsigned B);

        virtual unsigned int element(T j);
        virtual Hash<T>* copy();
        
        virtual ~Hash_Tab();
};

template<>
class Hash_Tab<uint16_t>: Hash<uint16_t>{
    protected:
        unsigned int num_buckets;
        uint16_t* table;

    public:
        Hash_Tab(uint64_t I1, uint64_t I2, uint64_t I3, uint64_t I4, unsigned int B);
        Hash_Tab(unsigned B);
        Hash_Tab();
        
        virtual unsigned int element(uint16_t j);
        virtual Hash<uint16_t>* copy();
        
        virtual ~Hash_Tab();
};

template<>
class Hash_Tab<uint32_t>: Hash<uint32_t>{
    protected:
        unsigned int num_buckets;
        uint16_t *T0, *T1, *T2;

    public:
        Hash_Tab(prime61_t* seeds0, prime61_t* seeds1, prime61_t* seeds2, unsigned int B);
        Hash_Tab(unsigned B);
        Hash_Tab();
        
        virtual unsigned int element(uint32_t j);
        virtual Hash<uint32_t>* copy();
        
        virtual ~Hash_Tab();
};

template<>
class Hash_Tab<uint64_t>: Hash<uint64_t>{
    protected:
        unsigned int num_buckets;
        uint16_t *T0, *T1, *T2, *T3, *T4, *T5, *T6;
        uint64_t **cauchy;

    public:
        Hash_Tab(prime61_t* seeds0, prime61_t* seeds1, prime61_t* seeds2, 
                    prime61_t* seeds3, prime61_t* seeds4, prime61_t* seeds5, 
                    prime61_t* seeds6, unsigned int B);
        Hash_Tab(unsigned B);
        Hash_Tab();
        
        virtual unsigned int element(uint64_t j);
        virtual Hash<uint64_t>* copy();
        
        virtual ~Hash_Tab();
};

template<>
class Hash_Tab<uint128>: Hash<uint128>{
    protected:
        unsigned int num_buckets;

    public:
        Hash_Tab(unsigned B){ num_buckets = B;};
        Hash_Tab(){};
        
        unsigned int element(uint128 j){ return 0;};
        Hash<uint128>* copy(){ };
        
        ~Hash_Tab(){};
};
#include "hash.tpp"

#endif // SKETCHES_HASH_H
