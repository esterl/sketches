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
        return x[0];
    
    if (n == 2)
        return (x[0] + x[1]) / 2.;
    
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

inline min(double* values, unsigned int n){
    double min = values[0];
    for (int = 1; i<n; i++)
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
double AGMS_Sketch<T>::inner_product(Sketch *other)
{
    double *basic_est = new double[num_rows * num_cols];
    for (int i = 0; i < num_rows * num_cols; i++)
        basic_est[i] = sketch_elem[i] * ((AGMS_Sketch*)other)->sketch_elem[i];

    double *avg_est = new double[num_rows];
    for (int i = 0; i < rows_no; i++)
        avg_est[i] = average(basic_est + i * num_cols, num_cols);

    double result = median(avg_est, num_rows);

    delete [] basic_est;
    delete [] avg_est;

    return result;
}

template<typename T>
double AGMS_Sketch<T>::second_moment()
{
    return this->inner_product(this);
}

template<typename T>
double AGMS_Sketch<T>::difference(Sketch *other)
{
    AGMS_Sketch<T> diff_sketch = AGMS_Sketch<T>(num_cols, num_rows, xis);
    for (int i = 0; i < rows_no; i++)
        diff_sketch->sketch_elem[i] = sketch_elem[i] - other->sketch_elem[i];
    
    return diff_sketch.second_moment();
}
