#ifndef SKETCHES_PY_AGMSSKETCH_H
#define SKETCHES_PY_AGMSSKETCH_H

#include "py_sketches.h"

/************************** AGMS sketch **********************************/
template<typename T>
struct AGMS {
    PyObject_HEAD
    AGMS_Sketch<T> * sketch;
};


template<typename KeyType, typename PrimeSpaceType>
static int
AGMS_init(AGMS<KeyType> *self, PyObject *args, PyObject *kwds)
{
    unsigned int buckets, rows;
    static char *kwlist[] = {"num_buckets", "num_rows", NULL};
    
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "|II", kwlist, &buckets, &rows))
        return -1;
    
    // Generate num_rows random hashes:
    Xi_CW4<KeyType, PrimeSpaceType>** xis;
    xis = new Xi_CW4<KeyType, PrimeSpaceType>*[rows*buckets];
    for (unsigned int i =0; i < rows*buckets; i++) {
        xis[i] = new Xi_CW4<KeyType, PrimeSpaceType>();
    }
    
    // Create the sketch:
    if ( self == NULL ) return -1;
    self->sketch = new AGMS_Sketch<KeyType>(buckets, rows, (Xi<KeyType>**) xis);

    return 0;
}


/******************************** uint16_t ************************************/
typedef AGMS<uint16_t> AGMS16;
template static void Sketch_dealloc<AGMS16>(AGMS16*);
template static int AGMS_init<uint16_t, uint64_t> (AGMS16 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<AGMS16, AGMS_Sketch<uint16_t>, uint16_t>(AGMS16* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<AGMS16>(AGMS16* , PyObject *, PyObject *);
template static PyObject * Sketch_first_moment<AGMS16>(AGMS16* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<AGMS16, AGMS_Sketch<uint16_t> >(AGMS16* );
template static PyObject * Sketch_new<AGMS16 >(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<AGMS16,uint16_t>(AGMS16* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<AGMS16>(AGMS16* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<AGMS16>(AGMS16* , PyObject *, PyObject *);
template static PyObject * Sketch_get_rows<AGMS16>(AGMS16* , PyObject *, PyObject *);
template static PyObject * Sketch_get_columns<AGMS16>(AGMS16* , PyObject *, PyObject *);

static PyTypeObject AGMS16Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.AGMS16",         /*tp_name*/
    sizeof(AGMS<uint16_t>),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) Sketch_dealloc<AGMS16>,/*tp_dealloc*/
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
    "AGMS sketch for numbers within I={2^16}", /* tp_doc */
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
    (initproc)AGMS_init<uint16_t, uint64_t>,     /* tp_init */
    0,                              /* tp_alloc */
    0,                              /* tp_new */
};




static PyMethodDef AGMS16_methods[] = {
    {"update", (PyCFunction)Sketch_update<AGMS<uint16_t>,uint16_t>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)Sketch_difference<AGMS<uint16_t>, AGMS_Sketch<uint16_t>, uint16_t>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<AGMS16>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"first_moment", (PyCFunction)Sketch_first_moment<AGMS16>, METH_NOARGS,
     "Provides and estimation to the first moment of the sketch"
    },
    {"copy", (PyCFunction)Sketch_copy<AGMS16, AGMS_Sketch<uint16_t> >, METH_NOARGS,
     "Copies a sketch"
    },
    {"clear", (PyCFunction)Sketch_clear<AGMS16>, METH_NOARGS,
     "Clears the sketch"
    },
    {"get_key_size", (PyCFunction)Sketch_get_key_size<AGMS16>, METH_NOARGS,
     "Returns the size of the key"
    },
    {"get_rows", (PyCFunction)Sketch_get_rows<AGMS16>, METH_NOARGS,
     "Returns the number of rows of the sketch"
    },
    {"get_columns", (PyCFunction)Sketch_get_columns<AGMS16>, METH_NOARGS,
     "Returns the number of columns of the sketch"
    },
    {NULL}  /* Sentinel */
};

/******************************** uint32_t ************************************/
typedef AGMS<uint32_t> AGMS32;
typedef ttmath::UInt<3> uint192;
template static void Sketch_dealloc<AGMS32>(AGMS32*);
template static int AGMS_init< uint32_t, uint192> (AGMS32 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<AGMS32, AGMS_Sketch<uint32_t>, uint32_t>(AGMS32* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<AGMS32>(AGMS32* , PyObject *, PyObject *);
template static PyObject * Sketch_first_moment<AGMS32>(AGMS32* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<AGMS32, AGMS_Sketch<uint32_t> >(AGMS32* );
template static PyObject * Sketch_new<AGMS32>(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<AGMS32, uint32_t>(AGMS32* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<AGMS32>(AGMS32* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<AGMS32>(AGMS32* , PyObject *, PyObject *);
template static PyObject * Sketch_get_rows<AGMS32>(AGMS32* , PyObject *, PyObject *);
template static PyObject * Sketch_get_columns<AGMS32>(AGMS32* , PyObject *, PyObject *);

static PyTypeObject AGMS32Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.AGMS32",         /*tp_name*/
    sizeof(AGMS32),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) Sketch_dealloc<AGMS32>,/*tp_dealloc*/
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
    "AGMS sketch for numbers within I={2^32}", /* tp_doc */
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
    (initproc)AGMS_init<uint32_t, uint192 >,     /* tp_init */
    0,                              /* tp_alloc */
    0,                              /* tp_new */
};

static PyMethodDef AGMS32_methods[] = {
    {"update", (PyCFunction)Sketch_update<AGMS32, uint32_t>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)Sketch_difference<AGMS32, AGMS_Sketch<uint32_t>, uint32_t>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<AGMS32>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"first_moment", (PyCFunction)Sketch_first_moment<AGMS32>, METH_NOARGS,
     "Provides and estimation to the first moment of the sketch"
    },
    {"copy", (PyCFunction)Sketch_copy<AGMS32, AGMS_Sketch<uint32_t> >, METH_NOARGS,
     "Copies a sketch"
    },
    {"clear", (PyCFunction)Sketch_clear<AGMS32>, METH_NOARGS,
     "Copies a sketch"
    },
    {"get_key_size", (PyCFunction)Sketch_get_key_size<AGMS32>, METH_NOARGS,
     "Returns the size of the key"
    },
    {"get_rows", (PyCFunction)Sketch_get_rows<AGMS32>, METH_NOARGS,
     "Returns the number of rows of the sketch"
    },
    {"get_columns", (PyCFunction)Sketch_get_columns<AGMS32>, METH_NOARGS,
     "Returns the number of columns of the sketch"
    },
    {NULL}  /* Sentinel */
};


/******************************** uint64_t ************************************/
typedef AGMS<uint64_t> AGMS64;
template static void Sketch_dealloc<AGMS64>(AGMS64*);
template static int AGMS_init< uint64_t, uint192> (AGMS64 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<AGMS64, AGMS_Sketch<uint64_t>, uint64_t>(AGMS64* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<AGMS64>(AGMS64* , PyObject *, PyObject *);
template static PyObject * Sketch_first_moment<AGMS64>(AGMS64* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<AGMS64, AGMS_Sketch<uint64_t> >(AGMS64* );
template static PyObject * Sketch_new<AGMS64>(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<AGMS64, uint64_t>(AGMS64* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<AGMS64>(AGMS64* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<AGMS64>(AGMS64* , PyObject *, PyObject *);
template static PyObject * Sketch_get_rows<AGMS64>(AGMS64* , PyObject *, PyObject *);
template static PyObject * Sketch_get_columns<AGMS64>(AGMS64* , PyObject *, PyObject *);

static PyTypeObject AGMS64Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.AGMS64",         /*tp_name*/
    sizeof(AGMS64),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) Sketch_dealloc<AGMS64>,/*tp_dealloc*/
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
    "AGMS sketch for numbers within I={2^64}", /* tp_doc */
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
    (initproc)AGMS_init<uint64_t, uint192 >,     /* tp_init */
    0,                              /* tp_alloc */
    0,                              /* tp_new */
};

static PyMethodDef AGMS64_methods[] = {
    {"update", (PyCFunction)Sketch_update<AGMS64, uint64_t>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)Sketch_difference<AGMS64, AGMS_Sketch<uint64_t>, uint64_t>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<AGMS64>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"first_moment", (PyCFunction)Sketch_first_moment<AGMS64>, METH_NOARGS,
     "Provides and estimation to the first moment of the sketch"
    },
    {"copy", (PyCFunction)Sketch_copy<AGMS64, AGMS_Sketch<uint64_t> >, METH_NOARGS,
     "Copies a sketch"
    },
    {"clear", (PyCFunction)Sketch_clear<AGMS64>, METH_NOARGS,
     "Copies a sketch"
    },
    {"get_key_size", (PyCFunction)Sketch_get_key_size<AGMS64>, METH_NOARGS,
     "Returns the size of the key"
    },
    {"get_rows", (PyCFunction)Sketch_get_rows<AGMS64>, METH_NOARGS,
     "Returns the number of rows of the sketch"
    },
    {"get_columns", (PyCFunction)Sketch_get_columns<AGMS64>, METH_NOARGS,
     "Returns the number of columns of the sketch"
    },
    {NULL}  /* Sentinel */
};

/******************************** uint128_t ************************************/
typedef ttmath::UInt<2> uint128;
typedef ttmath::UInt<17> uint1042;
typedef AGMS<uint128> AGMS128;
template static void Sketch_dealloc<AGMS128>(AGMS128*);
template static int AGMS_init< uint128, uint1042> (AGMS128 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<AGMS128, AGMS_Sketch<uint128>, uint128>(AGMS128* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<AGMS128>(AGMS128* , PyObject *, PyObject *);
template static PyObject * Sketch_first_moment<AGMS128>(AGMS128* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<AGMS128, AGMS_Sketch<uint128> >(AGMS128* );
template static PyObject * Sketch_new<AGMS128>(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<AGMS128,uint128>(AGMS128* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<AGMS128>(AGMS128* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<AGMS128>(AGMS128* , PyObject *, PyObject *);
template static PyObject * Sketch_get_rows<AGMS128>(AGMS128* , PyObject *, PyObject *);
template static PyObject * Sketch_get_columns<AGMS128>(AGMS128* , PyObject *, PyObject *);

static PyTypeObject AGMS128Type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /*ob_size*/
    "sketches.AGMS128",         /*tp_name*/
    sizeof(AGMS128),            /*tp_basicsize*/
    0,                              /*tp_itemsize*/
    (destructor) Sketch_dealloc<AGMS128>,/*tp_dealloc*/
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
    "AGMS sketch for numbers within I={2^128}", /* tp_doc */
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
    (initproc)AGMS_init<uint128, uint1042 >,     /* tp_init */
    0,                              /* tp_alloc */
    0,                              /* tp_new */
};

static PyMethodDef AGMS128_methods[] = {
    {"update", (PyCFunction)Sketch_update<AGMS128, uint128>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)Sketch_difference<AGMS128, AGMS_Sketch<uint128>, uint128>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<AGMS128>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"first_moment", (PyCFunction)Sketch_first_moment<AGMS128>, METH_NOARGS,
     "Provides and estimation to the first moment of the sketch"
    },
    {"copy", (PyCFunction)Sketch_copy<AGMS128, AGMS_Sketch<uint128> >, METH_NOARGS,
     "Copies a sketch"
    },
    {"clear", (PyCFunction)Sketch_clear<AGMS128>, METH_NOARGS,
     "Copies a sketch"
    },
    {"get_key_size", (PyCFunction)Sketch_get_key_size<AGMS128>, METH_NOARGS,
     "Returns the size of the key"
    },
    {"get_rows", (PyCFunction)Sketch_get_rows<AGMS128>, METH_NOARGS,
     "Returns the number of rows of the sketch"
    },
    {"get_columns", (PyCFunction)Sketch_get_columns<AGMS128>, METH_NOARGS,
     "Returns the number of columns of the sketch"
    },
    {NULL}  /* Sentinel */
};

#endif
