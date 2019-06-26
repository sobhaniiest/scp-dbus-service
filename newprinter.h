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
		  const char *device_uri,
		  const char *name,
		  FILE *ppd,
		  const char *device_id,
		  const char *host,
		  http_encryption_t encryption,
		  unsigned int xid);

static void activate(GtkApplication *app, 
                     char *user_data);
static int activate_NewPrinterFromDevice(const char *dev_uri, 
                                         const char *devid);
static int activate_DownloadDriverForDeviceID(const char *devid);
static int activate_ChangePPD(const char *dev_uri, 
                              const char *devid, 
                              const char *name, 
                              FILE *ppd);

#endif