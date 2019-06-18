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

GDBusConnection *conn;
const gchar *name = "org.fedoraproject.Config.Printing";
char path[1024];
gint pathn = 0;

static void name_acquired_handler(GDBusConnection *connection, 
	                              const gchar *name, 
	                              gpointer user_data);

static gboolean NewPrinterDialog(NewPrinterDialogDBusOrgFedoraprojectConfigPrinting *interface,
								 GDBusMethodInvocation *invocation,
								 gpointer user_data);

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
	NewPrinterDialogDBusOrgFedoraprojectConfigPrinting *interface;
	GError *error;

	/* main initialization */
	KillTimer((void *)gtk_main_quit);
    
	Async_Connection(NULL, 
					 NULL, 
					 NULL, 
					 (char *)"\0", 
					 0, 
					 0, 
					 true, 
					 true);
    
	/**********************/

	interface = newprinterdialog_dbus_org_fedoraproject_config_printing_skeleton_new();

	g_signal_connect(interface, 
					 "handle-new-printer-dialog", 
					 G_CALLBACK(NewPrinterDialog), 
					 NULL);
	
	error = NULL;
	g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(interface), 
									 connection, 
									 "/org/fedoraproject/Config/Printing", 
									 &error);
}

static gboolean NewPrinterDialog(NewPrinterDialogDBusOrgFedoraprojectConfigPrinting *interface,
								 GDBusMethodInvocation *invocation,
								 gpointer user_data)
{
	pathn += 1;
	snprintf(path, 1024, "/org/fedoraproject/Config/Printing/NewPrinterDialog/%d", pathn);
	CPNewPrinterDialog(conn, name, path);
	alive();
	newprinterdialog_dbus_org_fedoraproject_config_printing_complete_new_printer_dialog(interface, invocation, path);
	return FALSE;
}

