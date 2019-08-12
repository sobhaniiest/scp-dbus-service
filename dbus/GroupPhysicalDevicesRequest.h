#ifndef GROUPPHYSICALDEVICESREQUEST_H
#define GROUPPHYSICALDEVICESREQUEST_H

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>

#include "GroupPhysicalDevicesRequest.h"

typedef struct _device_obj
{
    bool is_class;
    char *type;
    char *uri;
    char *id;
    char *info;
}device_obj;

typedef _uri_by_name
{
    char *name;
    char *stype;
    char *domain;
}uri_by_name;

void GPDRequest(GDBusConnection *connection,
                GHashTable *devices,
                void(*reply_handler)(), 
                void(*error_handler)());

#endif