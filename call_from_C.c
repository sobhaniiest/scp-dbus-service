#include <python3.6m/Python.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	//Set PYTHONPATH to working directory
	setenv("PYTHONPATH",".",1);

	PyObject *Name, *Module, *Dict, *Fun, *Value;

	// Initialize the Python Interpreter
    Py_Initialize();
    PySys_SetArgv(argc, argv);

    // Build the name object
    Name = PyString_FromString((char*)"ppdcache");

    // Load the module object
    Module = PyImport_Import(Name);

    // Dict is a borrowed reference 
    Dict = PyModule_GetDict(Module);

    // Func is also a borrowed reference 
    Fun = PyDict_GetItemString(Dict, (char*)"demo"); 

    if (PyCallable_Check(Fun))
    {
        Value=Py_BuildValue("()");
        PyErr_Print();
        PyObject_CallObject(Fun,Value);
        PyErr_Print();
    }
    else 
        PyErr_Print();
   
    //printf("Result is %ld\n",PyLong_AsLong(presult));
    Py_DECREF(Value);

    // Clean up
    Py_DECREF(Module);
    Py_DECREF(Name);

    // Finish the Python Interpreter
    Py_Finalize();
 
    return 0;
}