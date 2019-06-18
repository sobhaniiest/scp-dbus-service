#ifndef ASYNCIPP_H
#define ASYNCIPP_H

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>
#include <cups/cups.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <errno.h>
#include <stdbool.h>
#include "asyncipp.h"
#include "authinfocache.h"

typedef struct _printer_uri
{
	char *name;
	char *uri;
	struct _printer_uri *next;
}printer_uri;

extern printer_uri *puri;

void insert_uri(printer_uri **head, char *n, char *u);
void set_ipp_error (ipp_status_t status, const char *message);
printer_uri *getURI(http_t *new);

void IPPAuthConnection(void(*reply_handler)(), 
					   void(*error_handler)(), 
					   void(*auth_handler)(), 
					   char *host, 
					   int port, 
					   http_encryption_t encryption, 
					   bool try_as_root, 
					   bool prompt_allowed);

void IPPConnection(void(*reply_handler)(), 
				   void(*error_handler)(), 
				   void(*auth_handler)(),
				   char *user, 
				   char *host, 
				   int port, 
				   http_encryption_t encryption);


#endif
