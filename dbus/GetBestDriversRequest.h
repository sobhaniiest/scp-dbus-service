#ifndef GETBESTDRIVERSREQUEST_H
#define GETBESTDRIVERSREQUEST_H

#include <stdio.h>
#include <stdbool.h>
#include <locale.h>
#include "nstring.h"
#include "asyncipp.h"
#include "killtimer.h"
#include "scp_interface.h"
#include "GetBestDriversRequest.h"

typedef struct _device_id_dict
{
    char *name;
    char *value;
    struct _device_id_dict *next;
}device_id_dict;

extern bool g_ppds;
extern gulong *id;
extern int id_index;

void GBDRequest(scpinterface *interface,
                const gchar *device_id,
                const gchar *device_make_and_model,
                const gchar *device_uri,
                char *language,
                http_t *status);

#endif