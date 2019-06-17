#ifndef ASYNCPK1_H
#define ASYNCPK1_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "asyncpk1.h"
#include "asyncipp.h"

void PK1Connection(void(*reply_handler)(), 
				   void(*error_handler)(), 
				   char *host, 
				   int port, 
				   http_encryption_t encryption);

#endif