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
#include "killtimer.h" /*KillTimer alive*/
#include "asyncconn.h" /*Async_Connection*/
#include "ConfigPrintingNewPrinterDialog.h" /*CPNewPrinterDialog*/
#include "MissingExecutables.h" /*missingexecutables*/
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

GDBusConnection *conn;
const gchar *name = "org.fedoraproject.Config.Printing";
char path[1024];
gint pathn = 0;
//char *language;
http_t *http;

static void name_acquired_handler(GDBusConnection *connection, 
	                              const gchar *name, 
	                              gpointer user_data);

static gboolean NewPrinterDialog(scpinterface *interface,
								 GDBusMethodInvocation *invocation,
								 gpointer user_data);
/*
static gboolean PrinterPropertiesDialog(scpinterface *interface,
								        GDBusMethodInvocation *invocation,
								        guint xid,
								        const gchar *name,
								        gpointer user_data);

static gboolean JobApplet(scpinterface *interface,
						  GDBusMethodInvocation *invocation,
						  gpointer user_data);

static gboolean GetBestDrivers(scpinterface *interface,
							   GDBusMethodInvocation *invocation,
							   const gchar *device_id,
							   const gchar *device_make_and_model,
							   const gchar *device_uri,
							   gpointer user_data);
*/
static gboolean MissingExecutables(scpinterface *interface,
								   GDBusMethodInvocation *invocation,
								   const gchar *ppd_filename,
								   gpointer user_data);
/*
static gboolean GroupPhysicalDevices(scpinterface *interface,
								     GDBusMethodInvocation *invocation,
								     gpointer user_data);
*/
int main()
{
	GMainLoop *loop;
	loop = g_main_loop_new(NULL, FALSE);

	g_bus_own_name(G_BUS_TYPE_SESSION, 
		           name, 
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
	conn = connection;
	scpinterface *interface;
	GError *error;

	/* main initialization */

	//language = setlocale(LC_ALL, "");
	KillTimer(gtk_main_quit);
	http = Async_Connection(NULL, NULL, NULL, NULL, 0, 0, true, true);

    if(http)
        fprintf(stderr, "Connected to cups server\n");
    else
    	fprintf(stderr, "Connection error\n");
    
	/**********************/

	interface = scp_interface__skeleton_new();

	g_signal_connect(interface, 
					 "handle-new-printer-dialog", 
					 G_CALLBACK(NewPrinterDialog), 
					 NULL);
	/*
	g_signal_connect(interface, 
					 "handle-printer-properties-dialog", 
					 G_CALLBACK(PrinterPropertiesDialog), 
					 NULL);
	g_signal_connect(interface, 
					 "handle-job-applet", 
					 G_CALLBACK(JobApplet), 
					 NULL);
	g_signal_connect(interface, 
					 "handle-get-best-drivers", 
					 G_CALLBACK(GetBestDrivers), 
					 NULL);
	*/
	g_signal_connect(interface, 
					 "handle-missing-executables", 
					 G_CALLBACK(MissingExecutables), 
					 NULL);
	/*
	g_signal_connect(interface, 
					 "handle-group-physical-devices", 
					 G_CALLBACK(GroupPhysicalDevices), 
					 NULL);
	*/
	error = NULL;
	g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(interface), 
									 connection, 
									 "/org/fedoraproject/Config/Printing", 
									 &error);


}

static gboolean NewPrinterDialog(scpinterface *interface,
								 GDBusMethodInvocation *invocation,
								 gpointer user_data)
{
	pathn += 1;
	snprintf(path, 1024, "/org/fedoraproject/Config/Printing/NewPrinterDialog/%d", pathn);
	CPNewPrinterDialog(conn, name, path, http);
	alive();
	scp_interface__complete_new_printer_dialog(interface, invocation, path);
	return TRUE;
}
/*
static gboolean PrinterPropertiesDialog(scpinterface *interface,
								        GDBusMethodInvocation *invocation,
								        guint xid,
								        const gchar *name,
								        gpointer user_data)
{
	pathn += 1;
	snprintf(path, 1024, "/org/fedoraproject/Config/Printing/PrinterPropertiesDialog/%d", pathn);
	CPPrinterPropertiesDialog(conn, path, xid, name);
	alive();
	scp_interface__complete_printer_properties_dialog(interface, invocation, path);
	return TRUE;
}

static gboolean JobApplet(scpinterface *interface,
						  GDBusMethodInvocation *invocation,
						  gpointer user_data)
{
	pathn += 1;
	snprintf(path, 1024, "/org/fedoraproject/Config/Printing/JobApplet/%d", pathn);
	CPJobApplet(conn, path);
	alive();
	scp_interface__complete_job_applet(interface, invocation, path);
	return TRUE;
}

static gboolean GetBestDrivers(scpinterface *interface,
							   GDBusMethodInvocation *invocation,
							   const gchar *device_id,
							   const gchar *device_make_and_model,
							   const gchar *device_uri,
							   gpointer user_data)
{
	GVariant *drivers = GetBestDriversRequest(device_id, device_make_and_model, device_uri, language);
	scp_interface__complete_get_best_drivers(interface, invocation, drivers);
	return TRUE;
}
*/
static gboolean MissingExecutables(scpinterface *interface,
								   GDBusMethodInvocation *invocation,
								   const gchar *ppd_filename,
								   gpointer user_data)
{
	char **missing_executables = missingexecutables(ppd_filename);
	scp_interface__complete_missing_executables(interface, invocation, missing_executables);
	return TRUE;
}
/*
static gboolean GroupPhysicalDevices(scpinterface *interface,
								 	 GDBusMethodInvocation *invocation,
								     gpointer user_data)
{
	GVariant *grouped_devices = GroupPhysicalDevicesRequest();
	scp_interface__complete_group_physical_devices(interface, invocation, grouped_devices);
	return TRUE;
}
*/