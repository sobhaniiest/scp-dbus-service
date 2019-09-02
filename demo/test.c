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

    char *make_and_model = (char *)malloc(sizeof(char) * strlen("Virtual Braille BRF Printer") + 1);
    strcpy(make_and_model, "Virtual Braille BRF Printer");
    char *dev_id = (char *)malloc(sizeof(char) * strlen("MFG:Generic;MDL:CUPS-BRF Printer;DES:Generic CUPS-BRF Printer;CLS:PRINTER;CMD:BRF;") + 1);
    strcpy(dev_id, "MFG:Generic;MDL:CUPS-BRF Printer;DES:Generic CUPS-BRF Printer;CLS:PRINTER;CMD:BRF;");
    char *dev_info = (char *)malloc(sizeof(char) * strlen("CUPS-BRF") + 1);
    strcpy(dev_info, "CUPS-BRF");
    char *dev_location = (char *)malloc(sizeof(char) * strlen("") + 1);
    strcpy(dev_location, "");


    printf("Calling - GroupPhysicalDevicesRequest(devices) \n\n");
    GHashTable *device = g_hash_table_new(g_str_hash, g_str_equal);
    char *uri = (char *)malloc(sizeof(char) * strlen("dnssd://foo._printer._tcp.local/") + 1);
    strcpy(uri, "dnssd://foo._printer._tcp.local/");
    devices_attr *dev_data = (devices_attr *)malloc(sizeof(devices_attr));
    dev_data->device_make_and_model = make_and_model;
    dev_data->device_id = dev_id;
    dev_data->device_info = dev_info;
    dev_data->device_location = dev_location;
    
    g_hash_table_insert(device, uri, dev_data);
    GPDRequest(NULL, device, NULL, NULL);

    //g_main_loop_run(loop);
    fprintf(stderr, "\nDone!!!\n");
    //g_main_loop_quit(loop);
    return 0;
}

// 0 'file:/tmp/printout' 'MFG:HP;MDL:hp 910;DES:hp 910;' 'test' 'Generic-PDF_Printer-PDF.ppd' 