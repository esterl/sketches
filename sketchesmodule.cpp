#include <Python.h>

#include "sketches.h"

template<typename T>
struct FastCount {
    PyObject_HEAD
    FastCount_Sketch<T> * sketch;
};

template<typename T>
static void
FastCount_dealloc(FastCount<T>* self)
{
    delete self->sketch;
    self->ob_type->tp_free((PyObject*)self);
}

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

// From http://www.cplusplus.com/forum/articles/9645/
template <typename T>
T StringToNumber ( const std::string &Text ) //Text not by const reference so that the function can be used with a 
{                                       //character array as argument
    std::stringstream ss(Text);
    T result;
    return ss >> result ? result : T(0);
}

//Needs to be specific since we are parsing the key
template<typename T>
static PyObject *
FastCount_update(FastCount<T>* self, PyObject *args, PyObject *kwds)
{
    T key;
    char *str;
    double weight;
    static char *kwlist[] = {"key", "weight", NULL};
    
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "|sd", kwlist, &str, &weight))
        return NULL;
    
    key = StringToNumber<T>(str);
    self->sketch->update(key, weight);
    
    Py_RETURN_NONE; 
}

template<typename T>
static PyObject *
FastCount_difference(FastCount<T>* self, PyObject *args, PyObject *kwds)
{
    FastCount<T>* other;
    double result;
    static char *kwlist[] = {"other", NULL};
    
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &other))
        return NULL;
    
    if (! PyObject_TypeCheck((PyObject*) other, self->ob_type))
        return NULL;
    
    result = self->sketch->difference((Sketch<T>*)other->sketch);
    return PyFloat_FromDouble(result);
}

template<typename T>
static PyObject *
FastCount_copy(FastCount<T>* self)
{
    FastCount<T>* result ;
    
    result = PyObject_NEW(FastCount<T>, self->ob_type);
    result->sketch = new FastCount_Sketch<T>(self->sketch);
    return (PyObject*) result;
}

template<typename T>
static PyObject *
FastCount_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    FastCount<T> *self;
    self = PyObject_NEW(FastCount<T>, type);
    return (PyObject *)self;
}

/******************************** uint16_t ************************************/
template static void FastCount_dealloc<uint16_t>(FastCount<uint16_t>*);
template static int FastCount_init<uint16_t, uint64_t> (FastCount<uint16_t> *, PyObject *, PyObject *);
template static PyObject * FastCount_difference<uint16_t>(FastCount<uint16_t>* , PyObject *, PyObject *);
template static PyObject * FastCount_copy<uint16_t>(FastCount<uint16_t>* );
template static PyObject * FastCount_new<uint16_t>(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * FastCount_update<uint16_t>(FastCount<uint16_t>* , PyObject *, PyObject *);


static PyTypeObject FastCount16Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.FastCount16",         /*tp_name*/
    sizeof(FastCount<uint16_t>),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) FastCount_dealloc<uint16_t>,/*tp_dealloc*/
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
    {"update", (PyCFunction)FastCount_update<uint16_t>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)FastCount_difference<uint16_t>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"copy", (PyCFunction)FastCount_copy<uint16_t>, METH_NOARGS,
     "Copies a sketch"
    },
    {NULL}  /* Sentinel */
};

/******************************** uint32_t ************************************/
typedef ttmath::UInt<3> uint192;
template static void FastCount_dealloc<uint32_t>(FastCount<uint32_t>*);
template static int FastCount_init< uint32_t, uint192> (FastCount<uint32_t> *, PyObject *, PyObject *);
template static PyObject * FastCount_difference<uint32_t>(FastCount<uint32_t>* , PyObject *, PyObject *);
template static PyObject * FastCount_copy<uint32_t>(FastCount<uint32_t>* );
template static PyObject * FastCount_new<uint32_t>(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * FastCount_update<uint32_t>(FastCount<uint32_t>* , PyObject *, PyObject *);


static PyTypeObject FastCount32Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.FastCount32",         /*tp_name*/
    sizeof(FastCount<uint32_t>),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) FastCount_dealloc<uint32_t>,/*tp_dealloc*/
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
    {"update", (PyCFunction)FastCount_update<uint32_t>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)FastCount_difference<uint32_t>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"copy", (PyCFunction)FastCount_copy<uint32_t>, METH_NOARGS,
     "Copies a sketch"
    },
    {NULL}  /* Sentinel */
};


/******************************** uint64_t ************************************/
template static void FastCount_dealloc<uint64_t>(FastCount<uint64_t>*);
template static int FastCount_init< uint64_t, uint192> (FastCount<uint64_t> *, PyObject *, PyObject *);
template static PyObject * FastCount_difference<uint64_t>(FastCount<uint64_t>* , PyObject *, PyObject *);
template static PyObject * FastCount_copy<uint64_t>(FastCount<uint64_t>* );
template static PyObject * FastCount_new<uint64_t>(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * FastCount_update<uint64_t>(FastCount<uint64_t>* , PyObject *, PyObject *);


static PyTypeObject FastCount64Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.FastCount64",         /*tp_name*/
    sizeof(FastCount<uint32_t>),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) FastCount_dealloc<uint32_t>,/*tp_dealloc*/
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
    {"update", (PyCFunction)FastCount_update<uint64_t>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)FastCount_difference<uint64_t>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"copy", (PyCFunction)FastCount_copy<uint64_t>, METH_NOARGS,
     "Copies a sketch"
    },
    {NULL}  /* Sentinel */
};

/******************************** uint128_t ************************************/
typedef ttmath::UInt<2> uint128;
typedef ttmath::UInt<17> uint1042;

template static void FastCount_dealloc<uint128>(FastCount<uint128>*);
template static int FastCount_init< uint128, uint1042> (FastCount<uint128> *, PyObject *, PyObject *);
template static PyObject * FastCount_difference<uint128>(FastCount<uint128>* , PyObject *, PyObject *);
template static PyObject * FastCount_copy<uint128>(FastCount<uint128>* );
template static PyObject * FastCount_new<uint128>(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * FastCount_update<uint128>(FastCount<uint128>* , PyObject *, PyObject *);


static PyTypeObject FastCount128Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.FastCount128",         /*tp_name*/
    sizeof(FastCount<uint128>),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) FastCount_dealloc<uint128>,/*tp_dealloc*/
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
    {"update", (PyCFunction)FastCount_update<uint128>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)FastCount_difference<uint128>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"copy", (PyCFunction)FastCount_copy<uint128>, METH_NOARGS,
     "Copies a sketch"
    },
    {NULL}  /* Sentinel */
};




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

    FastCount16Type.tp_new = FastCount_new<uint16_t>;
    FastCount16Type.tp_methods = FastCount16_methods;
    if (PyType_Ready(&FastCount16Type) < 0)
        return;

    FastCount32Type.tp_new = FastCount_new<uint32_t>;
    FastCount32Type.tp_methods = FastCount32_methods;
    if (PyType_Ready(&FastCount32Type) < 0)
        return;

    FastCount64Type.tp_new = FastCount_new<uint64_t>;
    FastCount64Type.tp_methods = FastCount64_methods;
    if (PyType_Ready(&FastCount64Type) < 0)
        return;

    FastCount128Type.tp_new = FastCount_new<uint128>;
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
