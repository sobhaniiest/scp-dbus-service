#ifndef GROUPPHYSICALDEVICESREQUEST_H
#define GROUPPHYSICALDEVICESREQUEST_H

#include <stdio.h>


#include "GroupPhysicalDevicesRequest.h"

typedef struct _device_obj
{
    bool is_class;
    char *type;
    char *uri;
    char *id;
    char *info;
}device_obj;



void GPDRequest(GDBusConnection *connection,
                GHashTable *devices,
                void(*reply_handler)(), 
                void(*error_handler)());

#endif