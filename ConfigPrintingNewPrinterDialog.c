#include "ConfigPrintingNewPrinterDialog.h"

gulong dialog_canceled = 0,
	   printer_added = 0,
	   printer_modified = 0,
	   drv_dwn_checked = 0;

void CPNewPrinterDialog(GDBusConnection *connection, 
	                    const gchar *name, 
	                    const gchar *path)
{
	NewPrinterDialogDBusNewPrinterDialog *interface;
	GError *error;

	/* main initialization */


    
	/**********************/

	interface = newprinterdialog_dbus_new_printer_dialog_skeleton_new();

	/* Methods */
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
	/* Signals */
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
									 path, 
									 &error);
}

/* Methods */

gboolean NewPrinterFromDevice(NewPrinterDialogDBusNewPrinterDialog *interface,
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
    
	newprinterdialog_dbus_new_printer_dialog_complete_new_printer_from_device(interface, invocation);
	return FALSE;
}

gboolean DownloadDriverForDeviceID(NewPrinterDialogDBusNewPrinterDialog *interface,
                                   GDBusMethodInvocation *invocation,
                                   const guint xid, 
                                   const gchar *device_id,
                                   gpointer user_data)
{
	add_hold();
	/*
	self.dialog.init ('download_driver', devid=device_id, xid=xid)
	*/
	newprinterdialog_dbus_new_printer_dialog_complete_download_driver_for_device_id(interface, invocation);
	return FALSE;
}

gboolean ChangePPD(NewPrinterDialogDBusNewPrinterDialog *interface,
                   GDBusMethodInvocation *invocation,
                   const guint xid, 
                   const gchar *name,
                   const gchar *device_id,
                   gpointer user_data)
{
	add_hold();
	g_print("Called!!!\n");
	fetch_ppd(name, change_ppd_got_ppd, true, "\0", 0, 0);
	newprinterdialog_dbus_new_printer_dialog_complete_change_ppd(interface, invocation);

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


gboolean on_dialog_canceled(NewPrinterDialogDBusNewPrinterDialog *interface,
                            GDBusMethodInvocation *invocation,
                            gpointer user_data)
{	
	remove_hold();
	newprinterdialog_dbus_new_printer_dialog_emit_dialog_canceled((interface));
	newprinterdialog_dbus_new_printer_dialog_complete_on_dialog_canceled(interface, invocation);
	if(dialog_canceled != 0)
		g_signal_handler_disconnect(interface, dialog_canceled);
	//remove_from_connection();
	return TRUE;
}

gboolean on_printer_added(NewPrinterDialogDBusNewPrinterDialog *interface,
                          GDBusMethodInvocation *invocation,
                          gpointer user_data)
{
	remove_hold();
	newprinterdialog_dbus_new_printer_dialog_emit_printer_added((interface));
	newprinterdialog_dbus_new_printer_dialog_complete_on_printer_added(interface, invocation);
	if(printer_added != 0)
		g_signal_handler_disconnect(interface, printer_added);
	//remove_from_connection();
	return TRUE;
}

gboolean on_printer_modified(NewPrinterDialogDBusNewPrinterDialog *interface,
                             GDBusMethodInvocation *invocation,
                             gpointer user_data)
{
	remove_hold();
	newprinterdialog_dbus_new_printer_dialog_emit_printer_modified((interface));
	newprinterdialog_dbus_new_printer_dialog_complete_on_printer_modified(interface, invocation);
	if(printer_modified != 0)
		g_signal_handler_disconnect(interface, printer_modified);
	//remove_from_connection();
	return TRUE;
}

gboolean on_driver_download_checked(NewPrinterDialogDBusNewPrinterDialog *interface,
                                    GDBusMethodInvocation *invocation,
                                    gpointer user_data)
{
	remove_hold();
	newprinterdialog_dbus_new_printer_dialog_emit_driver_download_cheked((interface));
	newprinterdialog_dbus_new_printer_dialog_complete_on_driver_download_checked(interface, invocation);
	if(drv_dwn_checked != 0)
		g_signal_handler_disconnect(interface, drv_dwn_checked);
	//remove_from_connection();
	return TRUE;
}

