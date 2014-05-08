#ifndef SKETCHES_PY_FAGMSSKETCH_H
#define SKETCHES_PY_FAGMSSKETCH_H

#include "py_sketches.h"

/************************** FAGMS sketch **********************************/
template<typename T>
struct FAGMS {
    PyObject_HEAD
    FAGMS_Sketch<T> * sketch;
};


template<typename KeyType, typename PrimeSpaceType>
static int
FAGMS_init(FAGMS<KeyType> *self, PyObject *args, PyObject *kwds)
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
    
    // Generate num_rows random variables:
    Xi_CW4<KeyType, PrimeSpaceType>** xis;
    xis = new Xi_CW4<KeyType, PrimeSpaceType>*[rows];
    for (unsigned int i =0; i < rows; i++) {
        xis[i] = new Xi_CW4<KeyType, PrimeSpaceType>();
    }
    
    // Create the sketch:
    if ( self == NULL ) return -1;
    self->sketch = new FAGMS_Sketch<KeyType>(buckets, rows, 
                            (Hash<KeyType>**) hashes, (Xi<KeyType>**) xis);

    return 0;
}


/******************************** uint16_t ************************************/
typedef FAGMS<uint16_t> FAGMS16;
template static void Sketch_dealloc<FAGMS16>(FAGMS16*);
template static int FAGMS_init<uint16_t, uint64_t> (FAGMS16 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<FAGMS16, FAGMS_Sketch<uint16_t>, uint16_t>(FAGMS16* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<FAGMS16>(FAGMS16* , PyObject *, PyObject *);
template static PyObject * Sketch_first_moment<FAGMS16>(FAGMS16* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<FAGMS16, FAGMS_Sketch<uint16_t> >(FAGMS16* );
template static PyObject * Sketch_new<FAGMS16 >(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<FAGMS16,uint16_t>(FAGMS16* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<FAGMS16>(FAGMS16* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<FAGMS16>(FAGMS16* , PyObject *, PyObject *);

static PyTypeObject FAGMS16Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.FAGMS16",         /*tp_name*/
    sizeof(FAGMS<uint16_t>),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) Sketch_dealloc<FAGMS16>,/*tp_dealloc*/
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
    "FAGMS sketch for numbers within I={2^16}", /* tp_doc */
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
    (initproc)FAGMS_init<uint16_t, uint64_t>,     /* tp_init */
    0,                              /* tp_alloc */
    0,                              /* tp_new */
};




static PyMethodDef FAGMS16_methods[] = {
    {"update", (PyCFunction)Sketch_update<FAGMS<uint16_t>,uint16_t>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)Sketch_difference<FAGMS<uint16_t>, FAGMS_Sketch<uint16_t>, uint16_t>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<FAGMS16>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"first_moment", (PyCFunction)Sketch_first_moment<FAGMS16>, METH_NOARGS,
     "Provides and estimation to the first moment of the sketch"
    },
    {"copy", (PyCFunction)Sketch_copy<FAGMS16, FAGMS_Sketch<uint16_t> >, METH_NOARGS,
     "Copies a sketch"
    },
    {"clear", (PyCFunction)Sketch_clear<FAGMS16>, METH_NOARGS,
     "Clears the sketch"
    },
    {"get_key_size", (PyCFunction)Sketch_get_key_size<FAGMS16>, METH_NOARGS,
     "Returns the size of the key"
    },
    {NULL}  /* Sentinel */
};

/******************************** uint32_t ************************************/
typedef FAGMS<uint32_t> FAGMS32;
typedef ttmath::UInt<3> uint192;
template static void Sketch_dealloc<FAGMS32>(FAGMS32*);
template static int FAGMS_init< uint32_t, uint192> (FAGMS32 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<FAGMS32, FAGMS_Sketch<uint32_t>, uint32_t>(FAGMS32* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<FAGMS32>(FAGMS32* , PyObject *, PyObject *);
template static PyObject * Sketch_first_moment<FAGMS32>(FAGMS32* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<FAGMS32, FAGMS_Sketch<uint32_t> >(FAGMS32* );
template static PyObject * Sketch_new<FAGMS32>(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<FAGMS32, uint32_t>(FAGMS32* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<FAGMS32>(FAGMS32* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<FAGMS32>(FAGMS32* , PyObject *, PyObject *);

static PyTypeObject FAGMS32Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.FAGMS32",         /*tp_name*/
    sizeof(FAGMS32),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) Sketch_dealloc<FAGMS32>,/*tp_dealloc*/
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
    "FAGMS sketch for numbers within I={2^32}", /* tp_doc */
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
    (initproc)FAGMS_init<uint32_t, uint192 >,     /* tp_init */
    0,                              /* tp_alloc */
    0,                              /* tp_new */
};

static PyMethodDef FAGMS32_methods[] = {
    {"update", (PyCFunction)Sketch_update<FAGMS32, uint32_t>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)Sketch_difference<FAGMS32, FAGMS_Sketch<uint32_t>, uint32_t>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<FAGMS32>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"first_moment", (PyCFunction)Sketch_first_moment<FAGMS32>, METH_NOARGS,
     "Provides and estimation to the first moment of the sketch"
    },
    {"copy", (PyCFunction)Sketch_copy<FAGMS32, FAGMS_Sketch<uint32_t> >, METH_NOARGS,
     "Copies a sketch"
    },
    {"clear", (PyCFunction)Sketch_clear<FAGMS32>, METH_NOARGS,
     "Copies a sketch"
    },
    {"get_key_size", (PyCFunction)Sketch_get_key_size<FAGMS32>, METH_NOARGS,
     "Returns the size of the key"
    },
    {NULL}  /* Sentinel */
};


/******************************** uint64_t ************************************/
typedef FAGMS<uint64_t> FAGMS64;
template static void Sketch_dealloc<FAGMS64>(FAGMS64*);
template static int FAGMS_init< uint64_t, uint192> (FAGMS64 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<FAGMS64, FAGMS_Sketch<uint64_t>, uint64_t>(FAGMS64* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<FAGMS64>(FAGMS64* , PyObject *, PyObject *);
template static PyObject * Sketch_first_moment<FAGMS64>(FAGMS64* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<FAGMS64, FAGMS_Sketch<uint64_t> >(FAGMS64* );
template static PyObject * Sketch_new<FAGMS64>(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<FAGMS64, uint64_t>(FAGMS64* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<FAGMS64>(FAGMS64* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<FAGMS64>(FAGMS64* , PyObject *, PyObject *);

static PyTypeObject FAGMS64Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.FAGMS64",         /*tp_name*/
    sizeof(FAGMS64),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) Sketch_dealloc<FAGMS64>,/*tp_dealloc*/
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
    "FAGMS sketch for numbers within I={2^64}", /* tp_doc */
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
    (initproc)FAGMS_init<uint64_t, uint192 >,     /* tp_init */
    0,                              /* tp_alloc */
    0,                              /* tp_new */
};

static PyMethodDef FAGMS64_methods[] = {
    {"update", (PyCFunction)Sketch_update<FAGMS64, uint64_t>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)Sketch_difference<FAGMS64, FAGMS_Sketch<uint64_t>, uint64_t>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<FAGMS64>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"first_moment", (PyCFunction)Sketch_first_moment<FAGMS64>, METH_NOARGS,
     "Provides and estimation to the first moment of the sketch"
    },
    {"copy", (PyCFunction)Sketch_copy<FAGMS64, FAGMS_Sketch<uint64_t> >, METH_NOARGS,
     "Copies a sketch"
    },
    {"clear", (PyCFunction)Sketch_clear<FAGMS64>, METH_NOARGS,
     "Copies a sketch"
    },
    {"get_key_size", (PyCFunction)Sketch_get_key_size<FAGMS64>, METH_NOARGS,
     "Returns the size of the key"
    },
    {NULL}  /* Sentinel */
};

/******************************** uint128_t ************************************/
typedef ttmath::UInt<2> uint128;
typedef ttmath::UInt<17> uint1042;
typedef FAGMS<uint128> FAGMS128;
template static void Sketch_dealloc<FAGMS128>(FAGMS128*);
template static int FAGMS_init< uint128, uint1042> (FAGMS128 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<FAGMS128, FAGMS_Sketch<uint128>, uint128>(FAGMS128* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<FAGMS128>(FAGMS128* , PyObject *, PyObject *);
template static PyObject * Sketch_first_moment<FAGMS128>(FAGMS128* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<FAGMS128, FAGMS_Sketch<uint128> >(FAGMS128* );
template static PyObject * Sketch_new<FAGMS128>(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<FAGMS128,uint128>(FAGMS128* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<FAGMS128>(FAGMS128* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<FAGMS128>(FAGMS128* , PyObject *, PyObject *);

static PyTypeObject FAGMS128Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.FAGMS128",         /*tp_name*/
    sizeof(FAGMS128),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) Sketch_dealloc<FAGMS128>,/*tp_dealloc*/
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
    "FAGMS sketch for numbers within I={2^128}", /* tp_doc */
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
    (initproc)FAGMS_init<uint128, uint1042 >,     /* tp_init */
    0,                              /* tp_alloc */
    0,                              /* tp_new */
};

static PyMethodDef FAGMS128_methods[] = {
    {"update", (PyCFunction)Sketch_update<FAGMS128, uint128>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)Sketch_difference<FAGMS128, FAGMS_Sketch<uint128>, uint128>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<FAGMS128>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"first_moment", (PyCFunction)Sketch_first_moment<FAGMS128>, METH_NOARGS,
     "Provides and estimation to the first moment of the sketch"
    },
    {"copy", (PyCFunction)Sketch_copy<FAGMS128, FAGMS_Sketch<uint128> >, METH_NOARGS,
     "Copies a sketch"
    },
    {"clear", (PyCFunction)Sketch_clear<FAGMS128>, METH_NOARGS,
     "Copies a sketch"
    },
    {"get_key_size", (PyCFunction)Sketch_get_key_size<FAGMS128>, METH_NOARGS,
     "Returns the size of the key"
    },
    {NULL}  /* Sentinel */
};

#endif
