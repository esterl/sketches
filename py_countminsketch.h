#ifndef SKETCHES_PY_COUNTMINSKETCH_H
#define SKETCHES_PY_COUNTMINSKETCH_H

#include "py_sketches.h"

/************************** CountMin sketch **********************************/
template<typename T>
struct CountMin {
    PyObject_HEAD
    CountMin_Sketch<T> * sketch;
};


template<typename KeyType, typename PrimeSpaceType>
static int
CountMin_init(CountMin<KeyType> *self, PyObject *args, PyObject *kwds)
{
    unsigned int buckets, rows;
    static char *kwlist[] = {"num_buckets", "num_rows", NULL};
    
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "|II", kwlist, &buckets, &rows))
        return -1;
    
    // Generate num_rows random hashes:
    Hash_CW2<KeyType, PrimeSpaceType>** hashes;
    hashes = new Hash_CW2<KeyType, PrimeSpaceType>*[rows];
    for (unsigned int i =0; i < rows; i++) {
        hashes[i] = new Hash_CW2<KeyType, PrimeSpaceType>(buckets);
    }
    
    // Create the sketch:
    if ( self == NULL ) return -1;
    self->sketch = new CountMin_Sketch<KeyType>(buckets, rows, (Hash<KeyType>**) hashes);

    return 0;
}


/******************************** uint16_t ************************************/
typedef CountMin<uint16_t> CountMin16;
template static void Sketch_dealloc<CountMin16>(CountMin16*);
template static int CountMin_init<uint16_t, uint64_t> (CountMin16 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<CountMin16, CountMin_Sketch<uint16_t>, uint16_t>(CountMin16* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<CountMin16>(CountMin16* , PyObject *, PyObject *);
template static PyObject * Sketch_first_moment<CountMin16>(CountMin16* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<CountMin16, CountMin_Sketch<uint16_t> >(CountMin16* );
template static PyObject * Sketch_new<CountMin16 >(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<CountMin16,uint16_t>(CountMin16* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<CountMin16>(CountMin16* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<CountMin16>(CountMin16* , PyObject *, PyObject *);

static PyTypeObject CountMin16Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.CountMin16",         /*tp_name*/
    sizeof(CountMin<uint16_t>),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) Sketch_dealloc<CountMin16>,/*tp_dealloc*/
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
    "CountMin sketch for numbers within I={2^16}", /* tp_doc */
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
    (initproc)CountMin_init<uint16_t, uint64_t>,     /* tp_init */
    0,                              /* tp_alloc */
    0,                              /* tp_new */
};




static PyMethodDef CountMin16_methods[] = {
    {"update", (PyCFunction)Sketch_update<CountMin<uint16_t>,uint16_t>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)Sketch_difference<CountMin<uint16_t>, CountMin_Sketch<uint16_t>, uint16_t>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<CountMin16>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"first_moment", (PyCFunction)Sketch_first_moment<CountMin16>, METH_NOARGS,
     "Provides and estimation to the first moment of the sketch"
    },
    {"copy", (PyCFunction)Sketch_copy<CountMin16, CountMin_Sketch<uint16_t> >, METH_NOARGS,
     "Copies a sketch"
    },
    {"clear", (PyCFunction)Sketch_clear<CountMin16>, METH_NOARGS,
     "Clears the sketch"
    },
    {"get_key_size", (PyCFunction)Sketch_get_key_size<CountMin16>, METH_NOARGS,
     "Returns the size of the key"
    },
    {NULL}  /* Sentinel */
};

/******************************** uint32_t ************************************/
typedef CountMin<uint32_t> CountMin32;
typedef ttmath::UInt<3> uint192;
template static void Sketch_dealloc<CountMin32>(CountMin32*);
template static int CountMin_init< uint32_t, uint192> (CountMin32 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<CountMin32, CountMin_Sketch<uint32_t>, uint32_t>(CountMin32* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<CountMin32>(CountMin32* , PyObject *, PyObject *);
template static PyObject * Sketch_first_moment<CountMin32>(CountMin32* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<CountMin32, CountMin_Sketch<uint32_t> >(CountMin32* );
template static PyObject * Sketch_new<CountMin32>(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<CountMin32, uint32_t>(CountMin32* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<CountMin32>(CountMin32* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<CountMin32>(CountMin32* , PyObject *, PyObject *);

static PyTypeObject CountMin32Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.CountMin32",         /*tp_name*/
    sizeof(CountMin32),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) Sketch_dealloc<CountMin32>,/*tp_dealloc*/
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
    "CountMin sketch for numbers within I={2^32}", /* tp_doc */
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
    (initproc)CountMin_init<uint32_t, uint192 >,     /* tp_init */
    0,                              /* tp_alloc */
    0,                              /* tp_new */
};

static PyMethodDef CountMin32_methods[] = {
    {"update", (PyCFunction)Sketch_update<CountMin32, uint32_t>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)Sketch_difference<CountMin32, CountMin_Sketch<uint32_t>, uint32_t>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<CountMin32>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"first_moment", (PyCFunction)Sketch_first_moment<CountMin32>, METH_NOARGS,
     "Provides and estimation to the first moment of the sketch"
    },
    {"copy", (PyCFunction)Sketch_copy<CountMin32, CountMin_Sketch<uint32_t> >, METH_NOARGS,
     "Copies a sketch"
    },
    {"clear", (PyCFunction)Sketch_clear<CountMin32>, METH_NOARGS,
     "Copies a sketch"
    },
    {"get_key_size", (PyCFunction)Sketch_get_key_size<CountMin32>, METH_NOARGS,
     "Returns the size of the key"
    },
    {NULL}  /* Sentinel */
};


/******************************** uint64_t ************************************/
typedef CountMin<uint64_t> CountMin64;
template static void Sketch_dealloc<CountMin64>(CountMin64*);
template static int CountMin_init< uint64_t, uint192> (CountMin64 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<CountMin64, CountMin_Sketch<uint64_t>, uint64_t>(CountMin64* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<CountMin64>(CountMin64* , PyObject *, PyObject *);
template static PyObject * Sketch_first_moment<CountMin64>(CountMin64* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<CountMin64, CountMin_Sketch<uint64_t> >(CountMin64* );
template static PyObject * Sketch_new<CountMin64>(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<CountMin64, uint64_t>(CountMin64* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<CountMin64>(CountMin64* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<CountMin64>(CountMin64* , PyObject *, PyObject *);

static PyTypeObject CountMin64Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.CountMin64",         /*tp_name*/
    sizeof(CountMin64),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) Sketch_dealloc<CountMin64>,/*tp_dealloc*/
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
    "CountMin sketch for numbers within I={2^64}", /* tp_doc */
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
    (initproc)CountMin_init<uint64_t, uint192 >,     /* tp_init */
    0,                              /* tp_alloc */
    0,                              /* tp_new */
};

static PyMethodDef CountMin64_methods[] = {
    {"update", (PyCFunction)Sketch_update<CountMin64, uint64_t>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)Sketch_difference<CountMin64, CountMin_Sketch<uint64_t>, uint64_t>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<CountMin64>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"first_moment", (PyCFunction)Sketch_first_moment<CountMin64>, METH_NOARGS,
     "Provides and estimation to the first moment of the sketch"
    },
    {"copy", (PyCFunction)Sketch_copy<CountMin64, CountMin_Sketch<uint64_t> >, METH_NOARGS,
     "Copies a sketch"
    },
    {"clear", (PyCFunction)Sketch_clear<CountMin64>, METH_NOARGS,
     "Copies a sketch"
    },
    {"get_key_size", (PyCFunction)Sketch_get_key_size<CountMin64>, METH_NOARGS,
     "Returns the size of the key"
    },
    {NULL}  /* Sentinel */
};

/******************************** uint128_t ************************************/
typedef ttmath::UInt<2> uint128;
typedef ttmath::UInt<17> uint1042;
typedef CountMin<uint128> CountMin128;
template static void Sketch_dealloc<CountMin128>(CountMin128*);
template static int CountMin_init< uint128, uint1042> (CountMin128 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<CountMin128, CountMin_Sketch<uint128>, uint128>(CountMin128* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<CountMin128>(CountMin128* , PyObject *, PyObject *);
template static PyObject * Sketch_first_moment<CountMin128>(CountMin128* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<CountMin128, CountMin_Sketch<uint128> >(CountMin128* );
template static PyObject * Sketch_new<CountMin128>(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<CountMin128,uint128>(CountMin128* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<CountMin128>(CountMin128* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<CountMin128>(CountMin128* , PyObject *, PyObject *);

static PyTypeObject CountMin128Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.CountMin128",         /*tp_name*/
    sizeof(CountMin128),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) Sketch_dealloc<CountMin128>,/*tp_dealloc*/
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
    "CountMin sketch for numbers within I={2^128}", /* tp_doc */
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
    (initproc)CountMin_init<uint128, uint1042 >,     /* tp_init */
    0,                              /* tp_alloc */
    0,                              /* tp_new */
};

static PyMethodDef CountMin128_methods[] = {
    {"update", (PyCFunction)Sketch_update<CountMin128, uint128>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)Sketch_difference<CountMin128, CountMin_Sketch<uint128>, uint128>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<CountMin128>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"first_moment", (PyCFunction)Sketch_first_moment<CountMin128>, METH_NOARGS,
     "Provides and estimation to the first moment of the sketch"
    },
    {"copy", (PyCFunction)Sketch_copy<CountMin128, CountMin_Sketch<uint128> >, METH_NOARGS,
     "Copies a sketch"
    },
    {"clear", (PyCFunction)Sketch_clear<CountMin128>, METH_NOARGS,
     "Copies a sketch"
    },
    {"get_key_size", (PyCFunction)Sketch_get_key_size<CountMin128>, METH_NOARGS,
     "Returns the size of the key"
    },
    {NULL}  /* Sentinel */
};

#endif
