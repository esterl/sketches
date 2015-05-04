import textwrap 

def create_py_files():
    sketches = [{
        'type': 'FastCount',
        'init': textwrap.dedent("""\
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
                
                if (strcmp(random_generator, "cw4") == 0) {
                    hashes = (Hash<KeyType>**) new Hash_CW4<KeyType, PrimeSpaceType>*[rows];
                    for (unsigned int i =0; i < rows; i++) {
                        hashes[i] = (Hash<KeyType>*) new Hash_CW4<KeyType, PrimeSpaceType>(buckets);
                    }
                } else if (strcmp(random_generator, "cw2") == 0) {
                    hashes = (Hash<KeyType>**) new Hash_CW2<KeyType, PrimeSpaceType>*[rows];
                    for (unsigned int i =0; i < rows; i++) {
                        hashes[i] = (Hash<KeyType>*) new Hash_CW2<KeyType, PrimeSpaceType>(buckets);
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
            """)
        }, {
        'type': 'FAGMS',
        'init': textwrap.dedent("""\
            template<typename KeyType, typename PrimeSpaceType>
            static int
            FAGMS_init(FAGMS<KeyType> *self, PyObject *args, PyObject *kwds)
            {
                unsigned int buckets, rows;
                const char * random_generator = "cw";
                const char * avg_func = "median";
                static char *kwlist[] = {"num_buckets", "num_rows", 
                                         "random_generator", "average_function",
                                         NULL};
                
                if (! PyArg_ParseTupleAndKeywords(args, kwds, "II|s", kwlist, 
                            &buckets, &rows, &random_generator, &avg_func))
                    return -1;
                
                // Generate num_rows random hashes:
                Hash_CW2<KeyType, PrimeSpaceType>** hashes;
                hashes = new Hash_CW2<KeyType, PrimeSpaceType>*[rows];
                for (unsigned int i =0; i < rows; i++) {
                    hashes[i] = new Hash_CW2<KeyType, PrimeSpaceType>(buckets);
                }
                
                // Generate num_rows random variables:
                Xi<KeyType> ** xis;
                if (strcmp(random_generator, "cw4") == 0) {
                    xis =(Xi<KeyType>**) new Xi_CW4<KeyType, PrimeSpaceType>*[rows];
                    for (unsigned int i =0; i < rows; i++) {
                        xis[i] = (Xi<KeyType>*) new Xi_CW4<KeyType, PrimeSpaceType>();
                    }
                } else if (strcmp(random_generator, "cw2") == 0) {
                    xis = (Xi<KeyType>**) new Xi_CW2<KeyType, PrimeSpaceType>*[rows];
                    for (unsigned int i =0; i < rows; i++) {
                        xis[i] = (Xi<KeyType>*) new Xi_CW2<KeyType, PrimeSpaceType>();
                    }
                } else if (strcmp(random_generator, "bch5") == 0) {
                    xis = (Xi<KeyType>**) new Xi_BCH5<KeyType>*[rows];
                    for (unsigned int i =0; i < rows; i++) {
                        xis[i] = (Xi<KeyType>*) new Xi_BCH5<KeyType>();
                    }
                } else if (strcmp(random_generator, "bch3") == 0) {
                    xis = (Xi<KeyType>**) new Xi_BCH3<KeyType>*[rows];
                    for (unsigned int i =0; i < rows; i++) {
                        xis[i] = (Xi<KeyType>*) new Xi_BCH3<KeyType>();
                    }
                } else if (strcmp(random_generator, "eh3") == 0) {
                    xis = (Xi<KeyType>**) new Xi_EH3<KeyType>*[rows];
                    for (unsigned int i =0; i < rows; i++) {
                        xis[i] = (Xi<KeyType>*) new Xi_EH3<KeyType>();
                    }
                }else {
                    throw std::invalid_argument("Unknown random generator type");
                }
                
                // Create the sketch:
                if ( self == NULL ) return -1;
                self->sketch = new FAGMS_Sketch<KeyType>(buckets, rows, 
                                        (Hash<KeyType>**) hashes, xis, avg_func);
                
                return 0;
            }
            """)
        }, {
        'type': 'AGMS',
        'init': textwrap.dedent("""\
            template<typename KeyType, typename PrimeSpaceType>
            static int
            AGMS_init(AGMS<KeyType> *self, PyObject *args, PyObject *kwds)
            {
                unsigned int buckets, rows;
                const char * random_generator = "cw";
                const char * avg_func = "median";
                static char *kwlist[] = {"num_buckets", "num_rows",
                                         "random_generator", "average_function", 
                                         NULL};
                
                if (! PyArg_ParseTupleAndKeywords(args, kwds, "II|s", kwlist, 
                            &buckets, &rows, &random_generator, &avg_func))
                    return -1;
                
                // Generate num_rows random hashes:
                Xi<KeyType> ** xis;
                if (strcmp(random_generator, "cw4") == 0) {
                    xis = (Xi<KeyType>**) new Xi_CW4<KeyType, PrimeSpaceType>*[rows*buckets];
                    for (unsigned i =0; i < rows*buckets; i++) {
                        xis[i] = (Xi<KeyType>*) new Xi_CW4<KeyType, PrimeSpaceType>();
                    }
                } else if (strcmp(random_generator, "cw2") == 0) {
                    xis = (Xi<KeyType>**) new Xi_CW2<KeyType, PrimeSpaceType>*[rows*buckets];
                    for (unsigned i=0; i < rows*buckets; i++) {
                        xis[i] = (Xi<KeyType>*) new Xi_CW2<KeyType, PrimeSpaceType>();
                    }
                } else if (strcmp(random_generator, "bch5") == 0) {
                    xis = (Xi<KeyType>**) new Xi_BCH5<KeyType>*[rows*buckets];
                    for (unsigned i =0; i < rows*buckets; i++) {
                        xis[i] = (Xi<KeyType>*) new Xi_BCH5<KeyType>();
                    }
                } else if (strcmp(random_generator, "bch3") == 0) {
                    xis = (Xi<KeyType>**) new Xi_BCH3<KeyType>*[rows*buckets];
                    for (unsigned i =0; i < rows*buckets; i++) {
                        xis[i] = (Xi<KeyType>*) new Xi_BCH3<KeyType>();
                    }
                } else if (strcmp(random_generator, "eh3") == 0) {
                    xis = (Xi<KeyType>**) new Xi_EH3<KeyType>*[rows*buckets];
                    for (unsigned i =0; i < rows*buckets; i++) {
                        xis[i] = (Xi<KeyType>*) new Xi_EH3<KeyType>();
                    }
                } else {
                    throw std::invalid_argument("Unknown random generator type");
                }
                // Create the sketch:
                if ( self == NULL ) return -1;
                self->sketch = new AGMS_Sketch<KeyType>(buckets, rows, xis, avg_func);

                return 0;
            }
            """)
        }, {
        'type': 'CountMin',
        'init': textwrap.dedent("""\
            template<typename KeyType, typename PrimeSpaceType>
            static int
            CountMin_init(CountMin<KeyType> *self, PyObject *args, PyObject *kwds)
            {
                unsigned int buckets, rows;
                const char * random_generator = "cw";
                static char *kwlist[] = {"num_buckets", "num_rows", "random_generator", NULL};
                
                if (! PyArg_ParseTupleAndKeywords(args, kwds, "II|s", kwlist, &buckets, &rows, &random_generator))
                    return -1;
                
                // Generate num_rows random hashes:
                Hash<KeyType>** hashes;
                
                if (strcmp(random_generator, "cw2") == 0) {
                    hashes = (Hash<KeyType>**) new Hash_CW2<KeyType, PrimeSpaceType>*[rows];
                    for (unsigned int i =0; i < rows; i++) {
                        hashes[i] = (Hash<KeyType>*) new Hash_CW2<KeyType, PrimeSpaceType>(buckets);
                    }
                } else {
                    throw std::invalid_argument("Unknown random generator type");
                }
                
                // Create the sketch:
                if ( self == NULL ) return -1;
                self->sketch = new CountMin_Sketch<KeyType>(buckets, rows, hashes);

                return 0;
            }
            """)
        }, 
    ]

    header = textwrap.dedent("""\
        #ifndef SKETCHES_PY_%(type_upper)sSKETCH_H
        #define SKETCHES_PY_%(type_upper)sSKETCH_H

        #include <string.h>
        #include <stdexcept> 
        #include "py_sketches.h"
        #include "mersenne.h"

        /************************** %(type)s sketch **********************************/
        template<typename T>
        struct %(type)s {
            PyObject_HEAD
            %(type)s_Sketch<T> * sketch;
        };
        """)

    footer = textwrap.dedent("""\
        #endif
        """)

    declarations = textwrap.dedent("""\
        typedef %(type)s<uint%(len)s_t> %(type)s%(len)s;
        template static void Sketch_dealloc<%(type)s%(len)s>(%(type)s%(len)s*);
        template static int %(type)s_init<uint%(len)s_t, %(prime)s_t> (%(type)s%(len)s *, PyObject *, PyObject *);
        template static PyObject * Sketch_difference<%(type)s%(len)s, %(type)s_Sketch<uint%(len)s_t>, uint%(len)s_t>(%(type)s%(len)s* , PyObject *, PyObject *);
        template static PyObject * Sketch_second_moment<%(type)s%(len)s>(%(type)s%(len)s* , PyObject *, PyObject *);
        template static PyObject * Sketch_first_moment<%(type)s%(len)s>(%(type)s%(len)s* , PyObject *, PyObject *);
        template static PyObject * Sketch_copy<%(type)s%(len)s, %(type)s_Sketch<uint%(len)s_t> >(%(type)s%(len)s* );
        template static PyObject * Sketch_new<%(type)s%(len)s >(PyTypeObject *, PyObject *, PyObject *);
        template static PyObject * Sketch_update<%(type)s%(len)s,uint%(len)s_t>(%(type)s%(len)s* , PyObject *, PyObject *);
        template static PyObject * Sketch_clear<%(type)s%(len)s>(%(type)s%(len)s* , PyObject *, PyObject *);
        template static PyObject * Sketch_get_key_size<%(type)s%(len)s>(%(type)s%(len)s* , PyObject *, PyObject *);
        template static PyObject * Sketch_get_rows<%(type)s%(len)s>(%(type)s%(len)s* , PyObject *, PyObject *);
        template static PyObject * Sketch_get_columns<%(type)s%(len)s>(%(type)s%(len)s* , PyObject *, PyObject *);
        template static PyObject * Sketch_iadd<%(type)s%(len)s>(%(type)s%(len)s*, PyObject *, PyObject *);
        template static PyObject * Sketch_get_max<%(type)s%(len)s>(%(type)s%(len)s* , PyObject *, PyObject *);
        template static PyObject * Sketch_inner_product<%(type)s%(len)s, uint%(len)s_t>(%(type)s%(len)s* , PyObject *, PyObject *);
        template static PyObject * Sketch_get_counters<%(type)s%(len)s>(%(type)s%(len)s* , PyObject *, PyObject *);
        
        static PyTypeObject %(type)s%(len)sType = {
            PyObject_HEAD_INIT(NULL)
            0,                              /*ob_size*/
            "sketches.%(type)s%(len)s",         /*tp_name*/
            sizeof(%(type)s<uint%(len)s_t>),            /*tp_basicsize*/
            0,                              /*tp_itemsize*/
            (destructor) Sketch_dealloc<%(type)s%(len)s>,/*tp_dealloc*/
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
            "%(type)s sketch for numbers within I={2^%(len)s}", /* tp_doc */
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
            (initproc)%(type)s_init<uint%(len)s_t, %(prime)s_t>,     /* tp_init */
            0,                              /* tp_alloc */
            0,                              /* tp_new */
        };
        
        static PyMethodDef %(type)s%(len)s_methods[] = {
            {"update", (PyCFunction)Sketch_update<%(type)s<uint%(len)s_t>,uint%(len)s_t>, METH_VARARGS|METH_KEYWORDS,
             "Updates the sketch with the given key and value"
            },
            {"difference", (PyCFunction)Sketch_difference<%(type)s<uint%(len)s_t>, %(type)s_Sketch<uint%(len)s_t>, uint%(len)s_t>, METH_VARARGS|METH_KEYWORDS,
             "Provides and estimation to the L2 difference between the sketches"
            },
            {"second_moment", (PyCFunction)Sketch_second_moment<%(type)s%(len)s>, METH_NOARGS,
             "Provides and estimation to the second moment of the sketch"
            },
            {"first_moment", (PyCFunction)Sketch_first_moment<%(type)s%(len)s>, METH_NOARGS,
             "Provides and estimation to the first moment of the sketch"
            },
            {"copy", (PyCFunction)Sketch_copy<%(type)s%(len)s, %(type)s_Sketch<uint%(len)s_t> >, METH_NOARGS,
             "Copies a sketch"
            },
            {"clear", (PyCFunction)Sketch_clear<%(type)s%(len)s>, METH_NOARGS,
             "Clears the sketch"
            },
            {"get_key_size", (PyCFunction)Sketch_get_key_size<%(type)s%(len)s>, METH_NOARGS,
             "Returns the size of the key"
            },
            {"get_rows", (PyCFunction)Sketch_get_rows<%(type)s%(len)s>, METH_NOARGS,
             "Returns the number of rows of the sketch"
            },
            {"get_columns", (PyCFunction)Sketch_get_columns<%(type)s%(len)s>, METH_NOARGS,
             "Returns the number of columns of the sketch"
            },
            {"__iadd__", (PyCFunction)Sketch_iadd<%(type)s%(len)s>, METH_VARARGS|METH_KEYWORDS,
            "Inplace add of sketches"
            },
            {"get_max", (PyCFunction)Sketch_get_max<%(type)s%(len)s>, METH_NOARGS,
             "Returns the value of the biggest counter"
            },
            {"inner_product", (PyCFunction)Sketch_inner_product<%(type)s%(len)s, uint%(len)s_t>, METH_VARARGS|METH_KEYWORDS,
            "Computes the inner product between two sketches"
            },
            {"get_counters", (PyCFunction)Sketch_get_counters<%(type)s%(len)s>, METH_NOARGS,
             "Returns the counters of the sketch"
            },
            {NULL}  /* Sentinel */
        };
        """)

    for sketch in sketches:
        header_sketch = header % { 
                            'type': sketch['type'],
                            'type_upper': sketch['type'].upper()
                        }
        
        content = '\n'.join([
            declarations % {
                'type': sketch['type'],
                'len': str(i),
                'prime': j
            } for i,j in ((8, 'prime13'), (16, 'prime17'), (32, 'prime61'), (64, 'prime89'), (128, 'prime521'))
        ])
        
        with open('py_%ssketch.h' % sketch['type'].lower(), 'w') as sketch_file :
            sketch_file.write(header_sketch)
            sketch_file.write(sketch['init'])
            sketch_file.write(content)
            sketch_file.write(footer)
