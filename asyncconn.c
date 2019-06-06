#include "asyncconn.h"

void(*fn_asyn)();
void(*reply_handler_asyn)();
void(*error_handler_asyn)();
void(*auth_handler_asyn)();
void(*reply_data_asyn)();
void(*error_data_asyn)();
void(*auth_data_asyn)();
bool destroyed_asyn, destroyed_cnn;
const char *host_asyn = "\0";
int port_asyn = 0;
int encryption_asyn = 0;
//??? parent
bool try_as_root = true;
bool prompt_allowed = true;

/*
void AsyncMethodCall(void(*fn)(), void(*reply_handler)(), void(*error_handler)(), void(*auth_handler)())
{
	fn_asyn = fn;
	reply_handler_asyn = reply_handler;
	error_handler_asyn = error_handler;
	auth_handler_asyn = auth_handler;
	destroyed_asyn = false;
}

void destroy_asyn()
{
	if(destroyed_asyn)
		return;

	destroyed_asyn = true;
	reply_handler_asyn = NULL;
	error_handler_asyn = NULL;
	auth_handler_asyn = NULL;
	reply_data_asyn = NULL;
	error_data_asyn = NULL;
	auth_data_asyn = NULL;
}

void run()
{

}

void reply_handler()
{
	
}

void error_handler()
{

}

void auth_handler()
{

}

*/

void Asyn_Connection()
{
	bool use_pk;

	destroyed_cnn = false;
	if(!(strcmp(host_asyn,"\0")))
	{
		host_asyn = cupsServer();
		port_asyn = ippPort();
		encryption_asyn = (http_encryption_t) cupsEncryption ();
		use_pk = (((host_asyn[0] == '/') | !(strcmp(host_asyn,"localhost"))) & getuid() != 0);
	}

	PyObject *pName, *pModule, *pFunc;
	PyObject *pArgs, *pValue;
	//Set PYTHONPATH to working directory
    setenv("PYTHONPATH",".",1);
	// Initialize the Python Interpreter
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\".\")");
    PyRun_SimpleString("sys.argv=['']");

	if(use_pk & try_as_root)
	{
		char *module = "asyncpk1"; // module name
    	char *method = "PK1Connection"; // method or class name

    	// Build the name object
    	pName = PyUnicode_DecodeFSDefault(module);

    	// Load the module object
    	pModule = PyImport_Import(pName);

   		Py_DECREF(pName);

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
	        return;
	    }
	}
	else
	{
		char *module = "asyncipp"; // module name
  		char *method = "IPPAuthConnection"; // method or class name
  		// Build the name object
    	pName = PyUnicode_DecodeFSDefault(module);

    	// Load the module object
    	pModule = PyImport_Import(pName);

   		Py_DECREF(pName);

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
	        return;
	    }

	}

}

/*
void subst_reply_handler()
{

}

void subst_error_handler()
{

}

void subst_auth_handler()
{

}

void destroy_cnn()
{

}

void make_binding()
{

}
void call_function()
{

}

void subst_reply_handler_inner()
{

}

void subst_error_handler_inner()
{

}

void subst_auth_handler_inner()
{

}

void set_auth_info()
{
	
}

*/