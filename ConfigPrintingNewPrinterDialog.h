#ifndef CONFIGPRINTINGNEWPRINTERDIALOG_H
#define CONFIGPRINTINGNEWPRINTERDIALOG_H

#include <stdio.h> /*fprintf*/
#include <glib.h>
/*
	g_signal_connect
	g_dbus_interface_skeleton_export
	g_signal_handler_disconnect
*/
#include <string.h> /*strcmp*/
#include <cups/cups.h> /*http_t*/
#include <stdbool.h> /*bool*/
#include "killtimer.h" /*add_hold remove_hold*/
#include "ppdcache.h" /*fetch_ppd*/
#include "asyncipp.h" /*getURI*/
#include "newprinter.h" /*init*/
#include "scp_interface.h"
/*
	scp_interface_new_printer_dialog_skeleton_new
	scp_interface_new_printer_dialog_complete_new_printer_from_device
	scp_interface_new_printer_dialog_complete_download_driver_for_device_id
	scp_interface_new_printer_dialog_complete_change_ppd
	scp_interface_new_printer_dialog_emit_dialog_canceled
	scp_interface_new_printer_dialog_complete_on_dialog_canceled
	scp_interface_new_printer_dialog_emit_printer_added
	scp_interface_new_printer_dialog_complete_on_printer_added
	scp_interface_new_printer_dialog_emit_printer_modified
	scp_interface_new_printer_dialog_complete_on_printer_modified
	scp_interface_new_printer_dialog_emit_driver_download_cheked
	scp_interface_new_printer_dialog_complete_on_driver_download_checked
*/
#include "ConfigPrintingNewPrinterDialog.h"

#define NPDinterface scpinterfaceNewPrinterDialog

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

extern http_t *http_status;

void CPNewPrinterDialog(GDBusConnection *connection, 
	                    const gchar *name, 
	                    gchar *path,
	                    http_t *status);

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



#endif

