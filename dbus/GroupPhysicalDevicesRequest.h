#ifndef GROUPPHYSICALDEVICESREQUEST_H
#define GROUPPHYSICALDEVICESREQUEST_H

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <gio/gio.h>
#include <glib.h> 
#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
#include "nstring.h"
#include "killtimer.h"
#include "scp_interface.h"
#include "asyncipp.h"
#include "GroupPhysicalDevicesRequest.h"

typedef struct _device_obj
{
    bool is_class;
    char *type;
    char *uri;
    char *id;
    char *info;
}device_obj;

typedef struct _uri_by_name
{
    char *name;
    char *stype;
    char *domain;
}uri_by_name;

extern guint unresolved;

GVariant *GPDRequest(scpinterface *interface,
                     GHashTable *devices,
                     void(*reply_handler)(), 
                     void(*error_handler)());

#endif