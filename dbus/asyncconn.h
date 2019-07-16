#ifndef ASYNCCONN_H
#define ASYNCCONN_H

#include <stdio.h>
#include <stdbool.h> /*bool*/
#include <cups/cups.h> 
/*
    cupsServer 
    ippPort 
    cupsEncryption 
    http_t
*/
#include <unistd.h> /*getuid*/
#include <sys/types.h> /*getuid*/
#include <string.h> /*strcmp*/
#include "asyncipp.h" /*IPPAuthConnection*/
#include "asyncpk1.h" /*PK1Connection*/
#include "asyncconn.h"

http_t *Async_Connection(void(*reply_handler)(), 
                         void(*error_handler)(), 
                         void(*auth_handler)(), 
                         const char *host, 
                         int port, 
                         http_encryption_t encryption, 
                         bool try_as_root, 
                         bool prompt_allowed);


#endif
