#ifndef PPDCACHE_H
#define PPDCACHE_H

#include <stdio.h> /*fopen fseek remove*/
#include <stdlib.h> /*malloc*/
#include <stdbool.h> /*bool*/
#include <string.h> /*strcmp*/
#include <cups/cups.h>
#include <cups/ppd.h> /*cupsGetPPD3*/
#include <pthread.h>
/*
  pthread_mutex_init
  pthread_mutex_lock
  pthread_mutex_unlock
*/
#include "asyncconn.h" /*Async_Connection*/
#include "ppdcache.h"

typedef struct _list
{
    const char *qname;
    void(*method)();
    struct _list *next;
}list;

typedef struct _dict_modtimes
{
    const char *qname;
    time_t time;
    struct _dict_modtimes *next;
}dict_modtimes;

typedef struct _dict_cache
{
    const char *qname;
    FILE *fp;
    struct _dict_cache *next;
}dict_cache;

extern dict_cache *cache;
extern dict_modtimes *modtimes;
extern list *queued;
extern bool connecting;
extern http_t *cups;

/* Function declaration */

/* Fetch the PPD file and check whether the PPD is up to date */
void fetch_ppd(const char *name, 
               void(*callback)(),
               bool check_uptodate,
               const char *host,
               int port,
               http_encryption_t encryption);

#endif
