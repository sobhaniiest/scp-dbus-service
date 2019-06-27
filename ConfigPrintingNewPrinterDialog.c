#include "ConfigPrintingNewPrinterDialog.h"

memory reference;

gulong dialog_canceled = 0,
	   printer_added = 0,
	   printer_modified = 0,
	   drv_dwn_checked = 0;

void CPNewPrinterDialog(GDBusConnection *connection, 
	                    const gchar *name, 
	                    gchar *path)
{
	NPDinterface *interface;
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

gboolean NewPrinterFromDevice(NPDinterface *interface,
                              GDBusMethodInvocation *invocation,
                              guint xid, 
                              const gchar *device_uri,
                              const gchar *device_id,
                              gpointer user_data)
{

	add_hold();
	/*
	self.dialog.init ('printer_with_uri', device_uri=device_uri,
                          devid=device_id, xid=xid)
	*/
    
    init("printer_with_uri", 
         device_uri, 
         "\0", 
         NULL, 
         device_id, 
         "\0", 
         0, 
         0);


	newprinterdialog_dbus_new_printer_dialog_complete_new_printer_from_device(interface, invocation);
	return FALSE;
}

gboolean DownloadDriverForDeviceID(NPDinterface *interface,
                                   GDBusMethodInvocation *invocation,
                                   guint xid, 
                                   const gchar *device_id,
                                   gpointer user_data)
{
	add_hold();
	/*
	self.dialog.init ('download_driver', devid=device_id, xid=xid)
	*/

    init("download_driver", 
         "\0", 
         "\0", 
         NULL, 
         device_id, 
         "\0", 
         0, 
         0);

	newprinterdialog_dbus_new_printer_dialog_complete_download_driver_for_device_id(interface, invocation);
	return FALSE;
}

gboolean ChangePPD(NPDinterface *interface,
                   GDBusMethodInvocation *invocation,
                   guint xid, 
                   const gchar *name,
                   const gchar *device_id,
                   gpointer user_data)
{
	add_hold();
	reference.mxid = xid;
    reference.mname = name;
    reference.mdevice_id = device_id;
	fetch_ppd(name, change_ppd_got_ppd, true, "\0", 0, 0);
	newprinterdialog_dbus_new_printer_dialog_complete_change_ppd(interface, invocation);

	return FALSE;
}

/* Internal Functions */

static void change_ppd_got_ppd(const char *name, FILE *ppd)
{
    printer_uri *status = Async_Connection(change_ppd_with_dev,
                                           do_change_ppd,
                                           NULL,
                                           "\0",
                                           0,
                                           0,
                                           true,
                                           true,
                                           "getURI");
    if(!status)
      do_change_ppd("\0", name, ppd);
    else
      change_ppd_with_dev(&status, name, ppd);
}


static void change_ppd_with_dev(printer_uri **head, const char *name, FILE *ppd)
{
    bool found = false;
    printer_uri *c = (*head);
    while(c != NULL)
    {
        if(!strcmp(c->name, name))
        {
            found = true;
            break;
        }
        c = c->next;
    }
    if(found)
        do_change_ppd(c->uri, name, ppd);
    else
        do_change_ppd("\0", name, ppd);
}

static void do_change_ppd(const char *device_uri, const char *name, FILE *ppd)
{  
    //char *device_id = "MFG:Generic;CMD:PJL,PDF;MDL:PDF Printer;CLS:PRINTER;DES:Generic PDF Printer;DRV:DPDF,R1,M0;";
    init("ppd",
         device_uri,
         name,
         ppd,
         reference.mdevice_id,
         "\0",
         0,
         reference.mxid);
}

/* Signals */


gboolean on_dialog_canceled(NPDinterface *interface,
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

gboolean on_printer_added(NPDinterface *interface,
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

gboolean on_printer_modified(NPDinterface *interface,
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

gboolean on_driver_download_checked(NPDinterface *interface,
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

