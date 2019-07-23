#ifndef PPDS_H
#define PPDS_H

#include <glib.h> 
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <stdlib.h>
#include <string.h>
#include "asyncipp.h"
#include "nstring.h"
#include "ppds.h"

/*
typedef struct _ids_dict
{
    char *lmfg;
    char *lmdl;
    const char *ppdname;
    struct _ids_dict *next;
}ids_dict;
*/
void PPDs(GHashTable *ppds, char *language, const char *xml_dir);

GHashTable *parseDeviceID(const char *device_id);


GHashTable *getPPDNamesFromDeviceID(GHashTable *ppds,
                                    char *mfg,
                                    char *mdl,
                                    char *description,
                                    char *commandsets,
                                    const char *uri,
                                    const char *make_and_model);

#endif