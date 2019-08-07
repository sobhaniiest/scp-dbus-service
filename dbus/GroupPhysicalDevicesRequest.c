#include "GroupPhysicalDevicesRequest.h"

void GPDRequest(GDBusConnection *connection,
                GHashTable *devices,
                void(*reply_handler)(), 
                void(*error_handler)())
{
    add_hold();

    GHashTable *need_resolving = g_hash_table_new(g_str_hash, g_str_equal);
    GHashTable *deviceobjs = g_hash_table_new(g_str_hash, g_str_equal);

    GHashTableIter iter;
    gpointer device_uri, device_dict;
    g_hash_table_iter_init(&iter, devices);
    while (g_hash_table_iter_next(&iter, &device_uri, &device_dict))
    {
        device_obj *devobj = Device((char *)device_uri, (devices_attr *)device_dict);
        g_hash_table_insert(deviceobjs, (char *)device_uri, devobj);

        if(startswith("dnssd://", (char *)device_uri))
            g_hash_table_insert(need_resolving, (char *)device_uri, devobj);        
    }

    if(g_hash_table_size(need_resolving) > 0)
        DNSSDHostNamesResolver(need_resolving, group);
    else
        group(deviceobjs);
    
}

void group(GHashTable *deviceobjs)
{
    /*
        We can ignore resolved_devices because the actual objects
        (in self.devices) have been modified.
    */
    bool matched;
    GPtrArray *physdevs = g_ptr_array_new ();
    GHashTableIter iter;
    gpointer device_uri, deviceobj;
    g_hash_table_iter_init(&iter, deviceobjs);
    while (g_hash_table_iter_next(&iter, &device_uri, &deviceobj))
    {
        PhysicalDevice ((device_obj *)deviceobj);

        matched = false;
        
    }






}

device_obj *Device(char *uri, devices_attr *dict)
{
    /*
        @param uri: device URI
        @type uri: string
        @param kw: device attributes
        @type kw: dict
    */

    char *dev_make_and_model = dict->device_make_and_model;
    char *dev_id = dict->device_id;
    //char *dev_classes = dict->device_classes;
    //char *info = dict->device_info;
    //char *location = dict->device_location;

    int count_pieces;
    char **uri_pieces;
    count_pieces = count_tokens(uri, ':');
    uri_pieces = split(uri, ":", count_pieces);

    char *type = uri_pieces[0];
    bool is_class;
    if(count_pieces == 1)
        is_class = true;
    else
        is_class = false;

    GHashTable *id_dict = parseDeviceID(dev_id);

    int s = find(uri, "serial=");
    if((s != -1) && (strcmp(g_hash_table_lookup(id_dict, (char *)"SN"), "")))
    {
        char *buffer = (char *)malloc(strlen(uri)-(s+6));
        slice(uri, buffer, (s+7));
        g_hash_table_replace(id_dict, "SN", buffer);
    }

    device_obj *deviceobj = (device_obj *)malloc(sizeof(device_obj));
    deviceobj->is_class = is_class;
    deviceobj->type = type;
    deviceobj->uri = uri;
    deviceobj->id = dev_id;
    deviceobj->info = info;
    
    return deviceobj;
}

bool dev_compare(device_obj *devobj)
{
    /* Compare devices by order of preference. */

    if(!devobj)
        return true;


}



void DNSSDHostNamesResolver(GHashTable *devices, void(*group)())
{
    guint unresolved = g_hash_table_size (devices);
    GHashTable *device_uri_by_name = g_hash_table_new(g_str_hash, g_str_equal);

    // g_bus_get(G_BUS_TYPE_SYSTEM, NULL, NULL, NULL);

    GHashTableIter iter;
    gpointer uri, device;
    g_hash_table_iter_init(&iter, devices);
    while (g_hash_table_iter_next(&iter, &uri, &device))
    {
        if(!startswith("dnssd://", (char *)uri))
        {
            unresolved--;
            continue;
        }   

        char *result = (char *)malloc(strlen((char *)uri));
        strcpy(result, (char *)uri);

        char *scheme = strtok(result, ":");
        result = strtok(NULL, ":");

        char *netlock = (char *)malloc(strlen(result));
        int k,i;
        if(result[0] == '/')
        {
            if(result[1] == '/')
            {
                k = 2;
                i = 0;
                while(result[k] != '/')
                {
                    netlock[i] = result[k];
                    i++;
                    k++;
                }
                netlock[i] = '\0';
            }
        }

        int count = count_tokens(netlock, '.');
        char **elements = split(netlock, ".", count);

        if(count != 4)
        {
            resolved();
            continue;
        }
        
        char *name = NULL, 
             *stype = NULL, 
             *protocal = NULL, 
             *domain = NULL;
        if(elements[4])
        {
            domain = (char *)malloc(strlen(elements[4]));
            strcpy(domain, elements[4]);
        }
            
        protocal = (char *)malloc(strlen(elements[3]));
        strcpy(protocal, elements[3]);

        stype = (char *)malloc(strlen(elements[2])+strlen(elements[3])+1);
        strcpy(stype, elements[2]);

        name = (char *)malloc(strlen(elements[1])+strlen(elements[0]));
        strcpy(name, elements[0]);
        strcat(name, elements[1]);

        char *buffer = (char *)malloc(strlen(name));
        int b = 0;
        for(i=0;i<strlen(name);i++)
        {
            if(name[i] == '%')
            {
                buffer[b] = ' ';
                i += 3;
                b++;
            }
            buffer[b] = name[i];
            b++;
        }

        strcat(stype, ".");
        strcat(stype, protocal);

        ResolveService(name, stype, domain);
    }
}