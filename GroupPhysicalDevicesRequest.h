#ifndef GROUPPHYSICALDEVICESREQUEST_H
#define GROUPPHYSICALDEVICESREQUEST_H

#include <stdio.h>

#include "newprinterdialog_dbus.h"
#include "GroupPhysicalDevicesRequest.h"

typedef struct _attributes
{
    char *device_id;
    char *device_make_and_model;
    char *device_class;
}atr;

typedef struct _devices
{
    char *device_uri;
    atr *ippatrs;
    struct _devices *next;
}dict_devices;

#endif