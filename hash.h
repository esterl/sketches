#ifndef SKETCHES_HASH_H
#define SKETCHES_HASH_H

#include <stdint.h>
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
        
        virtual ~Hash(){};
};


/* Polynomials over a Merssenne prime (p) scheme using the Carter and Wegman 
   trick for generating 2-wise independent random variables from {B}. The type 
   T1 refers to the input space, whereas T2 refers to the space where the 
   polynomial operations take place. Should be at least twice the space of the
   used Mersenne prime.*/
template<typename T1, typename T2>
class Hash_CW2{
    protected:
        T2 seeds[2];
        unsigned int num_buckets;
        unsigned int mersenne_exponent;

    public:
        Hash_CW2(T2 I1, T2 I2, unsigned int B);
        Hash_CW2(unsigned int B);
        virtual ~Hash_CW2();

        virtual unsigned int element(T1 j);
};


/* Polynomials over a Merssenne prime (p) scheme using the Carter and Wegman 
   trick for generating 4-wise independent random variables from {B}.*/
template<typename T1, typename T2>
class Hash_CW4 {
    protected:
        T2 seeds[4];
        unsigned int num_buckets;
        unsigned int mersenne_exponent;

    public:
        Hash_CW4(T2 I1, T2 I2, T2 I3, T2 I4, unsigned int B);
        Hash_CW4(unsigned int B);
        virtual ~Hash_CW4();

        virtual unsigned int element(T1 j);
};


// TODO Tabulated Hashing.

#include "hash.tpp"

#endif // SKETCHES_HASH_H
