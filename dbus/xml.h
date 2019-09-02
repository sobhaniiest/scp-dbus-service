#ifndef XML_H
#define XML_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h> 
#include <gio/gio.h>
#include <regex.h>
#include <ctype.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "nstring.h"
#include "asyncipp.h"
#include "xml.h"

#define FIT_EXACT_CMD "exact-cmd"
#define FIT_EXACT "exact"
#define FIT_CLOSE "close"
#define FIT_GENERIC "generic"
#define FIT_NONE "none"

typedef struct _dtype
{
    char *name;
    char *pkghint;
    GHashTable *attributes;
    GPtrArray *deviceid;
    GHashTable *fit;
}dtype;

typedef struct _ptype
{
    GPtrArray *make_and_model;
    GPtrArray *deviceid;
    GPtrArray *drivertype_patterns;
    GPtrArray *avoid;
    GPtrArray *blacklist;
}ptype;

GPtrArray *PreferredDrivers(char *docname);

GPtrArray *DriverTypes_load(xmlNodePtr drivertypes);

GPtrArray *PreferenceOrder_load(xmlNodePtr preferreddrivers);

GPtrArray *get_ordered_ppdnames(GPtrArray *drivertypes,
                                GHashTable *ppds_dict,
                                GHashTable *fit);

GPtrArray *get_ordered_types(GPtrArray *drivertypes,
                             GPtrArray *pordertypes,
                             char *make_and_model,
                             GHashTable *deviceid);

#endif 