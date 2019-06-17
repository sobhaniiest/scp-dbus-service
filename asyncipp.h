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
