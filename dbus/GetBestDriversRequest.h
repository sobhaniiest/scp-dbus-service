#ifndef GETBESTDRIVERSREQUEST_H
#define GETBESTDRIVERSREQUEST_H

#include <stdio.h>
#include <stdbool.h>
#include <locale.h>
#include <glib.h> 
#include <gtk/gtk.h>
#include <gio/gio.h>
#include "ppds.h"
#include "nstring.h"
#include "asyncipp.h"
#include "killtimer.h"
#include "scp_interface.h"
#include "GetBestDriversRequest.h"

extern bool g_ppds;

typedef struct _data_ppds_ready
{
    GHashTable *ppdnames;
    const char *device_id;
    const char *device_make_and_model;
    const char *device_uri;
    bool dwn_tried;
}data_ppds_ready;

void GBDRequest(scpinterface *interface,
                const gchar *device_id,
                const gchar *device_make_and_model,
                const gchar *device_uri,
                char *language,
                http_t *status);

#endif