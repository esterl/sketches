#ifndef SKETCHES_SKETCHES_H
#define SKETCHES_SKETCHES_H

#include <algorithm>

#include "xis.h"
#include "hash.h"

/* Implementation of different sketches for estimating size of joins and
 self-join; as well as the L2 difference between two data-streams.
 Based on the implementation by F. Rusu at:
 http://www.cise.ufl.edu/~frusu/code.html
 For details on all the sketches implemented here, see the paper:
 1) "Statistical Analysis of Sketch Estimators" by F. Rusu and A. Dobra */

// TODO move implementations to tpp
typedef double (*fptr)(double*, unsigned int);

template<typename T>
class Sketch {
protected:
    unsigned int num_rows;
    unsigned int num_cols;
    double *sketch_elem;
    fptr average_function;

public:
    unsigned int get_key_size() {
        return sizeof(T) * 8;
    }
    unsigned int get_num_rows() {
        return num_rows;
    }
    unsigned int get_num_columns() {
        return num_cols;
    }
    double* get_counters() {
        return sketch_elem;
    }
    double get_max() {
        return *std::max_element(sketch_elem, sketch_elem + num_rows * num_cols);
    }
    double get_min() {
        return *std::min_element(sketch_elem, sketch_elem + num_rows * num_cols);
    }
    void clear() {
        for (int i = 0; i < num_rows * num_cols; i++)
            sketch_elem[i] = 0.0;
    }
    Sketch<T>& operator+=(const Sketch<T>& other) {
        for (int i = 0; i < num_rows * num_cols; i++)
            sketch_elem[i] += other.sketch_elem[i];
        return *this;
    }
    void operator-=(const Sketch<T>& other) {
        for (int i = 0; i < num_rows * num_cols; i++)
            sketch_elem[i] -= other.sketch_elem[i];
    }

    virtual ~Sketch() {
        delete[] sketch_elem;
    }

    //updating the sketch with the value corresponding to the given key
    virtual void update(T key, double weight) = 0;

    //estimating the size of join of two sketches
    virtual double inner_join(Sketch<T> *other) = 0;

    //estimating the self-join size or the second frequency moment
    virtual double second_moment() = 0;

    //estimating the first frequency moment
    virtual double first_moment() = 0;

    //estimating the L2 difference
    virtual Sketch* difference(Sketch *other) = 0;

    //copy Sketch
    virtual Sketch* copy() = 0;
    virtual double get_bytes() { return sizeof(double) * num_rows * num_cols; };
    virtual double get_optimized_bits() = 0;
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
class AGMS_Sketch: public Sketch<T> {
protected:
    Xi<T> **xis;

public:
    AGMS_Sketch(unsigned int num_cols, unsigned int num_rows, Xi<T> **xis,
            const char *avg_func = "median");
    AGMS_Sketch(unsigned int num_cols, unsigned int num_rows,
            const char *xis_type = "eh3", const char *avg_func = "median");
    AGMS_Sketch(AGMS_Sketch<T>* copy);
    virtual ~AGMS_Sketch();

    virtual void update(T key, double weight);
    virtual double inner_join(Sketch<T> *other);
    virtual double second_moment();
    virtual double first_moment();
    virtual Sketch<T>* difference(Sketch<T> *other);
    virtual Sketch<T>* copy();
    virtual double get_optimized_bits();
};

/* Fast-AGMS sketches proposed in the paper:
 1) "Sketching streams through the net: distributed approximate query
 tracking" by G. Cormode and M. Garofalakis
 num_rows basic estimators are computed by hasing the key values to an array
 with num_buckets elements. Then the median of the num_rows estimators is
 returned as the final estimator.
 * sketch_elem is an array of num_rows * num_buckets counters.
 * hashes is an array of num_rows 2-way hashing functions that determine which
 bucket will be updated on each row.
 * xis is an array of num_rows +/-1 4-way random variable that determines
 the value used to update the bucket.
 In general, when the sketch needs to be updated with a pair (key,weight),
 for every row the basic estimator selected by hashes(key) is updated adding
 xis(key)*weight. */
template<typename T>
class FAGMS_Sketch: public Sketch<T> {
protected:
    Hash<T> **hashes;
    Xi<T> **xis;

public:
    FAGMS_Sketch(unsigned int num_buckets, unsigned int num_rows,
            Hash<T> **hashes, Xi<T> **xis, const char *avg_func = "median");
    FAGMS_Sketch(unsigned int num_buckets, unsigned int num_rows,
            const char *hash_func, const char* xis_type, const char *avg_func =
                    "median");
    FAGMS_Sketch(FAGMS_Sketch<T> *copy);
    virtual ~FAGMS_Sketch();

    virtual void update(T key, double weight);
    virtual double inner_join(Sketch<T> *other);
    virtual double second_moment();
    virtual double first_moment();
    virtual Sketch<T>* difference(Sketch<T> *other);
    virtual Sketch<T>* copy();
    virtual double get_optimized_bits();
};

/* Fast-Count sketches proposed in the paper:
 1) "Tabulation-based 4-universal hashing with applications to second moment
 estimation" by M. Thorup and Y. Zhang
 num_rows basic estimators are computed by hashing the key values to an array
 with num_buckets elements. Then the average of the num_rows estimators is
 returned as teh final estimator.
 * sketch_elem is an array of num_rows*num_buckets counters.
 * hashes is an array of 4-way hash function in the {num_buckets} space.
 In general, when the sketch needs to be updated with a pair (key,weight),
 for every row the basic estimator selected by hashes(key) is updated adding
 the weight. */

template<typename T>
class FastCount_Sketch: public Sketch<T> {
protected:
    Hash<T> **hashes;

public:
    FastCount_Sketch(unsigned int num_buckets, unsigned int num_rows,
            Hash<T> **hashes, const char *avg_func = "mean");
    FastCount_Sketch(unsigned int num_buckets, unsigned int num_rows,
            const char* hash_func, const char *avg_func = "mean");
    FastCount_Sketch(FastCount_Sketch<T> *copy);
    virtual ~FastCount_Sketch();

    virtual void update(T key, double weight);
    virtual double inner_join(Sketch<T> *other);
    virtual double second_moment();
    virtual double first_moment();
    virtual Sketch<T>* difference(Sketch<T> *other);
    virtual Sketch<T>* copy();
    virtual double get_optimized_bits();
};

/* Count-Min sketches proposed in the paper:
 1) "An improved data stream summary: the count-min sketch and its
 applications" by G. Cormode and S. Muthukrishnan
 num_rows basic estimators are computed by hashing the key values to an array
 with num_buckets elements. Then the minimum of the num_rows estimators is
 returned as the final estimator.
 * sketch_elem is an array of num_rows * num_buckets counters.
 * hashes is an array of num_rows 2-way hash functions in the space of
 {num_buckets}
 In general, when the sketch needs to be updated with a pair (key,weight),
 for every row the basic estimator selected by hashes(key) is updated adding
 the weight. */
template<typename T>
class CountMin_Sketch: public Sketch<T> {
protected:
    Hash<T> **hashes;

public:
    CountMin_Sketch(unsigned int num_buckets, unsigned int num_rows,
            Hash<T> **hashes);
    CountMin_Sketch(unsigned int num_buckets, unsigned int num_rows,
            const char* hash_func);
    CountMin_Sketch(CountMin_Sketch<T> *copy);
    virtual ~CountMin_Sketch();

    virtual void update(T key, double weight);
    virtual double inner_join(Sketch<T> *other);
    virtual double second_moment();
    virtual double first_moment();
    virtual Sketch<T>* difference(Sketch<T> *other);
    virtual Sketch<T>* copy();
    virtual double get_optimized_bits();
};

#include "sketches.tpp"
#endif //SKETCHES_SKETCHES_H
