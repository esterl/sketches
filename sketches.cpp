#include "sketches.h"

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

double min(double* values, unsigned int n) {
    double min = values[0];
    for (unsigned int i = 1; i < n; i++)
        if (values[i] < min)
            min = values[i];

    return min;
}
