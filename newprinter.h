#ifndef NEWPRINTER_H
#define NEWPRINTER_H

#include <stdio.h> /*popen snprintf*/
#include <string.h> /*strcmp*/
#include <glib.h> 
/*
    g_signal_connect
    g_application_run
    g_object_unref
*/
#include <stdbool.h>
#include <cups/cups.h> /*cupsServer cupsEncryption*/
#include <gtk/gtk.h>
/*
    gtk_application_window_new
    gtk_window_set_title
    gtk_container_set_border_width
    gtk_grid_new
    gtk_container_add
    gtk_label_new
    gtk_grid_attach
    gtk_button_new_with_label
    g_signal_connect_swapped
    gtk_widget_show_all
    gtk_application_new
*/
#include "newprinter.h"

bool init(char *dialog_mode,
		  const char *device_uri,
		  const char *name,
		  FILE *ppd,
		  const char *device_id,
		  const char *host,
		  http_encryption_t encryption,
		  unsigned int xid);

#endif