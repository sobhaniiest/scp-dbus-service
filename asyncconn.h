#ifndef ASYNCCONN_H
#define ASYNCCONN_H

#include <python3.6m/Python.h>
#include <stdio.h>
#include <stdbool.h>
#include <cups/cups.h>
#include <unistd.h>
#include <sys/types.h>
#include "asyncconn.h"
#include "asyncipp.h"
#include "asyncpk1.h"


void Async_Connection(void(*reply_handler)(), 
					 void(*error_handler)(), 
					 void(*auth_handler)(), 
					 char *host, 
					 int port, 
					 int encryption, 
					 bool try_as_root, 
					 bool prompt_allowed);


#endif
