#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <glib.h> 
#include <gio/gio.h>
#include <SCPService.h>

int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        printf("Usage : %s \n", argv[0]);
        printf("\t'xid'\n");
        printf("\t'device_uri'\n");
        printf("\t'device_id'\n");
        printf("\t'printer_name'\n"); 
        printf("\t'ppd_file_name'\n");
        exit(EXIT_SUCCESS);
    }

    unsigned int xid = (unsigned int) atoi(argv[1]);
    const char *device_uri = argv[2];
    const char *device_id = argv[3];
    const char *name = argv[4];
    const char *file_name = argv[5];    

    gboolean status;
    int dbus_flag = 0;

    GMainLoop *loop;
    loop = g_main_loop_new(NULL, FALSE);
    KillTimer(gtk_main_quit);

    printf("Calling - NewPrinterFromDevice (xid, device_uri, device_id) \n");
    status = NewPrinterFromDevice(NULL, NULL, xid, device_uri, device_id, &dbus_flag);

    printf("Calling - DownloadDriverForDeviceID (xid, device_id) \n");
    status = DownloadDriverForDeviceID(NULL, NULL, xid, device_id, &dbus_flag);
    
    printf("Calling - ChangePPD(xid, printer_name, device_id) \n");
    status = ChangePPD(NULL, NULL, xid, name, device_id, &dbus_flag);

    PPDialog_printer *data = (PPDialog_printer *)malloc(sizeof(PPDialog_printer));
    data->status = true;
    data->name = name;
    data->dbus_flag = false;
    printf("Calling - PrintTestPage (xid, printer_name) \n");
    status = PrintTestPage(NULL, NULL, data);

    printf("Calling - MissingExecutables(ppd_filename) \n");
    char **result = missingexecutables(file_name);
    int i = 0;
    while(strcmp(result[i], "\0"))
    {
        fprintf(stderr, "%s ",result[i]);
        i++;
    }

    g_main_loop_run(loop);
}

// 0 'file:/tmp/printout' 'MFG:HP;MDL:hp 910;DES:hp 910;' 'test' 'Generic-PDF_Printer-PDF.ppd' 