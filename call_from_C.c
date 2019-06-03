#include <python3.6m/Python.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* gcc call_from_C.c -o call -lpython3.6m */

int main()
{
    PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;
    int i;

    char *module = "ppdcache"; // module name
    char *method = "demo"; // method or class name

    //Set PYTHONPATH to working directory
    setenv("PYTHONPATH",".",1);

    // Initialize the Python Interpreter
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\".\")");
    PyRun_SimpleString("sys.argv=['']");

    // Build the name object
    pName = PyUnicode_DecodeFSDefault(module);
    /* Error checking of pName left out */

    // Load the module object
    pModule = PyImport_Import(pName);

    Py_DECREF(pName);

    if (pModule == NULL)
        printf("Cannot find module %s\n", module);

    if (pModule != NULL) 
    {
        pFunc = PyObject_GetAttrString(pModule, method);
        /* pFunc is a new reference */

        if (pFunc && PyCallable_Check(pFunc)) 
        {
            pArgs = Py_BuildValue("()");
            PyErr_Print();
            pValue = PyObject_CallObject(pFunc, pArgs);
            PyErr_Print();
        }
        else 
        {
            if (PyErr_Occurred())
                PyErr_Print();
            printf("Cannot find function %s\n", method);
        }

        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else 
    {
        PyErr_Print();
        printf("Failed to load %s\n", module);
        return 1;
    }

    return 0;
}