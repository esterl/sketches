#ifndef SKETCHES_PY_SKETCHES_H
#define SKETCHES_PY_SKETCHES_H

#include <Python.h>

#include "utils.h"
#include <sketches.h>

/************************** Templated functions *******************************/

template<typename SketchType>
static PyObject *
Sketch_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    SketchType *self;
    self = PyObject_NEW(SketchType, type);
    return (PyObject *)self;
}


template<typename SketchType>
static void
Sketch_dealloc(SketchType* self)
{
    delete self->sketch;
    self->ob_type->tp_free((PyObject*)self);
}


template<typename SketchType, typename KeyType>
static PyObject *
Sketch_update(SketchType* self, PyObject *args, PyObject *kwds)
{
    KeyType key;
    PyObject *py_key;
    double weight=1.;
    static char *kwlist[] = {"key", "weight", NULL};
    
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "|Od", kwlist, &py_key, &weight))
        return NULL;
    
    key = PyObjectToNumber<KeyType>(py_key);

    self->sketch->update(key, weight);
    
    Py_RETURN_NONE; 
}


template<typename SketchType>
static PyObject *
Sketch_clear(SketchType* self, PyObject *args, PyObject *kwds)
{
    self->sketch->clear();
    Py_RETURN_NONE; 
}


template<typename PyType,typename CType, typename KeyType>
static PyObject *
Sketch_difference(PyType* self, PyObject *args, PyObject *kwds)
{
    PyType* other;
    PyType* result;
    static char *kwlist[] = {"other", NULL};
    
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &other))
        return NULL;
    
    if (! PyObject_TypeCheck((PyObject*) other, self->ob_type))
        return NULL;
    
    result = PyObject_NEW(PyType, self->ob_type);
    result->sketch = (CType*) self->sketch->difference((Sketch<KeyType>*)other->sketch);
    return (PyObject*) result;
}

template<typename PyType,typename KeyType>
static PyObject *
Sketch_inner_product(PyType* self, PyObject *args, PyObject *kwds)
{
    PyType* other;
    double result;
    static char *kwlist[] = {"other", NULL};
    
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &other))
        return NULL;
    
    if (! PyObject_TypeCheck((PyObject*) other, self->ob_type))
        return NULL;
    
    result = self->sketch->inner_join((Sketch<KeyType>*)other->sketch);
    return PyFloat_FromDouble(result);
}

template<typename PyType, typename CType>
static PyObject *
Sketch_copy(PyType* self)
{
    PyType* result ;
    
    result = PyObject_NEW(PyType, self->ob_type);
    result->sketch = new CType(self->sketch);
    return (PyObject*) result;
}

template<typename SketchType>
static PyObject *
Sketch_second_moment(SketchType* self, PyObject *args, PyObject *kwds)
{
    double result;
    result = self->sketch->second_moment();
    return PyFloat_FromDouble(result);
}

template<typename SketchType>
static PyObject *
Sketch_first_moment(SketchType* self, PyObject *args, PyObject *kwds)
{
    double result;
    result = self->sketch->first_moment();
    return PyFloat_FromDouble(result);
}


template<typename SketchType>
static PyObject *
Sketch_get_key_size(SketchType* self, PyObject *args, PyObject *kwds)
{
    unsigned int result;
    
    result = self->sketch->get_key_size();
    return PyInt_FromLong(result);
}

template<typename SketchType>
static PyObject *
Sketch_get_rows(SketchType* self, PyObject *args, PyObject *kwds)
{
    unsigned int result;
    
    result = self->sketch->get_num_rows();
    return PyInt_FromLong(result);
}

template<typename SketchType>
static PyObject *
Sketch_get_columns(SketchType* self, PyObject *args, PyObject *kwds)
{
    unsigned int result;
    
    result = self->sketch->get_num_columns();
    return PyInt_FromLong(result);
}

template<typename PyType>
static PyObject * 
Sketch_iadd(PyType* self, PyObject *args, PyObject *kwds)
{
    PyType* other;
    static char *kwlist[] = {"other", NULL};
    
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &other))
        return NULL;
    
    if (! PyObject_TypeCheck((PyObject*) other, self->ob_type))
        return NULL;
    (*self->sketch) += (*other->sketch);
    Py_INCREF(self);
    return (PyObject*) self;
}

template<typename SketchType>
static PyObject *
Sketch_get_max(SketchType* self, PyObject *args, PyObject *kwds)
{
    double result;
    result = self->sketch->get_max();
    return PyFloat_FromDouble(result);
}

template<typename SketchType>
static PyObject *
Sketch_get_bytes(SketchType* self, PyObject *args, PyObject *kwds)
{
    double result;
    result = self->sketch->get_bytes();
    return PyFloat_FromDouble(result);
}

template<typename SketchType>
static PyObject *
Sketch_get_optimized_bits(SketchType* self, PyObject *args, PyObject *kwds)
{
    double result;
    result = self->sketch->get_optimized_bits();
    return PyFloat_FromDouble(result);
}

template<typename SketchType>
static PyObject *
Sketch_get_counters(SketchType* self, PyObject *args, PyObject *kwds)
{
    double *result;
    result = self->sketch->get_counters();
    int num_columns = self->sketch->get_num_columns();
    int num_rows = self->sketch->get_num_rows();
    
    PyObject* py_result = PyList_New(num_rows*num_columns);
    for (int i=0; i < num_rows*num_columns; i++){
        PyList_SetItem(py_result, i, PyFloat_FromDouble(result[i]));
    }
    
    return py_result;
}
#endif
