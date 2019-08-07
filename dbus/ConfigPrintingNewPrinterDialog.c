#include "ConfigPrintingNewPrinterDialog.h"

memory ref_data;
GPtrArray *NPDSignal_id = NULL;
http_t *http_status;

static void change_ppd_got_ppd(const char *name, 
                               FILE *ppd);

static void change_ppd_with_dev(GHashTable *result, 
                                const char *name, 
                                FILE *ppd);

static void do_change_ppd(const char *device_uri, 
                          const char *name, 
                          FILE *ppd);

static void remove_handles(NPDinterface *interface);
/*
static void DialogCanceled();
static void PrinterAdded(const gchar *name);
static void PrinterModifed(const gchar *name, gboolean ppd_has_changed);
static void DriverDownloadChecked(const gchar *installed_files);
static void remove_handles(NPDinterface *interface);
*/
void CPNewPrinterDialog(GDBusConnection *connection, 
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

    NPDSignal_id = g_ptr_array_new ();
    gulong index;

	index = g_signal_connect(interface, 
                		     "dialog-canceled", 
                		     G_CALLBACK(on_dialog_canceled), 
                		     NULL);
    g_ptr_array_add (NPDSignal_id, (gpointer) index);

	index = g_signal_connect(interface, 
                		     "printer-added", 
                		      G_CALLBACK(on_printer_added), 
                		      NULL);
    g_ptr_array_add (NPDSignal_id, (gpointer) index);

	index = g_signal_connect(interface, 
                		     "printer-modified", 
                		      G_CALLBACK(on_printer_modified), 
                		      NULL);
    g_ptr_array_add (NPDSignal_id, (gpointer) index);

	index = g_signal_connect(interface, 
                		     "driver-download-checked", 
                		     G_CALLBACK(on_driver_download_checked), 
                		     NULL);
    g_ptr_array_add (NPDSignal_id, (gpointer) index);
    
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

  	ref_data.mxid = xid;
    ref_data.mname = name;
    ref_data.mdevice_id = device_id;
  	fetch_ppd(name, change_ppd_got_ppd, true, NULL, 0, 0);
  	scp_interface_new_printer_dialog_complete_change_ppd(interface, invocation);

	return TRUE;
}

/* Internal Functions */

static void change_ppd_got_ppd(const char *name, FILE *ppd)
{
    GHashTable *result = getURI(http_status);

    if(!result || !http_status)
      do_change_ppd(NULL, name, ppd);
    else
      change_ppd_with_dev(result, name, ppd);
}


static void change_ppd_with_dev(GHashTable *result, 
                                const char *name, 
                                FILE *ppd)
{
    if(g_hash_table_contains(result, name))
        do_change_ppd(g_hash_table_lookup(result, name), name, ppd);
    else
        do_change_ppd(NULL, name, ppd);
}

static void do_change_ppd(const char *device_uri, 
                          const char *name, 
                          FILE *ppd)
{  
    init("ppd",
         device_uri,
         name,
         ppd,
         ref_data.mdevice_id,
         NULL,
         0,
         ref_data.mxid);
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
    for(int i = 0; i < NPDSignal_id->len; i++)
        g_signal_handler_disconnect(interface, (gulong)g_ptr_array_index ((GPtrArray *)NPDSignal_id, i));
}
