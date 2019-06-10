#include <python3.6m/Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>
#include <cups/cups.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include "killtimer.h"
#include "ppdcache.h"
#include "asyncconn.h"
#include "authinfocache.h"
#include "ConfigPrintingNewPrinterDialog.h"
#include "newprinterdialog_dbus.h"

static void name_acquired_handler(GDBusConnection *connection, 
	                              const gchar *name, 
	                              gpointer user_data);

static gboolean NewPrinterFromDevice(NewPrinterDialogDBusPrinting *interface,
									 GDBusMethodInvocation *invocation,
									 const guint xid, 
									 const gchar *device_uri,
									 const gchar *device_id,
									 gpointer user_data);

static gboolean DownloadDriverForDeviceID(NewPrinterDialogDBusPrinting *interface,
									      GDBusMethodInvocation *invocation,
									      const guint xid, 
								          const gchar *device_id,
									      gpointer user_data);

static gboolean ChangePPD(NewPrinterDialogDBusPrinting *interface,
						  GDBusMethodInvocation *invocation,
						  const guint xid, 
						  const gchar *name,
						  const gchar *device_id,
						  gpointer user_data);

static gboolean DialogCanceled_method(NewPrinterDialogDBusPrinting *interface,
						  			  GDBusMethodInvocation *invocation,
						              gpointer user_data);

static gboolean PrinterAdded_method(NewPrinterDialogDBusPrinting *interface,
						            GDBusMethodInvocation *invocation,
						            gpointer user_data);

static gboolean PrinterModified_method(NewPrinterDialogDBusPrinting *interface,
						               GDBusMethodInvocation *invocation,
						               gpointer user_data);

static gboolean DriverDownloadCheked_method(NewPrinterDialogDBusPrinting *interface,
						                    GDBusMethodInvocation *invocation,
						                    gpointer user_data);

static void change_ppd_got_ppd();

int main()
{
	GMainLoop *loop;
	loop = g_main_loop_new(NULL, FALSE);

	g_bus_own_name(G_BUS_TYPE_SESSION, 
		           "com.test", 
		           0, 
		           NULL, 
		           name_acquired_handler, 
		           NULL, 
		           NULL, 
		           NULL);

	g_main_loop_run(loop);
}

static void name_acquired_handler(GDBusConnection *connection, 
	                              const gchar *name, 
	                              gpointer user_data)
{
	NewPrinterDialogDBusPrinting *interface;
	GError *error;

	interface = newprinterdialog_dbus_printing_skeleton_new();

	g_signal_connect(interface, 
					 "handle-new-printer-from-device", 
					 G_CALLBACK(NewPrinterFromDevice), 
					 NULL);
	g_signal_connect(interface, 
					 "handle-download-driver-for-device-id", 
					  G_CALLBACK(DownloadDriverForDeviceID), 
					  NULL);
	g_signal_connect(interface, 
		             "handle-change-ppd", 
		              G_CALLBACK(ChangePPD), 
		              NULL);
	/*
	g_signal_connect(interface, 
		             "handle-dialog-canceled-method", 
		              G_CALLBACK(DialogCanceled_method), 
		              NULL);
	g_signal_connect(interface, 
		             "handle-printer-added-method", 
		              G_CALLBACK(PrinterAdded_method), 
		              NULL);
	g_signal_connect(interface, 
		             "handle-printer-modified-method", 
		              G_CALLBACK(PrinterModified_method), 
		              NULL);
	g_signal_connect(interface, 
		             "handle-driver-download-cheked-method", 
		              G_CALLBACK(DriverDownloadCheked_method), 
		              NULL);
	*/
	error = NULL;
	g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(interface), 
									 connection, 
									 "/", 
									 &error);
}

/* Methods */

static gboolean NewPrinterFromDevice(NewPrinterDialogDBusPrinting *interface,
									 GDBusMethodInvocation *invocation,
									 const guint xid, 
									 const gchar *device_uri,
									 const gchar *device_id,
									 gpointer user_data)
{

	add_hold();
	/*
	self.dialog.init ('printer_with_uri', device_uri=device_uri,
                          devid=device_id, xid=xid)
	*/
	return FALSE;
}

static gboolean DownloadDriverForDeviceID(NewPrinterDialogDBusPrinting *interface,
									      GDBusMethodInvocation *invocation,
									      const guint xid, 
								          const gchar *device_id,
									      gpointer user_data)
{
	add_hold();
	/*
	self.dialog.init ('download_driver', devid=device_id, xid=xid)
	*/
	return FALSE;
}

static gboolean ChangePPD(NewPrinterDialogDBusPrinting *interface,
						  GDBusMethodInvocation *invocation,
						  const guint xid, 
						  const gchar *name,
						  const gchar *device_id,
						  gpointer user_data)
{
	add_hold();
	g_print("Called!!!\n");
	//guint self_xid = xid;
	//gchar *self_name = name; 
	//gchar *self_device_id = device_id;
	fetch_ppd(name, change_ppd_got_ppd);
	newprinterdialog_dbus_printing_complete_change_ppd(interface, invocation);

	return FALSE;
}

/* Signals */

/*
static gboolean DialogCanceled_method(NewPrinterDialogDBusPrinting *interface,
						  			  GDBusMethodInvocation *invocation,
						              gpointer user_data)
{	
	newprinterdialog_dbus_printing_emit_dialog_canceled(NEW_PRINTER_DIALOG_DBUS_PRINTING(interface));
	newprinterdialog_dbus_printing_complete_dialog_canceled_method(interface, invocation);
	return TRUE;
}

static gboolean PrinterAdded_method(NewPrinterDialogDBusPrinting *interface,
						            GDBusMethodInvocation *invocation,
						            gpointer user_data)
{
	newprinterdialog_dbus_printing_emit_printer_added(NEW_PRINTER_DIALOG_DBUS_PRINTING(interface));
	newprinterdialog_dbus_printing_complete_printer_added_method(interface, invocation);
	return TRUE;
}

static gboolean PrinterModified_method(NewPrinterDialogDBusPrinting *interface,
						               GDBusMethodInvocation *invocation,
						               gpointer user_data)
{
	newprinterdialog_dbus_printing_emit_printer_modified(NEW_PRINTER_DIALOG_DBUS_PRINTING(interface));
	newprinterdialog_dbus_printing_complete_printer_modified_method(interface, invocation);
	return TRUE;
}

static gboolean DriverDownloadCheked_method(NewPrinterDialogDBusPrinting *interface,
						                    GDBusMethodInvocation *invocation,
						                    gpointer user_data)
{
	newprinterdialog_dbus_printing_emit_driver_download_cheked(NEW_PRINTER_DIALOG_DBUS_PRINTING(interface));
	newprinterdialog_dbus_printing_complete_driver_download_cheked_method(interface, invocation);
	return TRUE;
}
*/
/* Internal Functions */

static void change_ppd_got_ppd()
{

}

/*
static void change_ppd_with_dev()
{

}

static void do_change_ppd()
{

}
*/