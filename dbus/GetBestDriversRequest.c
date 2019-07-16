#include "GetBestDriversRequest.h"

//g_signal_handler_disconnect(interface, id);

bool g_ppds = false;
gulong *id = NULL;
int id_index = 0;

static void insert_id(device_id_dict **head, char *name, char *value)
{
    device_id_dict *c = (*head);
  
    if(*head == NULL)
    {
        (*head) = (device_id_dict *)malloc(sizeof(device_id_dict));
        (*head)->name = name;
        (*head)->value = value;
        (*head)->next = NULL;
    }
    else
    {
        while(c->next != NULL)
            c = c->next;
        c->next = (device_id_dict *)malloc(sizeof(device_id_dict));
        c->next->name = name;
        c->next->value = value;
        c->next->next = NULL;
    }
}

void GBDRequest(scpinterface *interface,
                const gchar *device_id,
                const gchar *device_make_and_model,
                const gchar *device_uri,
                char *language,
                http_t *status)
{
    bool download_tried = false;
    ppds_attr *ppdnames;
    add_hold();

    if(!g_ppds)
    {
        fprintf(stderr, "GetBestDrivers request: need to fetch PPDs\n");
        g_ppds = true;
        ppdnames = NULL;
        id[id_index++] = g_signal_connect(interface, 
                                          "ready", 
                                          G_CALLBACK(ppds_ready), 
                                          NULL);
        id[id_index++] = g_signal_connect(interface, 
                                          "error", 
                                          G_CALLBACK(ppds_error), 
                                          NULL);
        fprintf(stderr, "FetchPPDs: running\n");

        ppdnames = getPPDs(status, 1);
        if(!ppdnames || !status)
        {
            fprintf(stderr, "FetchPPDs: error\n");
            scp_interface__emit_error(interface);
        }
        else
        {
            fprintf(stderr, "FetchPPDs: success\n");
            PPDs(&ppdnames, language, NULL);
            scp_interface__emit_ready(interface);
        }
    }
    else
    {
        if(!ppdnames)
        {
            fprintf(stderr, "GetBestDrivers request: PPDs already fetched\n");
            ppds_ready();
        }
        else
        {
            fprintf(stderr, "GetBestDrivers request: waiting for PPDs\n");
            id[id_index++] = g_signal_connect(interface, 
                                              "ready", 
                                              G_CALLBACK(ppds_ready), 
                                              NULL);
            id[id_index++] = g_signal_connect(interface, 
                                              "error", 
                                              G_CALLBACK(ppds_error), 
                                              NULL);
        }
    }
    
}


void ppds_error(scpinterface *interface)
{
    int i;
    for(i=0;i<id_index;i++)
        g_signal_handler_disconnect(interface, id[i]);
}

void ppds_ready()
{
    if(ppdnames)
    {
        fprintf(stderr, "PPDs being reloaded. Wait for next 'ready' signal\n");
        return;
    }

    ppds_error();

    device_id_dict *id_dict = NULL;

    if(device_id)
        id_dict = parseDeviceID(device_id);
    else
    {
        char *mfg_mdl = ppdMakeModelSplit(device_make_and_model);
        char *mfg, *mdl;
        mfg = strtok(mfg_mdl, ":");
        mdl = strtok(NULL, ":");
        insert_id(&id_dict, "MFG", mfg);
        insert_id(&id_dict, "MDL", mdl);
        insert_id(&id_dict, "DES", "");
        insert_id(&id_dict, "CMD", "");
        size_t len = trlen(mfg)+strlen(mdl)+11;
        device_id = (char *)malloc(len);
        snprintf(device_id, len, "MFG:%s;MDL:%s;", mfg, mdl);
    }
    
    /*
    fit = getPPDNamesFromDeviceID();

    ppdnamelist = orderPPDNamesByPreference();

    ppdname = ppdnamelist[0];
    status = fit[ppdname];
    */

}



















device_id_dict *parseDeviceID(char *id)
{
    /*

        Parse an IEEE 1284 Device ID, so that it may be indexed by field name.

        @param id: IEEE 1284 Device ID, without the two leading length bytes
        @type id: string
        @returns: dict indexed by field name

    */

    device_id_dict *id_dict = NULL;
    int count_id, i, j;
    char **pieces;
    char *name, *value;
    char *device_name[9] = {"MFG", "MDL", "CMD", "CLS", "DES", "SN", "S", "P", "J"};
    char *id = (char *)malloc(strlen(device_id));
    strcpy(id,device_id);

    count_id = count_tokens(id, ';');
    pieces = split(id, ";", count_id);

    for(i=0;i<count_id;i++)
    {
        if(!(strstr(pieces[i], ":")))
            continue;

        name = strtok(pieces[i], ":");
        value = strtok(NULL, ":");
        insert_id(&id_dict, strstrip(name), strstrip(value));
    }

    device_id_dict *c = id_dict;
    bool found_mfg = false,
         found_mdl = false,
         found_cmd = false,
         found_name;

    while(c != NULL)
    {
        if(!strcmp(c->name, "MFG"))
            found_mfg = true;
        if(!strcmp(c->name, "MDL"))
            found_mdl = true;
        if(!strcmp(c->name, "CMD"))
            found_cmd = true;
        c = c->next;
    }

    c = id_dict;
    while(c != NULL)
    {
        if(!strcmp(c->name, "MANUFACTURER") && !found_mfg)
            insert_id(&id_dict, "MFG", c->value);
        if(!strcmp(c->name, "MODEL") && !found_mdl)
            insert_id(&id_dict, "MDL", c->value);
        if(!strcmp(c->name, "COMMAND SET") && !found_cmd)
            insert_id(&id_dict, "CMD", c->value);
        c = c->next;
    }

    for(i=0;i<9;i++)
    {
        c = id_dict;
        found_name = false;
        while(c != NULL)
        {
            if(!(strcmp(c->name, device_name[i])))
                found_name = true;
            c = c->next;
        }
        if(!found_name)
            insert_id(&id_dict, device_name[i], "");
    }

    /*
    if id_dict["CMD"] == '':
        id_dict["CMD"] = []
    else:
        id_dict["CMD"] = id_dict["CMD"].split(',') 
    */
    return id_dict;
}

