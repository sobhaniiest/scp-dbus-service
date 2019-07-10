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
#include "authinfocache.h" /*lookup_auth_info*/
#include "asyncipp.h"

typedef struct _ppds_attributes
{
    const char *ppdname;
    char *ppd_natural_language;
    char *ppd_device_id;
    char *ppd_make;
    char *ppd_make_and_model;
    int ppd_model_number;
    char *ppd_product;
    char *ppd_psversion;
    char *ppd_type;
    struct _ppds_attributes *next;
}ppds_attr;

typedef struct _printer_uri
{
  	const char *name;
  	const char *uri;
  	struct _printer_uri *next;
}printer_uri;

printer_uri *getURI(http_t *new);
ppds_attr *getPPDs(http_t *new, int all_lists);

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
