#include <stdio.h> /*snprintf*/
#include <locale.h> /*setlocale*/
#include <glib.h> 
/*
 g_main_loop_new 
 g_bus_own_name 
 g_main_loop_run 
 g_signal_connect
 g_dbus_interface_skeleton_export
*/
#include <gtk/gtk.h> /*gtk_main_quit*/
#include <stdbool.h>
#include <gio/gio.h>
#include "killtimer.h" /*KillTimer alive*/
#include "asyncconn.h" /*Async_Connection*/
#include "asyncipp.h" /* getURI */
#include "ConfigPrintingNewPrinterDialog.h" /*CPNewPrinterDialog*/
#include "MissingExecutables.h" /*missingexecutables*/
#include "GetBestDriversRequest.h"
#include "JobApplet.h"
#include "PPDialog.h" /* CPPrinterPropertiesDialog */
#include "GroupPhysicalDevicesRequest.h"
#include "scp_interface.h"
/*
 scp_interface__skeleton_new 
 scp_interface__complete_new_printer_dialog
 scp_interface__complete_printer_properties_dialog
 scp_interface__complete_job_applet
 scp_interface__complete_get_best_drivers
 scp_interface__complete_missing_executables
 scp_interface__complete_group_physical_devices
*/

typedef struct _ConfigPrinting_data
{
	GDBusConnection *conn;
	char *language;
	http_t *http;
	gint pathn;
}ConfigPrinting_data;

char path[1024];
const gchar *dbus_name = "org.fedoraproject.Config.Printing";

static void name_acquired_handler(GDBusConnection *connection, 
	                              const gchar *dbus_name, 
	                              gpointer user_data);

static gboolean NewPrinterDialog(scpinterface *interface,
								 GDBusMethodInvocation *invocation,
								 ConfigPrinting_data *user_data);

static gboolean PrinterPropertiesDialog(scpinterface *interface,
								        GDBusMethodInvocation *invocation,
								        guint xid,
								        const gchar *name,
								        ConfigPrinting_data *user_data);

static gboolean JobApplet(scpinterface *interface,
						  GDBusMethodInvocation *invocation,
						  ConfigPrinting_data *user_data);

static gboolean GetBestDrivers(scpinterface *interface,
							   GDBusMethodInvocation *invocation,
							   const gchar *device_id,
							   const gchar *device_make_and_model,
							   const gchar *device_uri,
							   ConfigPrinting_data *user_data);

static gboolean MissingExecutables(scpinterface *interface,
								   GDBusMethodInvocation *invocation,
								   const gchar *ppd_filename,
								   ConfigPrinting_data *user_data);

static gboolean GroupPhysicalDevices(scpinterface *interface,
								     GDBusMethodInvocation *invocation,
								     GVariant *devices,
								     ConfigPrinting_data *user_data);

int main()
{
	GMainLoop *loop;
	loop = g_main_loop_new(NULL, FALSE);

	g_bus_own_name(G_BUS_TYPE_SESSION, 
		           dbus_name, 
		           0, 
		           NULL, 
		           name_acquired_handler, 
		           NULL, 
		           NULL, 
		           NULL);

	g_main_loop_run(loop);
}

static void name_acquired_handler(GDBusConnection *connection, 
	                              const gchar *dbus_name, 
	                              gpointer user_data)
{
	scpinterface *interface;
	GError *error;

	/********************* main initialization ************************/

	char *language = setlocale(LC_ALL, "");
	KillTimer(gtk_main_quit);
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

    ConfigPrinting_data *CP_data = (ConfigPrinting_data *)malloc(sizeof(ConfigPrinting_data));
    CP_data->conn = connection;
    CP_data->language = language;
    CP_data->http = http;
    CP_data->pathn = 0;

	/****************************************************************/

	interface = scp_interface__skeleton_new();

	g_signal_connect(interface, 
					 "handle-new-printer-dialog", 
					 G_CALLBACK(NewPrinterDialog), 
					 CP_data);
	
	g_signal_connect(interface, 
					 "handle-printer-properties-dialog", 
					 G_CALLBACK(PrinterPropertiesDialog), 
					 CP_data);
	
	g_signal_connect(interface, 
					 "handle-job-applet", 
					 G_CALLBACK(JobApplet), 
					 CP_data);
	
	g_signal_connect(interface, 
					 "handle-get-best-drivers", 
					 G_CALLBACK(GetBestDrivers), 
					 CP_data);
	
	g_signal_connect(interface, 
					 "handle-missing-executables", 
					 G_CALLBACK(MissingExecutables), 
					 CP_data);
	
	g_signal_connect(interface, 
					 "handle-group-physical-devices", 
					 G_CALLBACK(GroupPhysicalDevices), 
					 CP_data);
	
	error = NULL;
	g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(interface), 
									 connection, 
									 "/org/fedoraproject/Config/Printing", 
									 &error);


}

static gboolean NewPrinterDialog(scpinterface *interface,
								 GDBusMethodInvocation *invocation,
								 ConfigPrinting_data *user_data)
{
	(user_data->pathn) += 1;
	snprintf(path, 1024, "/org/fedoraproject/Config/Printing/NewPrinterDialog/%d", user_data->pathn);
	CPNewPrinterDialog(user_data->conn, path, user_data->http);
	alive();
	scp_interface__complete_new_printer_dialog(interface, invocation, path);
	return TRUE;
}

static gboolean PrinterPropertiesDialog(scpinterface *interface,
								        GDBusMethodInvocation *invocation,
								        guint xid,
								        const gchar *name,
								        ConfigPrinting_data *user_data)
{
	(user_data->pathn) += 1;
	snprintf(path, 1024, "/org/fedoraproject/Config/Printing/PrinterPropertiesDialog/%d", user_data->pathn);
	fprintf(stderr, "Collecting all printer and corresponding uri...\n");
	GHashTable *result = getURI(user_data->http);
	
	CPPrinterPropertiesDialog(user_data->conn, path, xid, name, result);
	alive();
	scp_interface__complete_printer_properties_dialog(interface, invocation, path);
	return TRUE;
}

static gboolean JobApplet(scpinterface *interface,
						  GDBusMethodInvocation *invocation,
						  ConfigPrinting_data *user_data)
{
	(user_data->pathn) += 1;
	snprintf(path, 1024, "/org/fedoraproject/Config/Printing/JobApplet/%d", user_data->pathn);
	CPJobApplet(user_data->conn, path);
	alive();
	scp_interface__complete_job_applet(interface, invocation, path);
	return TRUE;
}

static gboolean GetBestDrivers(scpinterface *interface,
							   GDBusMethodInvocation *invocation,
							   const gchar *device_id,
							   const gchar *device_make_and_model,
							   const gchar *device_uri,
							   ConfigPrinting_data *user_data)
{
	GHashTable *fit = GBDRequest(interface, device_id, device_make_and_model, device_uri, user_data->language, user_data->http);
	GHashTableIter iter;
    gpointer key, value;
    GVariantBuilder *builder;
    GVariant *drivers;
    builder = g_variant_builder_new(G_VARIANT_TYPE("a(ss)"));
	g_hash_table_iter_init(&iter, fit);
    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        fprintf(stderr, "%s: %s\n", (char *)key, (char *)value);
        g_variant_builder_add(builder, "(ss)", (char *)value, (char *)key);
    }
    exit(0);
    drivers = g_variant_new("a(ss)", builder);
	scp_interface__complete_get_best_drivers(interface, invocation, drivers);

	return TRUE;
}

static gboolean MissingExecutables(scpinterface *interface,
								   GDBusMethodInvocation *invocation,
								   const gchar *ppd_filename,
								   ConfigPrinting_data *user_data)
{
	GPtrArray *missing_executables = missingexecutables(ppd_filename);
	scp_interface__complete_missing_executables(interface, invocation, missing_executables);
	return TRUE;
}

static gboolean GroupPhysicalDevices(scpinterface *interface,
								 	 GDBusMethodInvocation *invocation,
								 	 GVariant *devices,
								     ConfigPrinting_data *user_data)
{
	GVariant *grouped_devices = NULL;
	GPDRequest(interface, devices, NULL, NULL);
	scp_interface__complete_group_physical_devices(interface, invocation, grouped_devices);
	return TRUE;
}
