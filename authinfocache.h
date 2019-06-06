#ifndef AUTHINFOCACHE_H
#define AUTHINFOCACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct _dict
{
    char *hname;
    int p;
    char *uname;
    char *pword;
    struct _dict *next;
}dict;

/* Global variables */
extern dict *creds;

/* Function Declarations */

/* Internal Functions */

void insert(dict **head, char *host, int port, char *user, char *password);
dict *find(dict **head, char *host, int port);
void del(dict **head, char *host, int port);

/* External Functions */

void cache_auth_info(char *user, char *password, char *host, int port);
dict *lookup_auth_info(char *host, int port);
void remove_auth_info(char *host, int port);

#endif
