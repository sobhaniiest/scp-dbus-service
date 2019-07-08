#include "GetBestDriversRequest.h"

g_ppds



void GBDRequest(const gchar *device_id,
                const gchar *device_make_and_model,
                const gchar *device_uri,
                const gchar *language)
{
    bool download_tried = false;

    add_hold();

    if(!g_ppds)
    {
        ppds = NULL;
        printer_uri *ppdnames = Async_Connection(NULL, 
                                                 NULL, 
                                                 NULL, 
                                                 "\0", 
                                                 0, 
                                                 0,  
                                                 true, 
                                                 true,
                                                 "getPPDs");
        if(!ppdnames)
            cups_error();
        else
            cups_getppds_reply();
    }
    else
    {

    }
}