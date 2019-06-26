#ifndef AUTHINFOCACHE_H
#define AUTHINFOCACHE_H

/* Header Files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "authinfocache.h"

/* Structure contains host, port, user and password */ 
typedef struct _dict
{
    const char *hname;
    int p;
    const char *uname;
    const char *pword;
    struct _dict *next;
}dict;

/* Global variables */
extern dict *creds;

/* Function Declarations */

/* Internal Functions */

void insert(dict **head, const char *host, int port, const char *user, char *password);
dict *find(dict **head, const char *host, int port);
void del(dict **head, const char *host, int port);

/* External Functions */

void cache_auth_info(const char *user, char *password, const char *host, int port);
dict *lookup_auth_info(const char *host, int port);
void remove_auth_info(const char *host, int port);

#endif
