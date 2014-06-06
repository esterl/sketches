#ifndef SKETCHES_PY_FASTCOUNTSKETCH_H
#define SKETCHES_PY_FASTCOUNTSKETCH_H

#include <string.h>
#include <stdexcept> 
#include "py_sketches.h"
#include "mersenne.h"

/************************** FastCount sketch **********************************/
template<typename T>
struct FastCount {
    PyObject_HEAD
    FastCount_Sketch<T> * sketch;
};


template<typename KeyType, typename PrimeSpaceType>
static int
FastCount_init(FastCount<KeyType> *self, PyObject *args, PyObject *kwds)
{
    unsigned int buckets, rows;
    const char * random_generator = "cw";
    static char *kwlist[] = {"num_buckets", "num_rows", "random_generator", NULL};
    
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "II|s", kwlist, &buckets, &rows, &random_generator))
        return -1;
    
    // Generate num_rows random hashes:
    Hash<KeyType>** hashes;
    
    if (strcmp(random_generator, "cw") == 0) {
        hashes = (Hash<KeyType>**) new Hash_CW4<KeyType, PrimeSpaceType>*[rows];
        for (unsigned int i =0; i < rows; i++) {
            hashes[i] = (Hash<KeyType>*) new Hash_CW4<KeyType, PrimeSpaceType>(buckets);
        }
    } else if (strcmp(random_generator, "tab") == 0) {
        hashes = (Hash<KeyType>**) new Hash_Tab<KeyType>*[rows];
        for (unsigned int i =0; i < rows; i++) {
            hashes[i] = (Hash<KeyType>*) new Hash_Tab<KeyType>(buckets);
        }
    } else {
        throw std::invalid_argument("Unknown random generator type");
    }
    // Create the sketch:
    if ( self == NULL ) return -1;
    self->sketch = new FastCount_Sketch<KeyType>(buckets, rows, hashes);

    return 0;
}


/******************************** uint16_t ************************************/
typedef FastCount<uint16_t> FastCount16;
template static void Sketch_dealloc<FastCount16>(FastCount16*);
template static int FastCount_init<uint16_t, uint64_t> (FastCount16 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<FastCount16, FastCount_Sketch<uint16_t>, uint16_t>(FastCount16* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<FastCount16>(FastCount16* , PyObject *, PyObject *);
template static PyObject * Sketch_first_moment<FastCount16>(FastCount16* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<FastCount16, FastCount_Sketch<uint16_t> >(FastCount16* );
template static PyObject * Sketch_new<FastCount16 >(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<FastCount16,uint16_t>(FastCount16* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<FastCount16>(FastCount16* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<FastCount16>(FastCount16* , PyObject *, PyObject *);
template static PyObject * Sketch_get_rows<FastCount16>(FastCount16* , PyObject *, PyObject *);
template static PyObject * Sketch_get_columns<FastCount16>(FastCount16* , PyObject *, PyObject *);

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
    {"difference", (PyCFunction)Sketch_difference<FastCount<uint16_t>, FastCount_Sketch<uint16_t>, uint16_t>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<FastCount16>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"first_moment", (PyCFunction)Sketch_first_moment<FastCount16>, METH_NOARGS,
     "Provides and estimation to the first moment of the sketch"
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
    {"get_rows", (PyCFunction)Sketch_get_rows<FastCount16>, METH_NOARGS,
     "Returns the number of rows of the sketch"
    },
    {"get_columns", (PyCFunction)Sketch_get_columns<FastCount16>, METH_NOARGS,
     "Returns the number of columns of the sketch"
    },
    {NULL}  /* Sentinel */
};

/******************************** uint32_t ************************************/
typedef FastCount<uint32_t> FastCount32;
template static void Sketch_dealloc<FastCount32>(FastCount32*);
template static int FastCount_init< uint32_t, prime61_t> (FastCount32 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<FastCount32, FastCount_Sketch<uint32_t>, uint32_t>(FastCount32* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<FastCount32>(FastCount32* , PyObject *, PyObject *);
template static PyObject * Sketch_first_moment<FastCount32>(FastCount32* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<FastCount32, FastCount_Sketch<uint32_t> >(FastCount32* );
template static PyObject * Sketch_new<FastCount32>(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<FastCount32, uint32_t>(FastCount32* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<FastCount32>(FastCount32* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<FastCount32>(FastCount32* , PyObject *, PyObject *);
template static PyObject * Sketch_get_rows<FastCount32>(FastCount32* , PyObject *, PyObject *);
template static PyObject * Sketch_get_columns<FastCount32>(FastCount32* , PyObject *, PyObject *);

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
    (initproc)FastCount_init<uint32_t, prime61_t >,     /* tp_init */
    0,                              /* tp_alloc */
    0,                              /* tp_new */
};

static PyMethodDef FastCount32_methods[] = {
    {"update", (PyCFunction)Sketch_update<FastCount32, uint32_t>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)Sketch_difference<FastCount32, FastCount_Sketch<uint32_t>, uint32_t>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<FastCount32>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"first_moment", (PyCFunction)Sketch_first_moment<FastCount32>, METH_NOARGS,
     "Provides and estimation to the first moment of the sketch"
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
    {"get_rows", (PyCFunction)Sketch_get_rows<FastCount32>, METH_NOARGS,
     "Returns the number of rows of the sketch"
    },
    {"get_columns", (PyCFunction)Sketch_get_columns<FastCount32>, METH_NOARGS,
     "Returns the number of columns of the sketch"
    },
    {NULL}  /* Sentinel */
};


/******************************** uint64_t ************************************/
typedef FastCount<uint64_t> FastCount64;
template static void Sketch_dealloc<FastCount64>(FastCount64*);
template static int FastCount_init< uint64_t, prime61_t> (FastCount64 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<FastCount64, FastCount_Sketch<uint64_t>, uint64_t>(FastCount64* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<FastCount64>(FastCount64* , PyObject *, PyObject *);
template static PyObject * Sketch_first_moment<FastCount64>(FastCount64* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<FastCount64, FastCount_Sketch<uint64_t> >(FastCount64* );
template static PyObject * Sketch_new<FastCount64>(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<FastCount64, uint64_t>(FastCount64* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<FastCount64>(FastCount64* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<FastCount64>(FastCount64* , PyObject *, PyObject *);
template static PyObject * Sketch_get_rows<FastCount64>(FastCount64* , PyObject *, PyObject *);
template static PyObject * Sketch_get_columns<FastCount64>(FastCount64* , PyObject *, PyObject *);

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
    (initproc)FastCount_init<uint64_t, prime61_t >,     /* tp_init */
    0,                              /* tp_alloc */
    0,                              /* tp_new */
};

static PyMethodDef FastCount64_methods[] = {
    {"update", (PyCFunction)Sketch_update<FastCount64, uint64_t>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)Sketch_difference<FastCount64, FastCount_Sketch<uint64_t>, uint64_t>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<FastCount64>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"first_moment", (PyCFunction)Sketch_first_moment<FastCount64>, METH_NOARGS,
     "Provides and estimation to the first moment of the sketch"
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
    {"get_rows", (PyCFunction)Sketch_get_rows<FastCount64>, METH_NOARGS,
     "Returns the number of rows of the sketch"
    },
    {"get_columns", (PyCFunction)Sketch_get_columns<FastCount64>, METH_NOARGS,
     "Returns the number of columns of the sketch"
    },
    {NULL}  /* Sentinel */
};

/******************************** uint128_t ************************************/
typedef FastCount<uint128> FastCount128;
template static void Sketch_dealloc<FastCount128>(FastCount128*);
template static int FastCount_init< uint128, prime521_t> (FastCount128 *, PyObject *, PyObject *);
template static PyObject * Sketch_difference<FastCount128, FastCount_Sketch<uint128>, uint128>(FastCount128* , PyObject *, PyObject *);
template static PyObject * Sketch_second_moment<FastCount128>(FastCount128* , PyObject *, PyObject *);
template static PyObject * Sketch_first_moment<FastCount128>(FastCount128* , PyObject *, PyObject *);
template static PyObject * Sketch_copy<FastCount128, FastCount_Sketch<uint128> >(FastCount128* );
template static PyObject * Sketch_new<FastCount128>(PyTypeObject *, PyObject *, PyObject *);
template static PyObject * Sketch_update<FastCount128,uint128>(FastCount128* , PyObject *, PyObject *);
template static PyObject * Sketch_clear<FastCount128>(FastCount128* , PyObject *, PyObject *);
template static PyObject * Sketch_get_key_size<FastCount128>(FastCount128* , PyObject *, PyObject *);
template static PyObject * Sketch_get_rows<FastCount128>(FastCount128* , PyObject *, PyObject *);
template static PyObject * Sketch_get_columns<FastCount128>(FastCount128* , PyObject *, PyObject *);

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
    (initproc)FastCount_init<uint128, prime521_t >,     /* tp_init */
    0,                              /* tp_alloc */
    0,                              /* tp_new */
};

static PyMethodDef FastCount128_methods[] = {
    {"update", (PyCFunction)Sketch_update<FastCount128, uint128>, METH_VARARGS|METH_KEYWORDS,
     "Updates the sketch with the given key and value"
    },
    {"difference", (PyCFunction)Sketch_difference<FastCount128, FastCount_Sketch<uint128>, uint128>, METH_VARARGS|METH_KEYWORDS,
     "Provides and estimation to the L2 difference between the sketches"
    },
    {"second_moment", (PyCFunction)Sketch_second_moment<FastCount128>, METH_NOARGS,
     "Provides and estimation to the second moment of the sketch"
    },
    {"first_moment", (PyCFunction)Sketch_first_moment<FastCount128>, METH_NOARGS,
     "Provides and estimation to the first moment of the sketch"
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
    {"get_rows", (PyCFunction)Sketch_get_rows<FastCount128>, METH_NOARGS,
     "Returns the number of rows of the sketch"
    },
    {"get_columns", (PyCFunction)Sketch_get_columns<FastCount128>, METH_NOARGS,
     "Returns the size of the key"
    },
    {NULL}  /* Sentinel */
};

#endif
