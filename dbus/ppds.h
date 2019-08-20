#ifndef PPDS_H
#define PPDS_H

#include <glib.h> 
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <regex.h>
#include <ctype.h>
#include <math.h>
#include "asyncipp.h"
#include "nstring.h"
#include "xml.h"
#include "ppds.h"

/* Status of match. */
#define STATUS_SUCCESS 0
#define STATUS_MODEL_MISMATCH 1
#define STATUS_GENERIC_DRIVER 2
#define STATUS_NO_DRIVER 3

#define MFR_BY_RANGE_LEN 24

typedef struct _findBestMatchPPDs_data
{
    char *fit;
    GPtrArray *ppdnamelist;
}fBMP_data;

void PPDs(GHashTable *ppds, 
          char *language, 
          const char *xml_dir);

GHashTable *parseDeviceID(char *device_id);
char *ppdMakeModelSplit(const char *ppd_make_and_model);


GHashTable *getPPDNamesFromDeviceID(GHashTable *ppds,
                                    char *mfg,
                                    char *mdl,
                                    char *description,
                                    GPtrArray *commandsets,
                                    const char *uri,
                                    const char *make_and_model);

#endif