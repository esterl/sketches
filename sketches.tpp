#include "sketches.h"

#include <string.h>
#include <cmath>        // std::abs
#include <stdexcept>    // std::invalid_argument

void print(double * values, unsigned int buckets, unsigned int rows) {
    std::cout << std::endl;
    for (unsigned int i = 0; i < rows; i++) {
        for (unsigned int j = i * buckets; j < (i + 1) * buckets; j++) {
            std::cout << values[j] << " ";
        }
        std::cout << std::endl;
    }
}

/************************** Auxiliary functions *******************************/
double mean(double* values, unsigned int n) {
    double sum = 0;
    for (unsigned int i = 0; i < n; i++)
        sum += values[i];
    return sum / (double) n;
}

double trimmean(double* values, unsigned int n) {
    double trim = 0.05;
    double *aux = new double[n];
    std::sort(aux, aux + n);

    unsigned int start = std::round(trim * n);
    unsigned int size = n - 2 * start;
    double result = mean(values + start, size);
    delete[] aux;
    return result;
}

double median(double* values, unsigned int n) {
    if (n == 1)
        return values[0];

    if (n == 2)
        return (values[0] + values[1]) / 2.;

    double *aux = new double[n];
    for (unsigned int i = 0; i < n; i++)
        aux[i] = values[i];

    std::sort(aux, aux + n);

    double res;
    if (n % 2 == 0)
        res = (aux[n / 2 - 1] + aux[n / 2]) / 2.;
    else
        res = aux[n / 2];

    delete[] aux;
    return res;
}

fptr get_average_function(const char *name) {
    if (strcmp(name, "mean") == 0) {
        return (*mean);
    } else if (strcmp(name, "trimmean") == 0) {
        return (*trimmean);
    } else if (strcmp(name, "median") == 0) {
        return (*median);
    } else {
        throw std::invalid_argument("Unknown average function");
    }
}

inline double min(double* values, unsigned int n) {
    double min = values[0];
    for (unsigned int i = 1; i < n; i++)
        if (values[i] < min)
            min = values[i];

    return min;
}

template<typename T>
Xi<T>* get_random_xi(const char* generator) {
    Xi<T> *xi;
    if (strcmp(generator, "bch3")==0){
        xi = new Xi_BCH3<T>();
    } else if(strcmp(generator, "bch5")==0){
        xi = new Xi_BCH5<T>();
    }else if(strcmp(generator, "eh3")==0){
        xi = new Xi_EH3<T>();
    }else if(strcmp(generator, "cw2")==0){
        xi = new Xi_CW2<T>();
    }else if(strcmp(generator, "cw4")==0){
        xi = new Xi_CW4<T>();
    }
    return xi;
}

template<typename T>
Hash<T>* get_random_hash(const char* hash_func, unsigned buckets) {
    Hash<T> *hash;
    if(strcmp(hash_func,"cw2")==0){
        hash = new Hash_CW2<T>(buckets);
    }else if (strcmp(hash_func, "cw4")==0){
        hash = new Hash_CW4<T>(buckets);
    }else if (strcmp(hash_func, "tab")==0){
        hash = new Hash_Tab<T>(buckets);
    }
    return hash;
}

/**************************AGMS_Sketch implementation**************************/
template<typename T>
AGMS_Sketch<T>::AGMS_Sketch(unsigned int cols, unsigned int rows, Xi<T> **xis,
        const char* avg_func) {
    this->num_rows = rows;
    this->num_cols = cols;
    this->average_function = get_average_function(avg_func);

    this->xis = new Xi<T>*[rows * cols];
    this->sketch_elem = new double[rows * cols];
    for (int i = 0; i < rows * cols; i++) {
        this->sketch_elem[i] = 0.0;
        this->xis[i] = xis[i]->copy();
    }
}

template<typename T>
AGMS_Sketch<T>::AGMS_Sketch(unsigned int cols, unsigned int rows,
        const char *xis_type, const char *avg_func) {
    this->num_rows = rows;
    this->num_cols = cols;
    this->average_function = get_average_function(avg_func);

    this->xis = new Xi<T>*[rows * cols];
    this->sketch_elem = new double[rows * cols];
    for (int i = 0; i < rows * cols; i++) {
        this->sketch_elem[i] = 0.0;
        this->xis[i] = get_random_xi<T>(xis_type);
    }
}

template<typename T>
AGMS_Sketch<T>::AGMS_Sketch(AGMS_Sketch<T>* copy) {
    this->num_rows = copy->num_rows;
    this->num_cols = copy->num_cols;
    this->average_function = copy->average_function;

    this->xis = new Xi<T>*[this->num_rows * this->num_cols];
    this->sketch_elem = new double[this->num_rows * this->num_cols];
    for (int i = 0; i < this->num_rows * this->num_cols; i++) {
        this->sketch_elem[i] = copy->sketch_elem[i];
        this->xis[i] = copy->xis[i]->copy();
    }
}

template<typename T>
AGMS_Sketch<T>::~AGMS_Sketch() {
    for (int i = 0; i < this->num_rows * this->num_cols; i++) {
        delete xis[i];
    }
    delete[] xis;
}

template<typename T>
void AGMS_Sketch<T>::update(T key, double weight) {
    for (int i = 0; i < this->num_rows * this->num_cols; i++)
        this->sketch_elem[i] += double(xis[i]->element(key)) * weight;
}

template<typename T>
double AGMS_Sketch<T>::inner_join(Sketch<T> *other) {
    double *basic_est = new double[this->num_rows * this->num_cols];
    for (int i = 0; i < this->num_rows * this->num_cols; i++)
        basic_est[i] = this->sketch_elem[i]
                * ((AGMS_Sketch<T>*) other)->sketch_elem[i];

    double *avg_est = new double[this->num_rows];
    for (int i = 0; i < this->num_rows; i++)
        avg_est[i] = mean(basic_est + i * this->num_cols, this->num_cols);

    double result = (*average_function)(avg_est, this->num_rows);

    delete[] basic_est;
    delete[] avg_est;

    return result;
}

template<typename T>
double AGMS_Sketch<T>::second_moment() {
    return this->inner_join(this);
}

template<typename T>
double AGMS_Sketch<T>::first_moment() {
    double *basic_est = new double[this->num_rows * this->num_cols];
    for (int i = 0; i < this->num_rows * this->num_cols; i++)
        basic_est[i] = std::abs(this->sketch_elem[i]);

    double *avg_est = new double[this->num_rows];
    for (int i = 0; i < this->num_rows; i++) {
        avg_est[i] = mean(basic_est + i * this->num_cols, this->num_cols);
    }

    double result = (*average_function)(avg_est, this->num_rows);

    delete[] basic_est;
    delete[] avg_est;

    return result;
}

template<typename T>
Sketch<T>* AGMS_Sketch<T>::difference(Sketch<T> *other) {
    AGMS_Sketch<T>* diff_sketch = new AGMS_Sketch<T>(this->num_cols,
            this->num_rows, xis);
    for (int i = 0; i < this->num_rows * this->num_cols; i++) {
        diff_sketch->sketch_elem[i] = this->sketch_elem[i]
                - ((AGMS_Sketch<T>*) other)->sketch_elem[i];
    }
    return diff_sketch;
}

template<typename T>
Sketch<T>* AGMS_Sketch<T>::copy() {
    AGMS_Sketch<T>* copy = new AGMS_Sketch<T>(this);
    return copy;
}

/**************************FAGMS_Sketch implementation*************************/

template<typename T>
FAGMS_Sketch<T>::FAGMS_Sketch(unsigned int buckets, unsigned int rows,
        Hash<T> **hashes, Xi<T> **xis, const char *avg_func) {
    this->num_cols = buckets;
    this->num_rows = rows;
    this->average_function = get_average_function(avg_func);

    this->hashes = new Hash<T>*[rows];
    this->xis = new Xi<T>*[rows];
    for (unsigned i = 0; i < rows; i++) {
        this->hashes[i] = hashes[i]->copy();
        this->xis[i] = xis[i]->copy();
    }

    this->sketch_elem = new double[buckets * rows];
    for (int i = 0; i < buckets * rows; i++)
        this->sketch_elem[i] = 0.0;
}

template<typename T>
FAGMS_Sketch<T>::FAGMS_Sketch(unsigned int buckets, unsigned int rows,
        const char *hash_func, const char *xis_type, const char *avg_func) {
    this->num_cols = buckets;
    this->num_rows = rows;
    this->average_function = get_average_function(avg_func);

    this->hashes = new Hash<T>*[rows];
    this->xis = new Xi<T>*[rows];
    for (unsigned i = 0; i < rows; i++) {
        this->hashes[i] = get_random_hash<T>(hash_func, buckets);
        this->xis[i] = get_random_xi<T>(xis_type);
    }

    this->sketch_elem = new double[buckets * rows];
    for (int i = 0; i < buckets * rows; i++)
        this->sketch_elem[i] = 0.0;
}

template<typename T>
FAGMS_Sketch<T>::FAGMS_Sketch(FAGMS_Sketch<T> *copy) {
    this->num_cols = copy->num_cols;
    this->num_rows = copy->num_rows;
    this->average_function = copy->average_function;

    hashes = new Hash<T>*[this->num_rows];
    xis = new Xi<T>*[this->num_rows];
    for (unsigned i = 0; i < this->num_rows; i++) {
        hashes[i] = copy->hashes[i]->copy();
        xis[i] = copy->xis[i]->copy();
    }

    this->sketch_elem = new double[this->num_cols * this->num_rows];
    for (int i = 0; i < this->num_cols * this->num_rows; i++)
        this->sketch_elem[i] = copy->sketch_elem[i];
}

template<typename T>
FAGMS_Sketch<T>::~FAGMS_Sketch() {
    for (unsigned i = 0; i < this->num_rows; i++) {
        delete hashes[i];
        delete xis[i];
    }
    delete[] hashes;
    delete[] xis;
}

template<typename T>
void FAGMS_Sketch<T>::update(T key, double weight) {
    for (int i = 0; i < this->num_rows; i++) {
        int bucket = (int) hashes[i]->element(key);
        this->sketch_elem[i * this->num_cols + bucket] += xis[i]->element(key)
                * weight;
    }
}

template<typename T>
double FAGMS_Sketch<T>::inner_join(Sketch<T> *other) {
    double *basic_est = new double[this->num_rows];
    for (int i = 0; i < this->num_rows; i++) {
        basic_est[i] = 0.0;
        for (int j = i * this->num_cols; j < (i + 1) * this->num_cols; j++)
            basic_est[i] += this->sketch_elem[j]
                    * ((FAGMS_Sketch<T>*) other)->sketch_elem[j];
    }
    double result = (*average_function)(basic_est, this->num_rows);
    delete[] basic_est;
    return result;
}

template<typename T>
double FAGMS_Sketch<T>::second_moment() {
    return this->inner_join(this);
}

template<typename T>
double FAGMS_Sketch<T>::first_moment() {
    double *basic_est = new double[this->num_rows];
    for (int i = 0; i < this->num_rows; i++) {
        basic_est[i] = 0.0;
        for (int j = i * this->num_cols; j < (i + 1) * this->num_cols; j++)
            basic_est[i] += std::abs(this->sketch_elem[j]);
    }
    double result = (*average_function)(basic_est, this->num_rows);
    delete[] basic_est;
    return result;
}

template<typename T>
Sketch<T>* FAGMS_Sketch<T>::difference(Sketch<T> *other) {
    FAGMS_Sketch<T>* diff_sketch = new FAGMS_Sketch<T>(this->num_cols,
            this->num_rows, hashes, xis);
    for (unsigned int i = 0; i < this->num_rows * this->num_cols; i++) {
        diff_sketch->sketch_elem[i] = this->sketch_elem[i]
                - ((FAGMS_Sketch<T>*) other)->sketch_elem[i];
    }
    return diff_sketch;
}

template<typename T>
Sketch<T>* FAGMS_Sketch<T>::copy() {
    FAGMS_Sketch<T>* copy = new FAGMS_Sketch<T>(this);
    return copy;
}

/********************* FastCount_Sketch implementation ************************/

template<typename T>
FastCount_Sketch<T>::FastCount_Sketch(unsigned int buckets, unsigned int rows,
        Hash<T> **hashes) {
    this->num_cols = buckets;
    this->num_rows = rows;

    this->hashes = new Hash<T>*[rows];
    hashes[0]->copy();
    for (unsigned int i = 0; i < rows; i++) {
        this->hashes[i] = hashes[i]->copy();
    }
    this->sketch_elem = new double[buckets * rows];
    for (unsigned int i = 0; i < buckets * rows; i++)
        this->sketch_elem[i] = 0.0;
}

template<typename T>
FastCount_Sketch<T>::FastCount_Sketch(unsigned int buckets, unsigned int rows,
        const char *hash_func) {
    this->num_cols = buckets;
    this->num_rows = rows;

    this->hashes = new Hash<T>*[rows];
    hashes[0]->copy();
    for (unsigned int i = 0; i < rows; i++) {
        this->hashes[i] = get_random_hash<T>(hash_func, buckets);
    }
    this->sketch_elem = new double[buckets * rows];
    for (unsigned int i = 0; i < buckets * rows; i++)
        this->sketch_elem[i] = 0.0;
}

template<typename T>
FastCount_Sketch<T>::FastCount_Sketch(FastCount_Sketch<T> *copy) {
    this->num_cols = copy->num_cols;
    this->num_rows = copy->num_rows;

    // Copy hashes:
    hashes = new Hash<T>*[this->num_rows];
    for (unsigned int i = 0; i < this->num_rows; i++) {
        hashes[i] = copy->hashes[i]->copy();
    }

    this->sketch_elem = new double[this->num_cols * this->num_rows];
    for (unsigned int i = 0; i < this->num_cols * this->num_rows; i++)
        this->sketch_elem[i] = copy->sketch_elem[i];
}

template<typename T>
FastCount_Sketch<T>::~FastCount_Sketch() {
    for (unsigned int i = 0; i < this->num_rows; i++) {
        delete hashes[i];
    }
    delete[] hashes;
}

template<typename T>
void FastCount_Sketch<T>::update(T key, double weight) {
    for (unsigned int i = 0; i < this->num_rows; i++) {
        int bucket = (int) hashes[i]->element(key);
        this->sketch_elem[i * this->num_cols + bucket] += weight;
    }
}

template<typename T>
double FastCount_Sketch<T>::inner_join(Sketch<T> *other) {
    double *basic_est = new double[this->num_rows];
    for (unsigned int i = 0; i < this->num_rows; i++) {
        double L1 = 0.0;
        double L1p = 0.0;
        double L2 = 0.0;
        for (unsigned int j = i * this->num_cols; j < (i + 1) * this->num_cols;
                j++) {
            L1 += this->sketch_elem[j];
            L1p += ((FastCount_Sketch<T>*) other)->sketch_elem[j];
            L2 += this->sketch_elem[j]
                    * ((FastCount_Sketch<T>*) other)->sketch_elem[j];
        }
        basic_est[i] = 1.0 / ((double) this->num_cols - 1)
                * ((double) this->num_cols * L2 - L1 * L1p);
    }

    double result = mean(basic_est, this->num_rows);
    delete[] basic_est;
    return result;
}

template<typename T>
double FastCount_Sketch<T>::second_moment() {
    return this->inner_join(this);
}

template<typename T>
double FastCount_Sketch<T>::first_moment() {
    double* basic_est = new double[this->num_rows];
    for (unsigned int i = 0; i < this->num_rows; i++) {
        basic_est[i] = 0.0;
        for (unsigned int j = i * this->num_cols; j < (i + 1) * this->num_cols;
                j++) {
            basic_est[i] += std::abs(this->sketch_elem[j]);
        }
    }

    double result = mean(basic_est, this->num_rows);
    delete[] basic_est;
    return result;
}

template<typename T>
Sketch<T>* FastCount_Sketch<T>::difference(Sketch<T> *other) {
    FastCount_Sketch<T>* diff_sketch = new FastCount_Sketch<T>(this);
    for (unsigned int i = 0; i < this->num_rows * this->num_cols; i++) {
        diff_sketch->sketch_elem[i] = this->sketch_elem[i]
                - ((FastCount_Sketch<T>*) other)->sketch_elem[i];
    }
    return diff_sketch;
}

template<typename T>
Sketch<T>* FastCount_Sketch<T>::copy() {
    FastCount_Sketch<T>* copy = new FastCount_Sketch<T>(this);
    return copy;
}

/********************* CountMin_Sketch implementation ************************/

template<typename T>
CountMin_Sketch<T>::CountMin_Sketch(unsigned int buckets, unsigned int rows,
        Hash<T> **hashes) {
    this->num_cols = buckets;
    this->num_rows = rows;

    this->hashes = new Hash<T>*[rows];
    for (unsigned int i = 0; i < rows; i++) {
        this->hashes[i] = hashes[i]->copy();
    }

    this->sketch_elem = new double[buckets * rows];
    for (unsigned int i = 0; i < buckets * rows; i++)
        this->sketch_elem[i] = 0.0;
}

template<typename T>
CountMin_Sketch<T>::CountMin_Sketch(CountMin_Sketch<T> * copy) {
    this->num_cols = copy->num_cols;
    this->num_rows = copy->num_rows;

    // Copy hashes:
    hashes = new Hash<T>*[this->num_rows];
    for (unsigned int i = 0; i < this->num_rows; i++) {
        hashes[i] = copy->hashes[i]->copy();
    }

    this->sketch_elem = new double[this->num_cols * this->num_rows];
    for (unsigned int i = 0; i < this->num_cols * this->num_rows; i++)
        this->sketch_elem[i] = copy->sketch_elem[i];
}

template<typename T>
CountMin_Sketch<T>::~CountMin_Sketch() {
    for (unsigned int i = 0; i < this->num_rows; i++) {
        delete hashes[i];
    }
    delete[] hashes;
}

template<typename T>
void CountMin_Sketch<T>::update(T key, double weight) {

    for (unsigned int i = 0; i < this->num_rows; i++) {
        int bucket = (int) hashes[i]->element(key);
        this->sketch_elem[i * this->num_cols + bucket] += weight;
    }
}

template<typename T>
double CountMin_Sketch<T>::inner_join(Sketch<T> *other) {
    double *basic_est = new double[this->num_rows];
    for (unsigned int i = 0; i < this->num_rows; i++) {
        basic_est[i] = 0.0;
        for (int j = i * this->num_cols; j < (i + 1) * this->num_cols; j++) {
            basic_est[i] += this->sketch_elem[j]
                    * ((CountMin_Sketch<T>*) other)->sketch_elem[j];
        }
    }

    double result = min(basic_est, this->num_rows);
    delete[] basic_est;
    return result;
}

template<typename T>
double CountMin_Sketch<T>::second_moment() {
    return this->inner_join(this);
}

template<typename T>
double CountMin_Sketch<T>::first_moment() {
    double *basic_est = new double[this->num_rows];
    for (unsigned int i = 0; i < this->num_rows; i++) {
        basic_est[i] = 0.0;
        for (int j = i * this->num_cols; j < (i + 1) * this->num_cols; j++)
            basic_est[i] += std::abs(this->sketch_elem[j]);
    }

    double result = min(basic_est, this->num_rows);
    delete[] basic_est;
    return result;
}

template<typename T>
Sketch<T>* CountMin_Sketch<T>::difference(Sketch<T>* other) {
    CountMin_Sketch<T>* diff_sketch = new CountMin_Sketch<T>(this->num_cols,
            this->num_rows, hashes);
    for (int i = 0; i < this->num_rows * this->num_cols; i++)
        diff_sketch->sketch_elem[i] = this->sketch_elem[i]
                - ((CountMin_Sketch<T>*) other)->sketch_elem[i];
    return diff_sketch;
}

template<typename T>
Sketch<T>* CountMin_Sketch<T>::copy() {
    CountMin_Sketch<T>* copy = new CountMin_Sketch<T>(this);
    return copy;
}
