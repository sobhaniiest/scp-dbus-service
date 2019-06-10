#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "newprinterdialog_dbus.h"

static void
callback_async(GObject *proxy,
                   GAsyncResult *res,
                   gpointer user_data);

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Usage: ./demo xid name device_id\n");
        return 0;
    }

    NewPrinterDialogDBusPrinting *proxy;
    GError *error;
    gint retval;

    error = NULL;
    proxy = newprinterdialog_dbus_printing_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION, 
                                                                  0, 
                                                                  "com.test", 
                                                                  "/", 
                                                                  NULL, 
                                                                  &error);

    gint id;
    id = atoi(argv[1]);

    error = NULL;
    newprinterdialog_dbus_printing_call_change_ppd(proxy, 
                                                   id, 
                                                   argv[2], 
                                                   argv[3], 
                                                   NULL, 
                                                   callback_async, 
                                                   &error);

    GMainLoop *loop;
    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    g_object_unref(proxy);

    return 0;
}

static void
callback_async(GObject *proxy,
                   GAsyncResult *res,
                   gpointer user_data)
{
    g_print("callback_async called!\n");
    gint retval;
    GError *error;
    error = NULL;
    newprinterdialog_dbus_printing_call_change_ppd_finish(proxy,  
                                                          res, 
                                                          &error);

    if (error == NULL)
    {
        g_print("Method is called!!!\n");
        exit(0);
    }
    else
        g_print("ERROR!!\n");
}