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
    char *qname;
    void(*method)();
    struct _list *next;
}list;

typedef struct _dict_modtimes
{
    char *qname;
    time_t time;
    struct _dict_modtimes *next;
}dict_modtimes;

typedef struct _dict_cache
{
    char *qname;
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
static void insert_cache(dict_cache **head, char *str, FILE *fpname);

/* Inserting cache data - printer queue name and modification time */
static void insert_modtimes(dict_modtimes **head, char *str, time_t value);

/* Inserting cache data - printer queue name and callback functions */
static void insert_list(list **head, char *str, void(*func)());

/* Finding the printer queue name is in cache data or not */
static dict_modtimes *find_modtimes(dict_modtimes **head, char *str);
static bool find_cache(dict_cache **head, char *str);

/* Return the file descriptor of the corresponding printer queue if avaliable else return NULL */
static FILE *find_file(dict_cache **head, char *str);

/* Fetch the PPD file and check whether the PPD is up to date */
void fetch_ppd(char *name, 
               void(*callback)(),
               bool check_uptodate,
               char *host,
               int port,
               http_encryption_t encryption);
/* */
void connected();

/* Connecting to asyncconn : Asyn_Connection*/
static void self_connect(void(*callback)(),
                         char *host,
                         int port,
                         http_encryption_t encryption);

/* If the file is older, Cache the new version */
static void got_ppd3(char *name, http_status_t status, time_t time, char *fname, void(*callback)(), bool check_uptodate);

/* */
static void schedule_callback(void(*callback)(), char *name, FILE *ppd);

#endif
