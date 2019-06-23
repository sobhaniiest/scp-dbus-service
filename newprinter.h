#ifndef NEWPRINTER_H
#define NEWPRINTER_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <cups/cups.h>
#include <gtk/gtk.h>
#include "newprinter.h"


void NewPrinter();
static void s_c_p(GtkWidget *widget, gpointer data);
bool init(char *dialog_mode,
		  char *device_uri,
		  char *name,
		  FILE *ppd,
		  char *device_id,
		  char *host,
		  http_encryption_t encryption,
		  unsigned int xid);

static void activate(GtkApplication *app, 
                     char *user_data);
static int activate_NewPrinterFromDevice(char *dev_uri, 
                                         char *devid);
static int activate_DownloadDriverForDeviceID(char *devid);
static int activate_ChangePPD(char *dev_uri, 
                              char *devid, 
                              char *name, 
                              FILE *ppd);

#endif