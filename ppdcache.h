#ifndef PPDCACHE_H
#define PPDCACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <cups/cups.h>
#include <cups/ppd.h>
#include <pthread.h>
#include "asyncconn.h"

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
extern printer_uri *cups;

/* Function declaration */

/* Inserting cache data - printer queue name and PPD file descriptor */
void insert_cache(dict_cache **head, const char *str, FILE *fpname);

/* Inserting cache data - printer queue name and modification time */
void insert_modtimes(dict_modtimes **head, const char *str, time_t value);

/* Inserting cache data - printer queue name and callback functions */
void insert_list(list **head, const char *str, void(*func)());

/* Finding the printer queue name is in cache data or not */
dict_modtimes *find_modtimes(dict_modtimes **head, const char *str);
bool find_cache(dict_cache **head, const char *str);

/* Return the file descriptor of the corresponding printer queue if avaliable else return NULL */
FILE *find_file(dict_cache **head, const char *str);

/* Fetch the PPD file and check whether the PPD is up to date */
void fetch_ppd(const char *name, 
               void(*callback)(),
               bool check_uptodate,
               const char *host,
               int port,
               http_encryption_t encryption);
/* */
void connected();

/* Connecting to asyncconn : Asyn_Connection*/
void self_connect(void(*callback)(),
                         const char *host,
                         int port,
                         http_encryption_t encryption);

/* If the file is older, Cache the new version */
void got_ppd3(const char *name, http_status_t status, time_t time, char *fname, void(*callback)(), bool check_uptodate);

/* */
void schedule_callback(void(*callback)(), const char *name, FILE *ppd);

#endif
