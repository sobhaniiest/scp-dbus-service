#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "example.h"

static void name_acquired_handler(GDBusConnection *connection, const gchar *name, gpointer user_data);
static gboolean addition(ExampleExample *interface, GDBusMethodInvocation *invocation, const gint num1, const gint num2, gpointer user_data);
static gboolean emit(ExampleExample *interface, GDBusMethodInvocation *invocation, gpointer user_data);

int main()
{
    GMainLoop *loop;
    loop = g_main_loop_new(NULL, FALSE);

    g_bus_own_name(G_BUS_TYPE_SESSION, "com.test", 0, NULL, name_acquired_handler, NULL, NULL, NULL);

    g_main_loop_run(loop);
}

static void name_acquired_handler(GDBusConnection *connection, const gchar *name, gpointer user_data)
{
    ExampleExample *interface;
    GError *error;

    interface = example_example_skeleton_new();
    g_signal_connect(interface, "handle-add", G_CALLBACK(addition), NULL);
    g_signal_connect(interface, "handle-emitter", G_CALLBACK(emit), NULL);
    error = NULL;
    g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(interface), connection, "/", &error);
}

static gboolean addition(ExampleExample *interface, GDBusMethodInvocation *invocation, const gint num1, const gint num2, gpointer user_data)
{
    gint ans = num1 + num2;
    g_print("Adding %d and %d.\n", num1, num2);
    g_print("Press Enter to pass the result to client\n");
    getchar();
    example_example_complete_add (interface, invocation, ans);
    return FALSE;
}

static gboolean emit(ExampleExample *interface, GDBusMethodInvocation *invocation, gpointer user_data)
{
    example_example_emit_ping(EXAMPLE_EXAMPLE(interface));
    example_example_complete_emitter(interface, invocation);
    return TRUE;
}

