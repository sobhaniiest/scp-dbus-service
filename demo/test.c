#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <SCPService.h>

int main(int argc, char *argv[])
{
    /*
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

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    KillTimer(gtk_main_quit);

    unsigned int xid = (unsigned int) atoi(argv[1]);
    const char *device_uri = argv[2];
    const char *device_id = argv[3];
    const char *name = argv[4];
    const char *file_name = argv[5];    
    
    alive();

    printf("Calling - NewPrinterFromDevice (xid, device_uri, device_id) \n");
    NewPrinterFromDevice(xid, device_uri, device_id);
    remove_hold();

    printf("Calling - DownloadDriverForDeviceID (xid, device_id) \n");
    DownloadDriverForDeviceID(xid, device_id);
    remove_hold();

    /*
    printf("Calling - ChangePPD(xid, printer_name, device_id) \n");
    ChangePPD(xid, name, device_id);
    remove_hold();

    printf("Calling - PrintTestPage (xid, printer_name) \n");
    PrintTestPage((unsigned int)atoi(argv[1]), argv[2]);
    remove_hold();
    */

    printf("Calling - MissingExecutables(ppd_filename) \n");
    char **result = missingexecutables("Generic-PDF_Printer-PDF.ppd");
    printf("###  %s\n", result[0]);

    //g_main_loop_run(loop);
}