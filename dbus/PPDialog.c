#include "PPDialog.h"

gulong ppd_finished_id = 0;

void CPPrinterPropertiesDialog(GDBusConnection *connection,
                               gchar *path,
                               guint xid,
                               const gchar *name)
{
    PPDinterface *interface;
    GError *error;

    add_hold ();

    interface = scp_interface_printer_properties_dialog_skeleton_new();

    /* Methods */
    g_signal_connect(interface, 
                     "handle-print-test-page", 
                     G_CALLBACK(PrintTestPage), 
                     NULL);
    /* Signals */
    ppd_finished_id = g_signal_connect(interface, 
                                       "finished", 
                                       G_CALLBACK(on_dialog_closed), 
                                       NULL);
    error = NULL;
    g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(interface), 
                                     connection, 
                                     path, 
                                     &error);
}

gboolean PrintTestPage(PPDinterface *interface,
                       GDBusMethodInvocation *invocation,
                       gpointer user_data)
{
    
    scp_interface_printer_properties_dialog_complete_print_test_page(interface, invocation);
}

void Finished()
{
    
}

gboolean on_dialog_closed(PPDinterface *interface,
                          GDBusMethodInvocation *invocation,
                          gpointer user_data)
{
    remove_hold();
    Finished();
    if(ppd_finished_id != 0)
        g_signal_handler_disconnect(interface, ppd_finished_id);
    //remove_from_connection();
}