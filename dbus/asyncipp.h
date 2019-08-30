#ifndef ASYNCIPP_H
#define ASYNCIPP_H

#include <stdio.h> /*fprintf*/
#include <stdlib.h> /*malloc free*/
#include <string.h> /*strcmp*/
#include <cups/cups.h>
/*
  ippErrorString
  ippNewRequest
  ippAddStrings
  cupsDoRequest
  ippGetStatusCode
  ippDelete
  cupsLastError
  cupsLastErrorString
  ippFirstAttribute
  ippNextAttribute
  ippGetGroupTag
  ippGetValueTag
  ippGetName
  ippGetString
  ippGetInteger
*/
#include <stdbool.h>
#include <glib.h> 
#include <gio/gio.h>
#include "authinfocache.h" /*lookup_auth_info*/
#include "asyncipp.h"

typedef struct _ppds_attributes
{
    char *ppd_natural_language;
    char *ppd_device_id;
    char *ppd_make;
    char *ppd_make_and_model;
    int ppd_model_number;
    char *ppd_product;
    char *ppd_psversion;
    char *ppd_type;
}ppds_attr;

typedef struct _devices_attributes
{
    char *device_make_and_model;
    char *device_id;
    char *device_info;
    char *device_location;
}devices_attr;

GHashTable *getURI(http_t *new);
GHashTable *getPPDs(http_t *new, int all_lists);
GHashTable *getDevices(http_t *new);

http_t *IPPAuthConnection(void(*reply_handler)(), 
                          void(*error_handler)(), 
                          void(*auth_handler)(), 
                          const char *host, 
                          int port, 
                          http_encryption_t encryption, 
                          bool try_as_root, 
                          bool prompt_allowed);

http_t *IPPConnection(void(*reply_handler)(), 
                      void(*error_handler)(), 
                      void(*auth_handler)(),
                      const char *user, 
                      const char *host, 
                      int port, 
                      http_encryption_t encryption);  	
    
#endif
