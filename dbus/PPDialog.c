#include "PPDialog.h"

gulong ppd_finished_id = 0;

static int activate_PPDialog(const char *name);
static int activate_error(const char *name);

static int activate_PPDialog(const char *name)
{
    GtkApplication *app;
    int status;
    char buffer[1024];
    snprintf(buffer, 1024, "\tPrinter Properties - '%s' on localhost\t\n\n\tGoto 'system-config-printer' and Click on '%s' and select 'Print Test Page'\t\n",name, name);
    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), (gchar *)buffer);
    status = g_application_run (G_APPLICATION (app), 0, NULL);
    g_object_unref (app);
    return status;
}

static int activate_error(const char *name)
{
    GtkApplication *app;
    int status;
    char buffer[1024];
    snprintf(buffer, 1024, "\tPrinter Properties - '%s' Printer not found on localhost\t\n",name);
    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), (gchar *)buffer);
    status = g_application_run (G_APPLICATION (app), 0, NULL);
    g_object_unref (app);
    return status;
}

void CPPrinterPropertiesDialog(GDBusConnection *connection,
                               gchar *path,
                               guint xid,
                               const gchar *name,
                               GHashTable *uri)
{
    PPDinterface *interface;
    GError *error;

    /********************* main initialization ************************/

    PPDialog_printer *data = (PPDialog_printer *)malloc(sizeof(PPDialog_printer));
    
    if(g_hash_table_contains(uri, name))
    {
        data->status = true;
        data->name = g_locale_to_utf8 (name,-1,0,0,0);
        data->dbus_flag = true;
    }
    else
    {
        data->status = false;
        data->name = g_locale_to_utf8 (name,-1,0,0,0);
        data->dbus_flag = true;
        fprintf(stderr, "Printer not found\n");
        activate_error(name);
    }
    add_hold ();

    /*****************************************************************/

    interface = scp_interface_printer_properties_dialog_skeleton_new();

    /* Method */
    g_signal_connect(interface, 
                     "handle-print-test-page", 
                     G_CALLBACK(PrintTestPage), 
                     data);
    /* Signal */
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
                       PPDialog_printer *data)
{
    
    if(data->dbus_flag == true)
    {
        if(data->status)
        {
            fprintf(stderr, "Printing test page\n");
            activate_PPDialog(data->name);
        }
        else
        {
            fprintf(stderr, "Printer not found\n");
            activate_error(data->name);
        }
        scp_interface_printer_properties_dialog_complete_print_test_page(interface, invocation);
        return true;
    }
    else if (data->dbus_flag == false)
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

        if(g_hash_table_contains(result, data->name))
        {
            fprintf(stderr, "Printing test page\n");
            activate_PPDialog(g_locale_to_utf8 (data->name, -1, 0, 0, 0));
        }
        else
        {
            fprintf(stderr, "Printer not found\n");
            activate_error(g_locale_to_utf8 (data->name, -1, 0, 0, 0));
        }
        httpClose(http);
        fprintf(stderr, "Disconnected ...\n");
        return true;
    }
    return false;
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
    return true;
}