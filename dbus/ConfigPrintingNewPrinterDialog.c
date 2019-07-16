#include "ConfigPrintingNewPrinterDialog.h"

memory reference;

gulong *NPDSignal_id = NULL;
int NPDSignal_index = 0;

http_t *http_status;

static void change_ppd_got_ppd(const char *name, FILE *ppd);
static void change_ppd_with_dev(printer_uri **head, const char *name, FILE *ppd);
static void do_change_ppd(const char *device_uri, const char *name, FILE *ppd);

static void DialogCanceled();
static void PrinterAdded(const gchar *name);
static void PrinterModifed(const gchar *name, gboolean ppd_has_changed);
static void DriverDownloadChecked(const gchar *installed_files);
static void remove_handles(NPDinterface *interface);

void CPNewPrinterDialog(GDBusConnection *connection, 
	                    const gchar *name, 
	                    gchar *path,
                        http_t *status)
{
	NPDinterface *interface;
	GError *error;

	/******************************* main initialization **********************************/

    http_status = status;
    
	/**************************************************************************************/

	interface = scp_interface_new_printer_dialog_skeleton_new();

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
    
	NPDSignal_id[NPDSignal_index++] = g_signal_connect(interface, 
                		             	 			   "DialogCanceled", 
                		              	 			   G_CALLBACK(on_dialog_canceled), 
                		                               NULL);
	NPDSignal_id[NPDSignal_index++] = g_signal_connect(interface, 
                		                               "PrinterAdded", 
                		                                G_CALLBACK(on_printer_added), 
                		                                NULL);
	NPDSignal_id[NPDSignal_index++] = g_signal_connect(interface, 
                		                               "PrinterModified", 
                		                                G_CALLBACK(on_printer_modified), 
                		                                NULL);
	NPDSignal_id[NPDSignal_index++] = g_signal_connect(interface, 
                		                			  "DriverDownloadChecked", 
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
    
    init("printer_with_uri", 
         device_uri, 
         NULL, 
         NULL, 
         device_id, 
         NULL, 
         0, 
         0);

	scp_interface_new_printer_dialog_complete_new_printer_from_device(interface, invocation);
	return TRUE;
}

gboolean DownloadDriverForDeviceID(NPDinterface *interface,
                                   GDBusMethodInvocation *invocation,
                                   guint xid, 
                                   const gchar *device_id,
                                   gpointer user_data)
{
	add_hold();

    init("download_driver", 
         NULL, 
         NULL, 
         NULL, 
         device_id, 
         NULL, 
         0, 
         0);

	scp_interface_new_printer_dialog_complete_download_driver_for_device_id(interface, invocation);
	return TRUE;
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
	fetch_ppd(name, change_ppd_got_ppd, true, NULL, 0, 0);
	scp_interface_new_printer_dialog_complete_change_ppd(interface, invocation);

	return TRUE;
}

/* Internal Functions */

static void change_ppd_got_ppd(const char *name, FILE *ppd)
{
    printer_uri *result = getURI(http_status);

    if(!result || !http_status)
      do_change_ppd(NULL, name, ppd);
    else
      change_ppd_with_dev(&result, name, ppd);
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
        do_change_ppd(NULL, name, ppd);
}

static void do_change_ppd(const char *device_uri, const char *name, FILE *ppd)
{  
    //char *device_id = "MFG:Generic;CMD:PJL,PDF;MDL:PDF Printer;CLS:PRINTER;DES:Generic PDF Printer;DRV:DPDF,R1,M0;";
    init("ppd",
         device_uri,
         name,
         ppd,
         reference.mdevice_id,
         NULL,
         0,
         reference.mxid);
}

/* Signals */

static void DialogCanceled()
{

}

void on_dialog_canceled(NPDinterface *interface)
{	
    fprintf(stderr, "Dialog canceled\n");
	remove_hold();
	DialogCanceled();
    remove_handles(interface);
	//remove_from_connection();
}

static void PrinterAdded(const gchar *name)
{

}

void on_printer_added(NPDinterface *interface,
                      const gchar *name)
{
    fprintf(stderr, "printer added\n");
	remove_hold();
	PrinterAdded(name);
	remove_handles(interface);
	//remove_from_connection();
}

static void PrinterModifed(const gchar *name, gboolean ppd_has_changed)
{

}

void on_printer_modified(NPDinterface *interface,
                         const gchar *name,
                         gboolean ppd_has_changed)
{
    fprintf(stderr, "printer modified\n");
	remove_hold();
	PrinterModifed(name, ppd_has_changed);
	remove_handles(interface);
	//remove_from_connection();
}

static void DriverDownloadChecked(const gchar *installed_files)
{

}

void on_driver_download_checked(NPDinterface *interface,
                                const gchar *installed_files)
{
    fprintf(stderr, "driver download checked\n");
	remove_hold();
	DriverDownloadChecked(installed_files);
	remove_handles(interface);
	//remove_from_connection();
}

static void remove_handles(NPDinterface *interface)
{
    int i;
    for(i=0;i<NPDSignal_index;i++)
        g_signal_handler_disconnect(interface, NPDSignal_id[i]);
}