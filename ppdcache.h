#ifndef PPDCACHE_H
#define PPDCACHE_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct _list
{
    char *qname;
    struct _list *next;
}list;

typedef struct _dict
{
    char *qname;
    float *time;
    struct _dict *next;
}dict;

extern dict *cache;
extern dict *modtimes;
extern list *queued;
extern const char *host;
extern int port;
extern int encryption;
extern bool connecting;


#endif