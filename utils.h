#ifndef SKETCHES_UTILS_H
#define SKETCHES_UTILS_H

#include <Python.h>
#include <stdlib.h>

uint64_t PyObjectToNumber(PyObject *py_num){
    uint64_t result=0;
    if (PyInt_Check(py_num)){
        result = PyInt_AsLong(py_num);
    } else if (PyString_Check(py_num)){
        result = strtoul(PyString_AsString(py_num),NULL,0);
    }
    return result;
}

template<typename T>
T PyObjectToNumber( PyObject *py_num){
    T result = 0;
    PyObject * py_str = PyObject_Str(py_num);
    const char* str = PyString_AsString(py_str);
    result = str;
    return result;
};

template<>
uint16_t PyObjectToNumber<uint16_t>(PyObject * py_num)
{
    return PyObjectToNumber(py_num);
};

template<>
uint32_t PyObjectToNumber<uint32_t>(PyObject * py_num)
{
    return PyObjectToNumber(py_num);
};

template<>
uint64_t PyObjectToNumber<uint64_t>(PyObject * py_num)
{
    return PyObjectToNumber(py_num);
};

#endif
