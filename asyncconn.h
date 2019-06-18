#ifndef ASYNCCONN_H
#define ASYNCCONN_H

#include <stdio.h>
#include <stdbool.h>
#include <cups/cups.h>
#include <unistd.h>
#include <sys/types.h>
#include "asyncipp.h"
#include "asyncpk1.h"
#include "authinfocache.h"
#include "asyncconn.h"


void Async_Connection(void(*reply_handler)(), 
					 void(*error_handler)(), 
					 void(*auth_handler)(), 
					 char *host, 
					 int port, 
					 http_encryption_t encryption, 
					 bool try_as_root, 
					 bool prompt_allowed);


#endif
