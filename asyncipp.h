#ifndef ASYNCIPP_H
#define ASYNCIPP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "asyncipp.h"
#include "authinfocache.h"

/* Structure contains host, port, user and password */ 
typedef struct _dict
{
    char *hname;
    int p;
    char *uname;
    char *pword;
    struct _dict *next;
}dict;


static void IPPAuthConnection(void(*reply_handler)(), 
							  void(*error_handler)(), 
							  void(*auth_handler)(), 
							  char *host, 
							  int port, 
							  http_encryption_t encryption, 
							  bool try_as_root, 
							  bool prompt_allowed);

static void IPPConnection(void(*reply_handler)(), 
						  void(*error_handler)(), 
						  void(*auth_handler)(),
						  char *user, 
						  char *host, 
						  int port, 
						  http_encryption_t encryption);


#endif
