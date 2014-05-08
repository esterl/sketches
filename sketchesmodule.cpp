#include <Python.h>
#include <string>
#include <iostream>

#include "sketches.h"

uint64_t PyObjectToNumber(PyObject *py_num){
    uint64_t result=0;
    if (PyInt_Check(py_num)){
        result = PyInt_AsLong(py_num);
    } else if (PyString_Check(py_num)){
        result = std::stoull(PyString_AsString(py_num));
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


template<typename SketchType, typename KeyType>
static PyObject *
Sketch_difference(SketchType* self, PyObject *args, PyObject *kwds)
{
    SketchType* other;
    double result;
    static char *kwlist[] = {"other", NULL};
    
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &other))
        return NULL;
    
    if (! PyObject_TypeCheck((PyObject*) other, self->ob_type))
        return NULL;
    
    result = self->sketch->difference((Sketch<KeyType>*)other->sketch);
    return PyFloat_FromDouble(result);
}



template<typename SketchType>
static PyObject *
Sketch_second_moment(SketchType* self, PyObject *args, PyObject *kwds)
{
    double result;
    result = self->sketch->second_moment();
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
Sketch_get_key_size(SketchType* self, PyObject *args, PyObject *kwds)
{
    unsigned int result;
    
    result = self->sketch->get_key_size();
    return PyInt_FromLong(result);
}

/************************** FastCount sketch **********************************/
template<typename T>
struct FastCount {
    PyObject_HEAD
    FastCount_Sketch<T> * sketch;
};


// t1 = 16, t2 = 64
template<typename T1, typename T2>
static int
FastCount_init(FastCount<T1> *self, PyObject *args, PyObject *kwds)
{
    unsigned int buckets, rows;
    static char *kwlist[] = {"num_buckets", "num_rows", NULL};
    
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "|II", kwlist, &buckets, &rows))
        return -1;
    
    // Generate num_rows random hashes:
    Hash_CW4<T1, T2>** hashes;
    hashes = new Hash_CW4<T1, T2>*[rows];
    for (unsigned int i =0; i < rows; i++) {
        hashes[i] = new Hash_CW4<T1, T2>(buckets);
    }
    
    // Create the sketch:
    if ( self == NULL ) return -1;
    self->sketch = new FastCount_Sketch<T1>(buckets, rows, (Hash<T1>**) hashes);

    return 0;
}


/******************************** uint16_t ************************************/
typedef FastCount<uint16_t> FastCount16;
template static void Sketch_dealloc<FastCount16>(FastCount16*);
template static int FastCount_init<uint16_t, uint64_t> (FastCount16 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<FastCount16, uint16_t>(FastCount16* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<FastCount16>(FastCount16* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<FastCount16, FastCount_Sketch<uint16_t> >(FastCount16* );
template static PyObject * Sketch_new<FastCount16 >(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<FastCount16,uint16_t>(FastCount16* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<FastCount16>(FastCount16* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<FastCount16>(FastCount16* , PyObject *, PyObject *);

static PyTypeObject FastCount16Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.FastCount16",         /*tp_name*/
    sizeof(FastCount<uint16_t>),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) Sketch_dealloc<FastCount16>,/*tp_dealloc*/
    0,                              /*tp_print*/
    0,                              /*tp_getattr*/
    0,                              /*tp_setattr*/
    0,                              /*tp_compare*/
    0,                              /*tp_repr*/
    0,                              /*tp_as_number*/
    0,                              /*tp_as_sequence*/
    0,                              /*tp_as_mapping*/
    0,                              /*tp_hash */
    0,                              /*tp_call*/
    0,                              /*tp_str*/
    0,                              /*tp_getattro*/
    0,                              /*tp_setattro*/
    0,                              /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,             /*tp_flags*/
    "FastCount sketch for numbers within I={2^16}", /* tp_doc */
    0,                              /* tp_traverse */
    0,                              /* tp_clear */
    0,                              /* tp_richcompare */
    0,                              /* tp_weaklistoffset */
    0,                              /* tp_iter */
    0,                              /* tp_iternext */
    0,                              /* tp_methods */
    0,                              /* tp_members */
    0,                              /* tp_getset */
    0,                              /* tp_base */
    0,                              /* tp_dict */
    0,                              /* tp_descr_get */
    0,                              /* tp_descr_set */
    0,                              /* tp_dictoffset */
    (initproc)FastCount_init<uint16_t, uint64_t>,     /* tp_init */
    0,                              /* tp_alloc */
    0,                              /* tp_new */
};




static PyMethodDef FastCount16_methods[] = {
    {"update", (PyCFunction)Sketch_update<FastCount<uint16_t>,uint16_t>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)Sketch_difference<FastCount<uint16_t>, uint16_t>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<FastCount16>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"copy", (PyCFunction)Sketch_copy<FastCount16, FastCount_Sketch<uint16_t> >, METH_NOARGS,
     "Copies a sketch"
    },
    {"clear", (PyCFunction)Sketch_clear<FastCount16>, METH_NOARGS,
     "Clears the sketch"
    },
    {"get_key_size", (PyCFunction)Sketch_get_key_size<FastCount16>, METH_NOARGS,
     "Returns the size of the key"
    },
    {NULL}  /* Sentinel */
};

/******************************** uint32_t ************************************/
typedef FastCount<uint32_t> FastCount32;
typedef ttmath::UInt<3> uint192;
template static void Sketch_dealloc<FastCount32>(FastCount32*);
template static int FastCount_init< uint32_t, uint192> (FastCount32 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<FastCount32,uint32_t>(FastCount32* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<FastCount32>(FastCount32* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<FastCount32, FastCount_Sketch<uint32_t> >(FastCount32* );
template static PyObject * Sketch_new<FastCount32>(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<FastCount32, uint32_t>(FastCount32* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<FastCount32>(FastCount32* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<FastCount32>(FastCount32* , PyObject *, PyObject *);

static PyTypeObject FastCount32Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.FastCount32",         /*tp_name*/
    sizeof(FastCount32),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) Sketch_dealloc<FastCount32>,/*tp_dealloc*/
    0,                              /*tp_print*/
    0,                              /*tp_getattr*/
    0,                              /*tp_setattr*/
    0,                              /*tp_compare*/
    0,                              /*tp_repr*/
    0,                              /*tp_as_number*/
    0,                              /*tp_as_sequence*/
    0,                              /*tp_as_mapping*/
    0,                              /*tp_hash */
    0,                              /*tp_call*/
    0,                              /*tp_str*/
    0,                              /*tp_getattro*/
    0,                              /*tp_setattro*/
    0,                              /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,             /*tp_flags*/
    "FastCount sketch for numbers within I={2^32}", /* tp_doc */
    0,                              /* tp_traverse */
    0,                              /* tp_clear */
    0,                              /* tp_richcompare */
    0,                              /* tp_weaklistoffset */
    0,                              /* tp_iter */
    0,                              /* tp_iternext */
    0,                              /* tp_methods */
    0,                              /* tp_members */
    0,                              /* tp_getset */
    0,                              /* tp_base */
    0,                              /* tp_dict */
    0,                              /* tp_descr_get */
    0,                              /* tp_descr_set */
    0,                              /* tp_dictoffset */
    (initproc)FastCount_init<uint32_t, uint192 >,     /* tp_init */
    0,                              /* tp_alloc */
    0,                              /* tp_new */
};

static PyMethodDef FastCount32_methods[] = {
    {"update", (PyCFunction)Sketch_update<FastCount32, uint32_t>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)Sketch_difference<FastCount32, uint32_t>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<FastCount32>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"copy", (PyCFunction)Sketch_copy<FastCount32, FastCount_Sketch<uint32_t> >, METH_NOARGS,
     "Copies a sketch"
    },
    {"clear", (PyCFunction)Sketch_clear<FastCount32>, METH_NOARGS,
     "Copies a sketch"
    },
    {"get_key_size", (PyCFunction)Sketch_get_key_size<FastCount32>, METH_NOARGS,
     "Returns the size of the key"
    },
    {NULL}  /* Sentinel */
};


/******************************** uint64_t ************************************/
typedef FastCount<uint64_t> FastCount64;
template static void Sketch_dealloc<FastCount64>(FastCount64*);
template static int FastCount_init< uint64_t, uint192> (FastCount64 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<FastCount64, uint64_t>(FastCount64* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<FastCount64>(FastCount64* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<FastCount64, FastCount_Sketch<uint64_t> >(FastCount64* );
template static PyObject * Sketch_new<FastCount64>(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<FastCount64, uint64_t>(FastCount64* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<FastCount64>(FastCount64* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<FastCount64>(FastCount64* , PyObject *, PyObject *);

static PyTypeObject FastCount64Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.FastCount64",         /*tp_name*/
    sizeof(FastCount64),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) Sketch_dealloc<FastCount64>,/*tp_dealloc*/
    0,                              /*tp_print*/
    0,                              /*tp_getattr*/
    0,                              /*tp_setattr*/
    0,                              /*tp_compare*/
    0,                              /*tp_repr*/
    0,                              /*tp_as_number*/
    0,                              /*tp_as_sequence*/
    0,                              /*tp_as_mapping*/
    0,                              /*tp_hash */
    0,                              /*tp_call*/
    0,                              /*tp_str*/
    0,                              /*tp_getattro*/
    0,                              /*tp_setattro*/
    0,                              /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,             /*tp_flags*/
    "FastCount sketch for numbers within I={2^64}", /* tp_doc */
    0,                              /* tp_traverse */
    0,                              /* tp_clear */
    0,                              /* tp_richcompare */
    0,                              /* tp_weaklistoffset */
    0,                              /* tp_iter */
    0,                              /* tp_iternext */
    0,                              /* tp_methods */
    0,                              /* tp_members */
    0,                              /* tp_getset */
    0,                              /* tp_base */
    0,                              /* tp_dict */
    0,                              /* tp_descr_get */
    0,                              /* tp_descr_set */
    0,                              /* tp_dictoffset */
    (initproc)FastCount_init<uint64_t, uint192 >,     /* tp_init */
    0,                              /* tp_alloc */
    0,                              /* tp_new */
};

static PyMethodDef FastCount64_methods[] = {
    {"update", (PyCFunction)Sketch_update<FastCount64, uint64_t>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)Sketch_difference<FastCount64, uint64_t>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<FastCount64>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"copy", (PyCFunction)Sketch_copy<FastCount64, FastCount_Sketch<uint64_t> >, METH_NOARGS,
     "Copies a sketch"
    },
    {"clear", (PyCFunction)Sketch_clear<FastCount64>, METH_NOARGS,
     "Copies a sketch"
    },
    {"get_key_size", (PyCFunction)Sketch_get_key_size<FastCount64>, METH_NOARGS,
     "Returns the size of the key"
    },
    {NULL}  /* Sentinel */
};

/******************************** uint128_t ************************************/
typedef ttmath::UInt<2> uint128;
typedef ttmath::UInt<17> uint1042;
typedef FastCount<uint128> FastCount128;
template static void Sketch_dealloc<FastCount128>(FastCount128*);
template static int FastCount_init< uint128, uint1042> (FastCount128 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<FastCount128, uint128>(FastCount128* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<FastCount128>(FastCount128* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<FastCount128, FastCount_Sketch<uint128> >(FastCount128* );
template static PyObject * Sketch_new<FastCount128>(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<FastCount128,uint128>(FastCount128* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<FastCount128>(FastCount128* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<FastCount128>(FastCount128* , PyObject *, PyObject *);

static PyTypeObject FastCount128Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.FastCount128",         /*tp_name*/
    sizeof(FastCount128),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) Sketch_dealloc<FastCount128>,/*tp_dealloc*/
    0,                              /*tp_print*/
    0,                              /*tp_getattr*/
    0,                              /*tp_setattr*/
    0,                              /*tp_compare*/
    0,                              /*tp_repr*/
    0,                              /*tp_as_number*/
    0,                              /*tp_as_sequence*/
    0,                              /*tp_as_mapping*/
    0,                              /*tp_hash */
    0,                              /*tp_call*/
    0,                              /*tp_str*/
    0,                              /*tp_getattro*/
    0,                              /*tp_setattro*/
    0,                              /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,             /*tp_flags*/
    "FastCount sketch for numbers within I={2^128}", /* tp_doc */
    0,                              /* tp_traverse */
    0,                              /* tp_clear */
    0,                              /* tp_richcompare */
    0,                              /* tp_weaklistoffset */
    0,                              /* tp_iter */
    0,                              /* tp_iternext */
    0,                              /* tp_methods */
    0,                              /* tp_members */
    0,                              /* tp_getset */
    0,                              /* tp_base */
    0,                              /* tp_dict */
    0,                              /* tp_descr_get */
    0,                              /* tp_descr_set */
    0,                              /* tp_dictoffset */
    (initproc)FastCount_init<uint128, uint1042 >,     /* tp_init */
    0,                              /* tp_alloc */
    0,                              /* tp_new */
};

static PyMethodDef FastCount128_methods[] = {
    {"update", (PyCFunction)Sketch_update<FastCount128, uint128>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)Sketch_difference<FastCount128, uint128>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<FastCount128>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"copy", (PyCFunction)Sketch_copy<FastCount128, FastCount_Sketch<uint128> >, METH_NOARGS,
     "Copies a sketch"
    },
    {"clear", (PyCFunction)Sketch_clear<FastCount128>, METH_NOARGS,
     "Copies a sketch"
    },
    {"get_key_size", (PyCFunction)Sketch_get_key_size<FastCount128>, METH_NOARGS,
     "Returns the size of the key"
    },
    {NULL}  /* Sentinel */
};

/************************* Module *********************************************/
static PyMethodDef sketches_methods[] = {
    {NULL}  /* Sentinel */
};

extern "C"{
#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initsketches(void) 
{
    PyObject* m;

    FastCount16Type.tp_new = Sketch_new<FastCount16>;
    FastCount16Type.tp_methods = FastCount16_methods;
    if (PyType_Ready(&FastCount16Type) < 0)
        return;

    FastCount32Type.tp_new = Sketch_new<FastCount32>;
    FastCount32Type.tp_methods = FastCount32_methods;
    if (PyType_Ready(&FastCount32Type) < 0)
        return;

    FastCount64Type.tp_new = Sketch_new<FastCount64>;
    FastCount64Type.tp_methods = FastCount64_methods;
    if (PyType_Ready(&FastCount64Type) < 0)
        return;

    FastCount128Type.tp_new = Sketch_new<FastCount128>;
    FastCount128Type.tp_methods = FastCount128_methods;
    if (PyType_Ready(&FastCount128Type) < 0)
        return;

    m = Py_InitModule3("sketches", sketches_methods,
                       "Module with a list of sketches for data stream 2-moment estimations.");

    Py_INCREF(&FastCount16Type);
    PyModule_AddObject(m, "FastCount16", (PyObject *)&FastCount16Type);
    PyModule_AddObject(m, "FastCount32", (PyObject *)&FastCount32Type);
    PyModule_AddObject(m, "FastCount64", (PyObject *)&FastCount64Type);
    PyModule_AddObject(m, "FastCount128", (PyObject *)&FastCount128Type);
}

}
