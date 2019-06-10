#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "example.h"

static void caught(ExampleExample *proxy, gpointer user_data)
{
    printf("ping !!!\a\n");
    exit(0);
}

static gboolean catch_signal(ExampleExample *interface)
{
    GError *error;
    error = NULL;
    example_example_call_emitter_sync(interface, NULL , &error);
    if(error!=NULL)
    {
        printf("Error !!!\n");
        return 0;
    }
    return FALSE;
}

int main(int argc, char *argv[])
{
	GMainLoop *loop = g_main_loop_new(NULL,FALSE);

	ExampleExample *proxy;
    GError *error;
    error = NULL;
    proxy = NULL;

    proxy = example_example_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION, 0, "com.test", "/", NULL, &error);

    g_signal_connect(proxy, "ping", G_CALLBACK(caught) , NULL);

    g_timeout_add_seconds(3, catch_signal, proxy);

    g_main_loop_run(loop);

    return 0;

}