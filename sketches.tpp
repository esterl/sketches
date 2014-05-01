#include "sketches.h"

#include <string.h>

/**************************Basic math functions *******************************/
inline double average(double* values, unsigned int n)
{
    double sum = 0;
    for (int i = 0; i < n; i++)
        sum += values[i];
    return sum / (double) n;
}

inline double median(double* values, unsigned int n)
{
    if (n == 1)
        return values[0];
    
    if (n == 2)
        return (values[0] + values[1]) / 2.;
    
    double *aux = new double[n];
    for (int i = 0; i < n; i++)
        aux[i] = values[i];

    // implement bubble sort
    bool rpt = true;
    while (rpt){
        rpt = false;

        for (int i = 0; i < n - 1; i++) {
            if (aux[i] > aux[i+1]) {
                double t = aux[i];
                aux[i] = aux[i + 1];
                aux[i + 1] = t;
                rpt = true;
            }
        }
    }
    
    double res;
    if (n%2 == 0)
        res = (aux[n/2 -1] + aux[n/2]) / 2.;
    else
        res = aux[n/2];
    
    delete [] aux;
    return res;
}

inline double min(double* values, unsigned int n)
{
    double min = values[0];
    for (int i = 1; i<n; i++)
        if (values[i] < min) min = values[i];
    
    return min;
}


/**************************AGMS_Sketch implementation**************************/
template<typename T>
AGMS_Sketch<T>::AGMS_Sketch(unsigned int cols, unsigned int rows, Xi<T> **xis)
{
    num_rows = rows;
    num_cols = cols;
    this->xis = xis;
    this->sketch_elem = new double[rows * cols];
    for (int i = 0; i < rows * cols; i++)
        this->sketch_elem[i] = 0.0;
}

template<typename T>
AGMS_Sketch<T>::~AGMS_Sketch()
{
    delete [] sketch_elem;
}

template<typename T>
void AGMS_Sketch<T>::clear()
{
    for (int i = 0; i < num_rows * num_cols; i++)
        sketch_elem[i] = 0.0;
}

template<typename T>
void AGMS_Sketch<T>::update_sketch(T key, double weight)
{
    for (int i = 0; i < num_rows * num_cols; i++)
        sketch_elem[i] += double(xis[i]->element(key)) * weight;
}

template<typename T>
double AGMS_Sketch<T>::inner_join(Sketch<T> *other)
{
    double *basic_est = new double[num_rows * num_cols];
    for (int i = 0; i < num_rows * num_cols; i++)
        basic_est[i] = sketch_elem[i] * 
                            ((AGMS_Sketch<T>*)other)->sketch_elem[i];

    double *avg_est = new double[num_rows];
    for (int i = 0; i < num_rows; i++)
        avg_est[i] = average(basic_est + i * num_cols, num_cols);

    double result = median(avg_est, num_rows);

    delete [] basic_est;
    delete [] avg_est;

    return result;
}

template<typename T>
double AGMS_Sketch<T>::second_moment()
{
    return this->inner_join(this);
}

template<typename T>
double AGMS_Sketch<T>::difference(Sketch<T> *other)
{
    AGMS_Sketch<T> diff_sketch = AGMS_Sketch<T>(num_cols, num_rows, xis);
    for (int i = 0; i < num_rows; i++)
        diff_sketch.sketch_elem[i] = sketch_elem[i] - 
                                    ((AGMS_Sketch<T>*)other)->sketch_elem[i];
    
    return diff_sketch.second_moment();
}


/**************************FAGMS_Sketch implementation*************************/

template<typename T>
FAGMS_Sketch<T>::FAGMS_Sketch(unsigned int buckets, unsigned int rows, 
                                Hash<T> **hashes, Xi<T> **xis)
{
    num_buckets = buckets;
    num_rows = rows;
    this->hashes = hashes;
    this->xis = xis;
    
    this->sketch_elem = new double[buckets*rows];
    for (int i = 0; i < buckets*rows; i++)
        this->sketch_elem[i] = 0.0;
}

template<typename T>
FAGMS_Sketch<T>::~FAGMS_Sketch()
{
    delete [] sketch_elem;
}

template<typename T>
void FAGMS_Sketch<T>::clear()
{
    for (int i = 0; i < num_buckets * num_rows; i++)
        sketch_elem[i] = 0.0;
}

template<typename T>
void FAGMS_Sketch<T>::update_sketch(T key, double weight)
{
    for (int i = 0; i < num_rows; i++)
    {
        int bucket = (int)hashes[i]->element(key);
        sketch_elem[i * num_buckets + bucket] += xis[i]->element(key) * weight;
    }
}

template<typename T>
double FAGMS_Sketch<T>::inner_join(Sketch<T> *other)
{
    double *basic_est = new double[num_rows];
    for (int i = 0; i < num_rows; i++)
    {
        basic_est[i] = 0.0;
        for (int j = i*num_buckets; j < (i+1)*num_buckets; j++)
            basic_est[i] += sketch_elem[j] * 
                                ((FAGMS_Sketch<T>*)other)->sketch_elem[j];
    }
    double result = median(basic_est, num_rows);
    delete [] basic_est;
    return result;
}

template<typename T>
double FAGMS_Sketch<T>::second_moment()
{
    return this->inner_join(this);
}

template<typename T>
double FAGMS_Sketch<T>::difference(Sketch<T> *other)
{
    FAGMS_Sketch<T> diff_sketch = FAGMS_Sketch<T>(num_buckets, num_rows, 
                                                    hashes, xis);
    for (int i = 0; i < num_rows; i++)
        diff_sketch.sketch_elem[i] = sketch_elem[i] - 
                                    ((FAGMS_Sketch<T>*)other)->sketch_elem[i];
    
    return diff_sketch.second_moment();
}



/********************* FastCount_Sketch implementation ************************/

template<typename T>
FastCount_Sketch<T>::FastCount_Sketch(unsigned int buckets, unsigned int rows, 
                                    Hash<T> **hashes)
{
    num_buckets = buckets;
    num_rows = rows;
    this->hashes = hashes;
    
    this->sketch_elem = new double[buckets*rows];
    for (int i = 0; i < buckets * rows; i++)
        this->sketch_elem[i] = 0.0;
}

template<typename T>
FastCount_Sketch<T>::~FastCount_Sketch()
{
    delete [] sketch_elem;
}

template<typename T>
void FastCount_Sketch<T>::clear()
{
    for (int i = 0; i < num_buckets * num_rows; i++)
        sketch_elem[i] = 0.0;
}

template<typename T>
void FastCount_Sketch<T>::update_sketch(T key, double weight)
{
    for (int i = 0; i < num_rows; i++)
    {
        int bucket = (int)hashes[i]->element(key);
        sketch_elem[i * num_buckets + bucket] += weight;
    }
}

template<typename T>
double FastCount_Sketch<T>::inner_join(Sketch<T> *other)
{
    double *basic_est = new double[num_rows];
    for (int i = 0; i < num_rows; i++)
    {
        double L1 = 0.0;
        double L1p = 0.0;
        double L2 = 0.0;
        for (int j = i*num_buckets; j < (i+1)*num_buckets; j++)
        {
            L1 += sketch_elem[j];
            L1p += ((FastCount_Sketch<T>*)other)->sketch_elem[j];
            L2 += sketch_elem[j] * ((FastCount_Sketch<T>*)other)->sketch_elem[j];
        }
        basic_est[i] = 1.0 / 
            ((double)num_buckets - 1) * ((double)num_buckets * L2 - L1 * L1p);
    }

    double result = average(basic_est, num_rows);
    delete [] basic_est;
    return result;
}

template<typename T>
double FastCount_Sketch<T>::second_moment()
{
    return this->inner_join(this);
}

template<typename T>
double FastCount_Sketch<T>::difference(Sketch<T> *other)
{
    FastCount_Sketch<T> diff_sketch = FastCount_Sketch<T>(num_buckets, num_rows,
                                                            hashes);
    for (int i = 0; i < num_rows; i++)
        diff_sketch.sketch_elem[i] = sketch_elem[i] - 
                                ((FastCount_Sketch<T>*)other)->sketch_elem[i];
    
    return diff_sketch.second_moment();
}



/********************* FastCount_Sketch implementation ************************/

template<typename T>
CountMin_Sketch<T>::CountMin_Sketch(unsigned int buckets, unsigned int rows, 
                                        Hash<T> **hashes)
{
    num_buckets = buckets;
    num_rows = rows;
    this->hashes = hashes;
    
    this->sketch_elem = new double[buckets * rows];
    for (int i = 0; i < buckets * rows; i++)
        this->sketch_elem[i] = 0.0;
}

template<typename T>
CountMin_Sketch<T>::~CountMin_Sketch()
{
    delete [] sketch_elem;
}

template<typename T>
void CountMin_Sketch<T>::clear()
{
    for (int i = 0; i < num_buckets * num_rows; i++)
        sketch_elem[i] = 0.0;
}

template<typename T>
void CountMin_Sketch<T>::update_sketch(T key, double weight)
{
    for (int i = 0; i < num_rows; i++)
    {
        int bucket = (int)hashes[i]->element(key);
        sketch_elem[i * num_buckets + bucket] += weight;
    }
}

template<typename T>
double CountMin_Sketch<T>::inner_join(Sketch<T> *other)
{
    double *basic_est = new double[num_rows];
    for (int i = 0; i < num_rows; i++)
    {
        basic_est[i] = 0.0;
        for (int j = i*num_buckets; j < (i+1)*num_buckets; j++)
            basic_est[i] += sketch_elem[j] * 
                                ((CountMin_Sketch<T>*)other)->sketch_elem[j];
    }

    double result = min(basic_est, num_rows);
    delete [] basic_est;
    return result;
}

template<typename T>
double CountMin_Sketch<T>::second_moment()
{
    return this->inner_join(this);
}

template<typename T>
double CountMin_Sketch<T>::difference(Sketch<T>* other)
{
    CountMin_Sketch<T> diff_sketch = CountMin_Sketch<T>(num_buckets, num_rows,
                                                            hashes);
    for (int i = 0; i < num_rows; i++)
        diff_sketch.sketch_elem[i] = sketch_elem[i] - 
                                ((CountMin_Sketch<T>*)other)->sketch_elem[i];
    
    return diff_sketch.second_moment();
}

