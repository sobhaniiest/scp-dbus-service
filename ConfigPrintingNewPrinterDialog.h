#ifndef CONFIGPRINTINGNEWPRINTERDIALOG_H
#define CONFIGPRINTINGNEWPRINTERDIALOG_H

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>
#include <cups/cups.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <errno.h>
#include "killtimer.h"
#include "ppdcache.h"
#include "asyncconn.h"
#include "authinfocache.h"
#include "newprinter.h"
#include "ConfigPrintingNewPrinterDialog.h"
#include "newprinterdialog_dbus.h"

#define NPDinterface NewPrinterDialogDBusNewPrinterDialog

typedef struct _memory
{
	guint mxid;
    const gchar *mname;
    const gchar *mdevice_id;
}memory;

extern memory reference;

extern gulong dialog_canceled,
	   printer_added,
	   printer_modified,
	   drv_dwn_checked;

void CPNewPrinterDialog(GDBusConnection *connection, 
	                    const gchar *name, 
	                    gchar *path);

/* Methods */

gboolean NewPrinterFromDevice(NPDinterface *interface,
							  GDBusMethodInvocation *invocation,
							  guint xid, 
							  const gchar *device_uri,
							  const gchar *device_id,
							  gpointer user_data);

gboolean DownloadDriverForDeviceID(NPDinterface *interface,
								   GDBusMethodInvocation *invocation,
								   guint xid, 
								   const gchar *device_id,
								   gpointer user_data);

gboolean ChangePPD(NPDinterface *interface,
				   GDBusMethodInvocation *invocation,
				   guint xid, 
				   const gchar *name,
				   const gchar *device_id,
				   gpointer user_data);

/* signals */

gboolean on_dialog_canceled(NPDinterface *interface,
						  	GDBusMethodInvocation *invocation,
						    gpointer user_data);

gboolean on_printer_added(NPDinterface *interface,
				          GDBusMethodInvocation *invocation,
			   	          gpointer user_data);

gboolean on_printer_modified(NPDinterface *interface,
						     GDBusMethodInvocation *invocation,
						     gpointer user_data);

gboolean on_driver_download_checked(NPDinterface *interface,
						            GDBusMethodInvocation *invocation,
						            gpointer user_data);

/* Internal Functions */

static void change_ppd_got_ppd(const char *name, FILE *ppd);
static void change_ppd_with_dev(printer_uri **head, const char *name, FILE *ppd);
static void do_change_ppd(const char *device_uri, const char *name, FILE *ppd);

#endif