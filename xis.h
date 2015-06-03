#ifndef SKETCHES_XIS_H
#define SKETCHES_XIS_H

#include <stdint.h>
#include "mersenne.h"
/* Generating schemes for different types of  +/- 1 random variables.
   Based on the implementation by F. Rusu at:
   http://www.cise.ufl.edu/~frusu/code.html
   More details can be found on:
    1) "Fast Range-Summable Random Variables for Efficient Aggregate Estimation"
       by F. Rusu and A. Dobra
    2) "Pseudo-Random Number Generation for Sketch-Based Estimations" by F. Rusu
       and A. Dobra. */

/* Base class that every +/-1 random variable should extend. The type T 
   represents the type of the variables on the input space (I). The following 
   operations should be defined for T: 
    * operator+
    * operator>>
    * operator unsigned int
    * operator ^=
    * operator &
    * operator *
    */
template<typename T>
class Xi
{
    public:
        virtual int element(T j) = 0;
        virtual Xi<T>* copy(){};
        
        virtual ~Xi(){};
};


/* BCH Scheme for generating +/-1 3-wise independent random variables.*/
template <typename T>
class Xi_BCH3: public Xi<T> {
    protected:
        T seeds[2];

    public:
        Xi_BCH3(T I1, T I2);
        //Random constructor:
        Xi_BCH3();

        virtual int element(T j);
        virtual Xi<T>* copy();
        virtual ~Xi_BCH3();
};


/* Extended Hamming Scheme for generating +/-1 3-wise independent random 
   variables.*/
template <typename T>
class Xi_EH3: public Xi<T> {
    protected:
        T seeds[2];

    public:
        Xi_EH3(T I1, T I2);
        Xi_EH3();

        virtual int element(T j);
        virtual Xi<T>* copy();
        virtual ~Xi_EH3();
};


/* Polynomials over a Merssenne prime (p) scheme using the Carter and Wegman 
   trick for generating +/-1 2-wise independent random variables. The type T1 
   refers to the input space, whereas T2 refers to the space where the 
   polynomial operations take place. Should be at least twice the space of the
   used Mersenne prime.*/
template<typename T1, typename T2 = hash_type_for_t<T1>>
class Xi_CW2 : public Xi<T1>{
    protected:
        T2 seeds[2];
        int mersenne_exponent;

    public:
        Xi_CW2(T2 I1,T2 I2);
        Xi_CW2();

        virtual int element(T1 j);
        virtual Xi<T1>* copy();
        virtual ~Xi_CW2();
};

/* As Xi_CW2 but for implementing +/-1 4-wise independent random variables. */
// TODO CW2 and CW4 could be implemented as templates of the integer k.
template <typename T1, typename T2 = hash_type_for_t<T1>>
class Xi_CW4 : public Xi<T1> {
    protected:
        T2 seeds[4];
        int mersenne_exponent;

    public:
        Xi_CW4(T2 I1, T2 I2, T2 I3, T2 I4);
        Xi_CW4();

        virtual int element(T1 j);
        virtual Xi<T1>* copy();
        virtual ~Xi_CW4();
};

/* BCH Scheme for generating +/-1 5-wise independent random variables.*/
template<typename T>
class Xi_BCH5 : public Xi<T> {
    protected:
        T seeds[3];

    public:
        Xi_BCH5(T I1, T I2, T I3);
        Xi_BCH5();

        virtual int element(T j);
        virtual Xi<T>* copy();
        virtual ~Xi_BCH5();
};


#include "xis.tpp"

#endif // SKETCHES_XIS_H
