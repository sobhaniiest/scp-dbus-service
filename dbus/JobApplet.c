#include "JobApplet.h"

gulong finished_id = 0;
bool has_finished = false;

void CPJobApplet(GDBusConnection *connection, gchar *path)
{
    JAinterface *interface;
    GError *error;

    /********************* main initialization ************************/
       
    add_hold ();



    /****************************************************************/
    interface = scp_interface_job_applet_skeleton_new();

    /* Methods */
    g_signal_connect(interface, 
                     "handle-quit", 
                     G_CALLBACK(Quit), 
                     NULL);
    /* Signals */
    finished_id = g_signal_connect(interface, 
                                   "finished", 
                                   G_CALLBACK(on_dialog_closed), 
                                   NULL);

    error = NULL;
    g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(interface), 
                                     connection, 
                                     path, 
                                     &error);
}

gboolean Quit(JAinterface *interface,
              GDBusMethodInvocation *invocation,
              gpointer user_data)
{
    
    scp_interface_job_applet_complete_quit(interface, invocation);
}

void Finished()
{
    
}

void on_dialog_closed(JAinterface *interface)
{
    Finished();
    remove_hold();
    if(finished_id != 0)
        g_signal_handler_disconnect(interface, finished_id);
    has_finished = true;
    //remove_from_connection();
}