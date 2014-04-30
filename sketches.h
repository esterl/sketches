#ifndef SKETCHES_SKETCHES_H
#define SKETCHES_SKETCHES_H

#include "xis.h"
#include "hash.h"

/* Implementation of different sketches for stimating size of joins and 
   self-join; as well as the L2 difference between two data-streams.
   Based on the implementation by F. Rusu at:
   http://www.cise.ufl.edu/~frusu/code.html
   For details on all the sketches implemented here, see the paper:
    1) "Statistical Analysis of Sketch Estimators" by F. Rusu and A. Dobra */

template<typename T>
class Sketch
{
    public:
        //reseting the sketch structure
        virtual void clear() = 0;

        //updating the sketch with the value corresponding to the given key
        virtual void update_sketch(T key, double weight) = 0;

        //estimating the size of join of two sketches
        virtual double inner_product(Sketch *other) = 0;

        //estimating the self-join size or the second frequency moment
        virtual double second_moment() = 0;
        
        //estimating the L2 difference
        virtual double difference(Sketch *other)=0;
};



/* AGMS sketch, as proposed in the paper:
    * "Tracking join and self-join sizes in limited storage" by N. Alon et al.
   An array of num_rows*num_cols basic sketch estimators of size num_cols is 
   used to compute the improved estimator. Estimators from the same row are 
   averaged, and the median of the num_rows average estimators is returned as 
   the final estimate.
   * sketch_elem is the array of basic estimators.
   * xis is an array of +/-1 random variable generators. A random variable is 
     attached to each basic sketch estimator (num_rows*num_cols) random 
     variables.*/
template<typename T>
class AGMS_Sketch : public Sketch<T>
{
    protected:
        unsigned int num_rows;
        unsigned int num_cols;

        double *sketch_elem;

        Xi<T> **xis;


    public:
        AGMS_Sketch(unsigned int num_rows, unsigned int num_rows, Xi **xis);
        virtual ~AGMS_Sketch();

        virtual void clear();
        virtual void update_sketch(T key, double weight);
        virtual double inner_product(Sketch *other);
        virtual double second_moment();
        virtual double difference(Sketch *other);
};

#include "sketches.tpp"
#endif //SKETCHES_SKETCHES_H
