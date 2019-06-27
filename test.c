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
#include "newprinter.h"
#include "ConfigPrintingNewPrinterDialog.h"
#include "newprinterdialog_dbus.h"

int main()
{
    
    const char *host = cupsServer();
    int port = ippPort();
    http_encryption_t encryption = cupsEncryption();

    printer_uri *status = Async_Connection(NULL,
                                           NULL,
                                           NULL,
                                           "\0",
                                           0,
                                           0,
                                           true,
                                           true,
                                           "getPPDs");

/*
    while(status != NULL)
    {
        fprintf(stderr, "%s %s\n", status->name, status->uri);
        status = status->next;
    }
*/
    while(status != NULL)
    {
        fprintf(stderr, "%s\n", status->name);
        status = status->next;
    }

}