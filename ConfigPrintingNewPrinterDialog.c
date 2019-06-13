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
#include "ppdcache.h"
#include "asyncconn.h"
#include "authinfocache.h"
#include "ConfigPrintingNewPrinterDialog.h"
#include "newprinterdialog_dbus.h"

gulong dialog_canceled = 0,
	   printer_added = 0,
	   printer_modified = 0,
	   drv_dwn_checked = 0;

static void name_acquired_handler(GDBusConnection *connection, 
	                              const gchar *name, 
	                              gpointer user_data);

/* Methods */

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

/* signals */

static gboolean on_dialog_canceled(NewPrinterDialogDBusPrinting *interface,
						  			  GDBusMethodInvocation *invocation,
						              gpointer user_data);

static gboolean on_printer_added(NewPrinterDialogDBusPrinting *interface,
						            GDBusMethodInvocation *invocation,
						            gpointer user_data);

static gboolean on_printer_modified(NewPrinterDialogDBusPrinting *interface,
						               GDBusMethodInvocation *invocation,
						               gpointer user_data);

static gboolean on_driver_download_checked(NewPrinterDialogDBusPrinting *interface,
						                    GDBusMethodInvocation *invocation,
						                    gpointer user_data);

/* Internal Functions */

static void change_ppd_got_ppd();
static void change_ppd_with_dev();
static void do_change_ppd();

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
	
	dialog_canceled = g_signal_connect(interface, 
		             	 			  "handle-on-dialog-canceled", 
		              	 			   G_CALLBACK(on_dialog_canceled), 
		                               NULL);
	printer_added = g_signal_connect(interface, 
		                             "handle-on-printer-added", 
		                              G_CALLBACK(on_printer_added), 
		                              NULL);
	printer_modified = g_signal_connect(interface, 
		                               "handle-on-printer-modified", 
		                                G_CALLBACK(on_printer_modified), 
		                                NULL);
	drv_dwn_checked = g_signal_connect(interface, 
		                			  "handle-on-driver-download-checked", 
		                 			   G_CALLBACK(on_driver_download_checked), 
		                               NULL);
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
	newprinterdialog_dbus_printing_complete_new_printer_from_device(interface, invocation);
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
	newprinterdialog_dbus_printing_complete_download_driver_for_device_id(interface, invocation);
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

/* Internal Functions */

static void change_ppd_got_ppd()
{

}


static void change_ppd_with_dev()
{

}

static void do_change_ppd()
{

}

/* Signals */


static gboolean on_dialog_canceled(NewPrinterDialogDBusPrinting *interface,
						  			  GDBusMethodInvocation *invocation,
						              gpointer user_data)
{	
	remove_hold();
	newprinterdialog_dbus_printing_emit_dialog_canceled((interface));
	newprinterdialog_dbus_printing_complete_on_dialog_canceled(interface, invocation);
	if(dialog_canceled != 0)
		g_signal_handler_disconnect(interface, dialog_canceled);
	//remove_from_connection();
	return TRUE;
}

static gboolean on_printer_added(NewPrinterDialogDBusPrinting *interface,
						            GDBusMethodInvocation *invocation,
						            gpointer user_data)
{
	remove_hold();
	newprinterdialog_dbus_printing_emit_printer_added((interface));
	newprinterdialog_dbus_printing_complete_on_printer_added(interface, invocation);
	if(printer_added != 0)
		g_signal_handler_disconnect(interface, printer_added);
	//remove_from_connection();
	return TRUE;
}

static gboolean on_printer_modified(NewPrinterDialogDBusPrinting *interface,
						               GDBusMethodInvocation *invocation,
						               gpointer user_data)
{
	remove_hold();
	newprinterdialog_dbus_printing_emit_printer_modified((interface));
	newprinterdialog_dbus_printing_complete_on_printer_modified(interface, invocation);
	if(printer_modified != 0)
		g_signal_handler_disconnect(interface, printer_modified);
	//remove_from_connection();
	return TRUE;
}

static gboolean on_driver_download_checked(NewPrinterDialogDBusPrinting *interface,
						                    GDBusMethodInvocation *invocation,
						                    gpointer user_data)
{
	remove_hold();
	newprinterdialog_dbus_printing_emit_driver_download_cheked((interface));
	newprinterdialog_dbus_printing_complete_on_driver_download_checked(interface, invocation);
	if(drv_dwn_checked != 0)
		g_signal_handler_disconnect(interface, drv_dwn_checked);
	//remove_from_connection();
	return TRUE;
}

