#include "GetBestDriversRequest.h"

bool g_ppds = false;
GPtrArray *GBDRSignal_id = NULL;

static gboolean ppds_error(scpinterface *interface);
static gboolean ppds_ready(scpinterface *interface, data_ppds_ready *data);

void GBDRequest(scpinterface *interface,
                const gchar *device_id,
                const gchar *device_make_and_model,
                const gchar *device_uri,
                char *language,
                http_t *status)
{
    bool download_tried = false;
    GHashTable *ppdnames;
    GBDRSignal_id = g_ptr_array_new ();
    gulong index;

    add_hold();

    data_ppds_ready *data = (data_ppds_ready *)malloc(sizeof(data_ppds_ready));

    if(!g_ppds)
    {
        fprintf(stderr, "GetBestDrivers request: need to fetch PPDs\n");
        g_ppds = true;
        ppdnames = NULL;

        fprintf(stderr, "FetchPPDs: running\n");
        ppdnames = getPPDs(status, 1);
        fprintf(stderr, "size : %d\n", g_hash_table_size(ppdnames));
        data->ppdnames = ppdnames;
        data->device_id = device_id;
        data->device_make_and_model = device_make_and_model;
        data->device_uri = device_uri;
        data->dwn_tried = download_tried;

        index = g_signal_connect(interface, 
                                 "ready", 
                                 G_CALLBACK(ppds_ready), 
                                 data);
        g_ptr_array_add (GBDRSignal_id, (gpointer) index);

        index = g_signal_connect(interface, 
                                 "error", 
                                 G_CALLBACK(ppds_error), 
                                 NULL);
        g_ptr_array_add (GBDRSignal_id, (gpointer) index);

        if(!ppdnames || !status)
        {
            fprintf(stderr, "FetchPPDs: error\n");
            scp_interface__emit_error(interface);
        }
        else
        {
            fprintf(stderr, "FetchPPDs: success\n");
            PPDs(ppdnames, language, NULL);
            scp_interface__emit_ready(interface);
        }  
    }
    else
    {
        if(!ppdnames)
        {
            fprintf(stderr, "GetBestDrivers request: PPDs already fetched\n");
            ppds_ready(interface, data);
        }
        else
        {
            fprintf(stderr, "GetBestDrivers request: waiting for PPDs\n");
            index = g_signal_connect(interface, 
                                     "ready", 
                                     G_CALLBACK(ppds_ready), 
                                     data);
            g_ptr_array_add (GBDRSignal_id, (gpointer) index);

            index = g_signal_connect(interface, 
                                     "error", 
                                     G_CALLBACK(ppds_error), 
                                     NULL);
            g_ptr_array_add (GBDRSignal_id, (gpointer) index);
        }
    }
}


static gboolean ppds_error(scpinterface *interface)
{

    for(int i = 0; i < GBDRSignal_id->len; i++)
        g_signal_handler_disconnect(interface, (gulong)g_ptr_array_index ((GPtrArray *)GBDRSignal_id, i));
    return true;
}

static gboolean ppds_ready(scpinterface *interface, data_ppds_ready *data)
{
    GPtrArray *installed_files = g_ptr_array_new ();
    if((data->ppdnames) == NULL)
    {
        fprintf(stderr, "PPDs being reloaded. Wait for next 'ready' signal\n");
        return true;
    }

    ppds_error(interface);
   
    char *mfg, *mdl, *des, *cmd;
    
    GHashTable *id_dict;
    char *arg;
    GHashTableIter iter;
    gpointer key, value;

    if(data->device_id)
    {
        fprintf(stderr, "hello hello hello!!!!\n" );
        arg = (char *)malloc(sizeof(char) * strlen(data->device_id) + 1);
        strcpy(arg, data->device_id);
        id_dict = parseDeviceID(arg);
        free(arg);  
    }
    else
    {
        GPtrArray *array = g_ptr_array_new ();
        make_model_data *mfg_mdl = ppdMakeModelSplit(data->device_make_and_model);
        mfg = mfg_mdl->make;
        mdl = mfg_mdl->model;
        g_hash_table_insert(id_dict, "MFG", mfg);
        g_hash_table_insert(id_dict, "MDL", mdl);
        g_hash_table_insert(id_dict, "DES", "");
        g_hash_table_insert(id_dict, "CMD", array);
        size_t len = strlen(mfg)+strlen(mdl)+11;
        char *device_id = (char *)malloc(len);
        snprintf(device_id, len, "MFG:%s;MDL:%s;", mfg, mdl); 
    }
    
    ///////////////////////////////////////////////////////////////////
    GPtrArray *arr = (GPtrArray *)(g_hash_table_lookup(id_dict, "CMD"));
    fprintf(stderr, "1 %s\n",(char *)g_hash_table_lookup(id_dict, "MFG"));
    fprintf(stderr, "2 %s\n",(char *)g_hash_table_lookup(id_dict, "MDL"));
    fprintf(stderr, "3 %s\n",(char *)g_hash_table_lookup(id_dict, "DES"));

    fprintf(stderr, "4 %s %s\n",(char *)g_ptr_array_index ((GPtrArray*)arr, 0), (char *)g_ptr_array_index ((GPtrArray*)arr, 1));
    fprintf(stderr, "5 %s\n",data->device_uri);
    fprintf(stderr, "6 %s\n",data->device_make_and_model);
    ///////////////////////////////////////////////////////////////////

    GHashTable *fit = getPPDNamesFromDeviceID(data->ppdnames,
                                              (char *)g_hash_table_lookup(id_dict, "MFG"),
                                              (char *)g_hash_table_lookup(id_dict, "MDL"),
                                              (char *)g_hash_table_lookup(id_dict, "DES"),
                                              (GPtrArray*)g_hash_table_lookup(id_dict, "CMD"),
                                              data->device_uri, 
                                              data->device_make_and_model);


    g_hash_table_iter_init(&iter, id_dict);
    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        if(!(strcmp((char *)key, "CMD")))
        {
            g_free(key);
            g_ptr_array_free((GPtrArray *)value, true);
        }
        else
        {
            g_free(key);
            g_free(value);
        }
    }


    /*
    GPtrArray *ppdnamelist = orderPPDNamesByPreference(fit, installed_files, id_dict);

    char *ppdname = (char *)g_ptr_array_index ((GPtrArray *)ppdnamelist, 0);
    char *status = g_hash_table_lookup(fit, ppdname);

    if((!(strcmp(status, "exact"))) && (!(data->download_tried)))
    {
        data->download_tried = true;
        g_signal_connect(interface, 
                        "dialog-canceled", 
                         G_CALLBACK(on_dialog_canceled), 
                         user_data);
        g_signal_connect(interface, 
                        "driver-download-checked", 
                         G_CALLBACK(on_driver_download_checked), 
                         uesr_data);
        //self.reply_if_fail = [(x, fit[x]) for x in ppdnamelist]

        if(!init("download_driver", NULL, NULL, NULL, device_id, NULL, 0, 0))
        {
            remove_hold();
            return;
        }
    }
    */
    remove_hold();
    return true;
}
/*
gboolean on_driver_download_checked(scpinterface *interface,
                                    GDBusMethodInvocation *invocation,
                                    gpointer user_data)
{
    if(installed_files->len)
    {
        gulong index;
        fprintf(stderr, "GetBestDrivers request: Re-fetch PPDs after driver download\n");
        index = g_signal_connect(interface, 
                                 "ready", 
                                 G_CALLBACK(ppds_ready), 
                                 data);
        g_ptr_array_add (GBDRSignal_id, (gpointer) index);

        index = g_signal_connect(interface, 
                                 "error", 
                                 G_CALLBACK(ppds_error), 
                                 NULL);
        g_ptr_array_add (GBDRSignal_id, (gpointer) index);

        return true;
    }
    remove_hold();
    destroy();
}

gboolean on_dialog_canceled(scpinterface *interface,
                            GDBusMethodInvocation *invocation,
                            gpointer user_data)
{
    remove_hold();
    destroy();
}

*/
















