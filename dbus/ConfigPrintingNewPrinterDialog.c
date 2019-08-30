#include "ConfigPrintingNewPrinterDialog.h"

memory ref_data;
GPtrArray *NPDSignal_id = NULL;
http_t *http_status;
bool changeppd_flag = false;

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
    int dbus_flag = 1;
	/**************************************************************************************/

	interface = scp_interface_new_printer_dialog_skeleton_new();

	/* Methods */
	g_signal_connect(interface, 
					 "handle-new-printer-from-device", 
					 G_CALLBACK(NewPrinterFromDevice), 
					 &dbus_flag);
	g_signal_connect(interface, 
					 "handle-download-driver-for-device-id", 
					  G_CALLBACK(DownloadDriverForDeviceID), 
					  &dbus_flag);
	g_signal_connect(interface, 
		             "handle-change-ppd", 
		              G_CALLBACK(ChangePPD), 
		              &dbus_flag);

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
                              int *dbus_flag)
{
    if(*dbus_flag)
    {
        add_hold();
        //fprintf(stderr, "I am here\n");
    
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
    else
    {
        //fprintf(stderr, "I am here\n");
        add_hold();
    
        init("printer_with_uri", 
             device_uri, 
             NULL, 
             NULL, 
             device_id, 
             NULL, 
             0, 
             0);
        return true;
    }
}

gboolean DownloadDriverForDeviceID(NPDinterface *interface,
                                   GDBusMethodInvocation *invocation,
                                   guint xid, 
                                   const gchar *device_id,
                                   int *dbus_flag)
{
	if(*dbus_flag)
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
    else
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

        return TRUE;
    }
}

gboolean ChangePPD(NPDinterface *interface,
                   GDBusMethodInvocation *invocation,
                   guint xid, 
                   const gchar *name,
                   const gchar *device_id,
                   int *dbus_flag)
{
    if(*dbus_flag)
    {
        add_hold();

        ref_data.mxid = xid;
        ref_data.mname = name;
        ref_data.mdevice_id = device_id;
        fetch_ppd(name, change_ppd_got_ppd, true, NULL, 0, 0);
        scp_interface_new_printer_dialog_complete_change_ppd(interface, invocation);

        return TRUE;
    }
    else
    {
        changeppd_flag = true;
        add_hold();

        ref_data.mxid = xid;
        ref_data.mname = name;
        ref_data.mdevice_id = device_id;
        fetch_ppd(name, change_ppd_got_ppd, true, NULL, 0, 0);

        return TRUE;
    }
}

/* Internal Functions */

static void change_ppd_got_ppd(const char *name, FILE *ppd)
{
    if(changeppd_flag)
    {
        http_t *http = Async_Connection(NULL, 
                                    NULL, 
                                    NULL, 
                                    NULL, 
                                    0, 
                                    0, 
                                    true, 
                                    true);

        if(http)
            fprintf(stderr, "Connected to cups server\n");
        else
            fprintf(stderr, "Connection error\n");

        GHashTable *result = getURI(http);

        ////////////////////////////////////////
        /*
        fprintf(stderr, "get_Devices\n");
        GHashTable *res = getDevices(http);

        FILE *fp = fopen("device.txt", "w");

        GHashTableIter iter;
        gpointer key, value;
        g_hash_table_iter_init(&iter, res);
        while (g_hash_table_iter_next(&iter, &key, &value))
        {
            fprintf(fp, "device_uri : %s\n", (char *)key);
            fprintf(fp, "device_make_and_model : %s\n", ((devices_attr *)value)->device_make_and_model);
            fprintf(fp, "device_id : %s\n", ((devices_attr *)value)->device_id);
            fprintf(fp, "device_info : %s\n", ((devices_attr *)value)->device_info);
            fprintf(fp, "device_location : %s\n", ((devices_attr *)value)->device_location);
        }
    
        fclose(fp);
        */
        ///////////////////////////////////////

        if(!result || !http)
          do_change_ppd(NULL, name, ppd);
        else
          change_ppd_with_dev(result, name, ppd);

        httpClose(http);
        fprintf(stderr, "Disconnected ...\n");
    }
    else
    {
        GHashTable *result = getURI(http_status);

        if(!result || !http_status)
          do_change_ppd(NULL, name, ppd);
        else
          change_ppd_with_dev(result, name, ppd);
    }
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
