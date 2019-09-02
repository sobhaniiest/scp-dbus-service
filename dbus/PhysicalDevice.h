#ifndef PHYSICALDEVICE_H
#define PHYSICALDEVICE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "nstring.h"
#include "ppds.h"
#include "PhysicalDevice.h"

typedef struct _device_obj
{
    GHashTable *id_dict;
    char *make_and_model;
    bool is_class;
    char *type;
    char *uri;
    char *id;
    char *info;
}device_obj;

typedef struct _PhysicalDevice_data
{
    device_obj *devices;
    char *network_host;
    char *dnssd_hostname;
    bool cupsserver;
    char *firsturi;
    GHashTable *user_data;
}PhysicalDevice_data;

typedef struct _host_data
{
    char *host;
    char *dnssdhost;
}host_data;

PhysicalDevice_data *PhysicalDevice();

PhysicalDevice_data *add_device(device_obj *device, PhysicalDevice_data *pdata);

#endif