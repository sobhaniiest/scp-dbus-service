#ifndef PPDCACHE_H
#define PPDCACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <cups/cups.h>
#include <cups/ppd.h>

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

extern void(*cups)();

/* Global variables */
extern dict_cache *cache;
extern dict_modtimes *modtimes;
extern list *queued;

extern const char *host;
extern int port;
extern int encryption;

extern bool connecting;
extern bool check_uptodate;

/* Function declaration */

/* Initializing Function */
void PPDCache(const char *h, int p, int e);

/* Inserting cache data - printer queue name and PPD file descriptor */
void insert_cache(dict_cache **head, char *str, FILE *fpname);

/* Inserting cache data - printer queue name and modification time */
void insert_modtimes(dict_modtimes **head, char *str, time_t value);

/* Inserting cache data - printer queue name and callback functions */
void insert_list(list **head, char *str, void(*func)());

/* Finding the printer queue name is in cache data or not */
bool find_modtimes(dict_modtimes **head, char *str);
bool find_cache(dict_cache **head, char *str);

/* Return the file descriptor of the corresponding printer queue if avaliable else return NULL */
FILE *find_file(dict_cache **head, char *str);

/* Fetch the PPD file and check whether the PPD is up to date */
void fetch_ppd(char *name, void(*callback)());

/* */
void connected();

/* Connecting to asyncconn : Asyn_Connection*/
void self_connect(void(*callback)());

/* If the file is older, Cache the new version */
void got_ppd3(char *name, http_status_t status, time_t time, char *fname, void(*callback)());

/* */
void schedule_callback(void(*callback)(), char *name);

#endif
