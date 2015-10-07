#include <Python.h>

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <random>

#include "utils.h"
#include "py_sketches.h"
#include <sketches.h>
#include "py_fastcountsketch.h"
#include "py_countminsketch.h"
#include "py_agmssketch.h"
#include "py_fagmssketch.h"


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
    // Set random seed:
    std::random_device rd;
    std::uniform_int_distribution<int> seed;
    srand((unsigned) seed(rd));
    
    PyObject* m;

    FastCount8Type.tp_new = Sketch_new<FastCount8>;
    FastCount8Type.tp_methods = FastCount8_methods;
    if (PyType_Ready(&FastCount8Type) < 0)
        return;

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

    CountMin8Type.tp_new = Sketch_new<CountMin8>;
    CountMin8Type.tp_methods = CountMin8_methods;
    if (PyType_Ready(&CountMin8Type) < 0)
        return;

    CountMin16Type.tp_new = Sketch_new<CountMin16>;
    CountMin16Type.tp_methods = CountMin16_methods;
    if (PyType_Ready(&CountMin16Type) < 0)
        return;

    CountMin32Type.tp_new = Sketch_new<CountMin32>;
    CountMin32Type.tp_methods = CountMin32_methods;
    if (PyType_Ready(&CountMin32Type) < 0)
        return;

    CountMin64Type.tp_new = Sketch_new<CountMin64>;
    CountMin64Type.tp_methods = CountMin64_methods;
    if (PyType_Ready(&CountMin64Type) < 0)
        return;

    CountMin128Type.tp_new = Sketch_new<CountMin128>;
    CountMin128Type.tp_methods = CountMin128_methods;
    if (PyType_Ready(&CountMin128Type) < 0)
        return;

    AGMS8Type.tp_new = Sketch_new<AGMS8>;
    AGMS8Type.tp_methods = AGMS8_methods;
    if (PyType_Ready(&AGMS8Type) < 0)
        return;

    AGMS16Type.tp_new = Sketch_new<AGMS16>;
    AGMS16Type.tp_methods = AGMS16_methods;
    if (PyType_Ready(&AGMS16Type) < 0)
        return;

    AGMS32Type.tp_new = Sketch_new<AGMS32>;
    AGMS32Type.tp_methods = AGMS32_methods;
    if (PyType_Ready(&AGMS32Type) < 0)
        return;

    AGMS64Type.tp_new = Sketch_new<AGMS64>;
    AGMS64Type.tp_methods = AGMS64_methods;
    if (PyType_Ready(&AGMS64Type) < 0)
        return;

    AGMS128Type.tp_new = Sketch_new<AGMS128>;
    AGMS128Type.tp_methods = AGMS128_methods;
    if (PyType_Ready(&AGMS128Type) < 0)
        return;

    FAGMS8Type.tp_new = Sketch_new<FAGMS8>;
    FAGMS8Type.tp_methods = FAGMS8_methods;
    if (PyType_Ready(&FAGMS8Type) < 0)
        return;

    FAGMS16Type.tp_new = Sketch_new<FAGMS16>;
    FAGMS16Type.tp_methods = FAGMS16_methods;
    if (PyType_Ready(&FAGMS16Type) < 0)
        return;

    FAGMS32Type.tp_new = Sketch_new<FAGMS32>;
    FAGMS32Type.tp_methods = FAGMS32_methods;
    if (PyType_Ready(&FAGMS32Type) < 0)
        return;

    FAGMS64Type.tp_new = Sketch_new<FAGMS64>;
    FAGMS64Type.tp_methods = FAGMS64_methods;
    if (PyType_Ready(&FAGMS64Type) < 0)
        return;

    FAGMS128Type.tp_new = Sketch_new<FAGMS128>;
    FAGMS128Type.tp_methods = FAGMS128_methods;
    if (PyType_Ready(&FAGMS128Type) < 0)
        return;

    m = Py_InitModule3("sketches", sketches_methods,
                       "Module with a list of sketches for data stream 2-moment estimations.");

    Py_INCREF(&FastCount8Type);
    Py_INCREF(&FastCount16Type);
    Py_INCREF(&FastCount32Type);
    Py_INCREF(&FastCount64Type);
    Py_INCREF(&FastCount128Type);
    PyModule_AddObject(m, "FastCount8", (PyObject *)&FastCount8Type);
    PyModule_AddObject(m, "FastCount16", (PyObject *)&FastCount16Type);
    PyModule_AddObject(m, "FastCount32", (PyObject *)&FastCount32Type);
    PyModule_AddObject(m, "FastCount64", (PyObject *)&FastCount64Type);
    PyModule_AddObject(m, "FastCount128", (PyObject *)&FastCount128Type);
    
    Py_INCREF(&CountMin8Type);
    Py_INCREF(&CountMin16Type);
    Py_INCREF(&CountMin32Type);
    Py_INCREF(&CountMin64Type);
    Py_INCREF(&CountMin128Type);
    PyModule_AddObject(m, "CountMin8", (PyObject *)&CountMin8Type);
    PyModule_AddObject(m, "CountMin16", (PyObject *)&CountMin16Type);
    PyModule_AddObject(m, "CountMin32", (PyObject *)&CountMin32Type);
    PyModule_AddObject(m, "CountMin64", (PyObject *)&CountMin64Type);
    PyModule_AddObject(m, "CountMin128", (PyObject *)&CountMin128Type);

    Py_INCREF(&AGMS8Type);
    Py_INCREF(&AGMS16Type);
    Py_INCREF(&AGMS32Type);
    Py_INCREF(&AGMS64Type);
    Py_INCREF(&AGMS128Type);
    PyModule_AddObject(m, "AGMS8", (PyObject *)&AGMS8Type);
    PyModule_AddObject(m, "AGMS16", (PyObject *)&AGMS16Type);
    PyModule_AddObject(m, "AGMS32", (PyObject *)&AGMS32Type);
    PyModule_AddObject(m, "AGMS64", (PyObject *)&AGMS64Type);
    PyModule_AddObject(m, "AGMS128", (PyObject *)&AGMS128Type);

    Py_INCREF(&FAGMS8Type);
    Py_INCREF(&FAGMS16Type);
    Py_INCREF(&FAGMS32Type);
    Py_INCREF(&FAGMS64Type);
    Py_INCREF(&FAGMS128Type);
    PyModule_AddObject(m, "FAGMS8", (PyObject *)&FAGMS8Type);
    PyModule_AddObject(m, "FAGMS16", (PyObject *)&FAGMS16Type);
    PyModule_AddObject(m, "FAGMS32", (PyObject *)&FAGMS32Type);
    PyModule_AddObject(m, "FAGMS64", (PyObject *)&FAGMS64Type);
    PyModule_AddObject(m, "FAGMS128", (PyObject *)&FAGMS128Type);

}

}
