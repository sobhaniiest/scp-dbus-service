#ifndef CONFIGPRINTINGNEWPRINTERDIALOG_H
#define CONFIGPRINTINGNEWPRINTERDIALOG_H

#include <python3.6m/Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>
#include <cups/cups.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <errno.h>
#include "killtimer.h"
//#include "ppdcache.h"
//#include "asyncconn.h"
//#include "authinfocache.h"
#include "ConfigPrintingNewPrinterDialog.h"
#include "newprinterdialog_dbus.h"

extern gulong dialog_canceled,
	   printer_added,
	   printer_modified,
	   drv_dwn_checked;

void CPNewPrinterDialog(GDBusConnection *connection, 
	                    const gchar *name, 
	                    const gchar *path);

/* Methods */

static gboolean NewPrinterFromDevice(NewPrinterDialogDBusNewPrinterDialog *interface,
									 GDBusMethodInvocation *invocation,
									 const guint xid, 
									 const gchar *device_uri,
									 const gchar *device_id,
									 gpointer user_data);

static gboolean DownloadDriverForDeviceID(NewPrinterDialogDBusNewPrinterDialog *interface,
									      GDBusMethodInvocation *invocation,
									      const guint xid, 
								          const gchar *device_id,
									      gpointer user_data);

static gboolean ChangePPD(NewPrinterDialogDBusNewPrinterDialog *interface,
						  GDBusMethodInvocation *invocation,
						  const guint xid, 
						  const gchar *name,
						  const gchar *device_id,
						  gpointer user_data);

/* signals */

static gboolean on_dialog_canceled(NewPrinterDialogDBusNewPrinterDialog *interface,
						  		   GDBusMethodInvocation *invocation,
						           gpointer user_data);

static gboolean on_printer_added(NewPrinterDialogDBusNewPrinterDialog *interface,
						         GDBusMethodInvocation *invocation,
						         gpointer user_data);

static gboolean on_printer_modified(NewPrinterDialogDBusNewPrinterDialog *interface,
						            GDBusMethodInvocation *invocation,
						            gpointer user_data);

static gboolean on_driver_download_checked(NewPrinterDialogDBusNewPrinterDialog *interface,
						                   GDBusMethodInvocation *invocation,
						                   gpointer user_data);

/* Internal Functions */

static void change_ppd_got_ppd();
static void change_ppd_with_dev();
static void do_change_ppd();


#endif