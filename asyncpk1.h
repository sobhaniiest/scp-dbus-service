#ifndef ASYNCPK1_H
#define ASYNCPK1_H

#include <cups/cups.h> /*http_t http_encryption_t*/
#include <stdbool.h>
#include "asyncipp.h" /*IPPAuthConnection*/
#include "asyncpk1.h"

http_t *PK1Connection(void(*reply_handler)(), 
                      void(*error_handler)(), 
                      const char *host, 
                      int port, 
                      http_encryption_t encryption);

#endif