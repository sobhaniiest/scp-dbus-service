#include "GroupPhysicalDevicesRequest.h"

static AvahiSimplePoll *simple_poll = NULL;
guint unresolved;

device_obj *Device(char *uri, devices_attr *dict);
void group(GHashTable *deviceobjs);
void DNSSDHostNamesResolver(GHashTable *devices, void(*group)());

GVariant *GPDRequest(scpinterface *interface,
                     GHashTable *devices,
                     void(*reply_handler)(), 
                     void(*error_handler)())
{
    add_hold();
    /*
        @type need_resolving: dict
        key: device_uri(string)
        value: device_obj(struct - GroupPhysicalDevicesRequest.h) 
    */
    GHashTable *need_resolving = g_hash_table_new(g_str_hash, g_str_equal);
    /*
        @type deviceobjs: dict
        key: device_uri(string)
        value: device_obj(struct - GroupPhysicalDevicesRequest.h) 
    */
    GHashTable *deviceobjs = g_hash_table_new(g_str_hash, g_str_equal);

    GHashTableIter iter;
    gpointer device_uri, device_dict;
    g_hash_table_iter_init(&iter, devices);
    while (g_hash_table_iter_next(&iter, &device_uri, &device_dict))
    {
        device_obj *devobj = Device((char *)device_uri, (devices_attr *)device_dict);
        g_hash_table_insert(deviceobjs, (char *)device_uri, devobj);

        if (startswith("dnssd://", (char *)device_uri))
            g_hash_table_insert(need_resolving, (char *)device_uri, devobj); 
    }

    if (g_hash_table_size(need_resolving) > 0)
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
        @param dict: device attributes
        @type dict: struct type of devices_attr in asyncipp.h
    */

    char *dev_make_and_model = dict->device_make_and_model;
    char *dev_id = dict->device_id;
    char *dev_classes = dict->device_classes;
    char *info = dict->device_info;
    char *location = dict->device_location;

    int count_pieces;
    char **uri_pieces;
    count_pieces = count_tokens(uri, ':');
    uri_pieces = split(uri, ":", count_pieces);

    char *type = uri_pieces[0];
    bool is_class;
    if (count_pieces == 1)
        is_class = true;
    else
        is_class = false;

    GHashTable *id_dict = parseDeviceID(dev_id);

    int s = find(uri, "serial=");
    if ((s != -1) && (strcmp(g_hash_table_lookup(id_dict, (char *)"SN"), "")))
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
    unresolved = g_hash_table_size (devices);
    /*
        @type device_uri_by_name: dict
        key: uri_by_name(struct - GroupPhysicalDevicesRequest.h)
        value: uri(string) 
    */
    GHashTable *device_uri_by_name = g_hash_table_new(g_str_hash, g_str_equal);

    // g_bus_get(G_BUS_TYPE_SYSTEM, NULL, NULL, NULL);

    /* resolve */

    GHashTableIter iter;
    gpointer uri, device;
    g_hash_table_iter_init(&iter, devices);
    while (g_hash_table_iter_next(&iter, &uri, &device))
    {
        if (!startswith("dnssd://", (char *)uri))
        {
            unresolved--;
            continue;
        }   

        char *result = (char *)malloc(sizeof(char) * strlen((char *)uri) + 1);
        strcpy(result, (char *)uri);

        char *scheme = strtok(result, ":");
        result = strtok(NULL, ":");

        char *netlock = (char *)malloc(sizeof(char) * strlen(result) + 1);
        int k,i;
        if (result[0] == '/')
        {
            if (result[1] == '/')
            {
                k = 2;
                i = 0;
                while (result[k] != '/')
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

        if (count != 4)
        {
            resolved();
            continue;
        }
        
        char *name = NULL, 
             *stype = NULL, 
             *protocal = NULL, 
             *domain = NULL;
        if (elements[4])
        {
            domain = (char *)malloc(sizeof(char) * strlen(elements[4]) + 1);
            strcpy(domain, elements[4]);
        }
            
        protocal = (char *)malloc(sizeof(char) * strlen(elements[3]) + 1);
        strcpy(protocal, elements[3]);

        stype = (char *)malloc(sizeof(char) * (strlen(elements[2])+strlen(elements[3])) + 1);
        strcpy(stype, elements[2]);

        name = (char *)malloc(sizeof(char) * (strlen(elements[1])+strlen(elements[0])) + 1);
        strcpy(name, elements[0]);
        strcat(name, elements[1]);

        char *buffer = (char *)malloc(sizeof(char) * strlen(name) + 1);
        int b = 0;
        for (int i = 0; i < strlen(name); i++)
        {
            if (name[i] == '%')
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

        uri_by_name *data = (uri_by_name *)malloc(sizeof(uri_by_name));
        data->name = name;
        data->stype = stype;
        data->domain = domain;
        g_hash_table_insert(device_uri_by_name, uri, data);

        fprintf(stderr, "Resolving address\n" );

        if (ResolveService(name, stype, domain, device_uri_by_name) == 1)////
            error();
    }
}

static void resolve_callback(AvahiServiceResolver *r,
                             AVAHI_GCC_UNUSED AvahiIfIndex interface,
                             AVAHI_GCC_UNUSED AvahiProtocol protocol,
                             AvahiResolverEvent event,
                             const char *name,
                             const char *type,
                             const char *domain,
                             const char *host_name,
                             const AvahiAddress *address,
                             uint16_t port,
                             AvahiStringList *txt,
                             AvahiLookupResultFlags flags,
                             AVAHI_GCC_UNUSED void * userdata) 
{
    assert(r);
    /* Called whenever a service has been resolved successfully or timed out */
    switch (event) 
    {
        case AVAHI_RESOLVER_FAILURE:
            fprintf(stderr, 
                    "(Resolver) Failed to resolve service"
                    " '%s' of type '%s' in domain '%s': %s\n", 
                    name, 
                    type, 
                    domain, 
                    avahi_strerror(avahi_client_errno(avahi_service_resolver_get_client(r))));
            break;
        case AVAHI_RESOLVER_FOUND: 
        {
            char a[AVAHI_ADDRESS_STR_MAX], *t;
            fprintf(stderr, "Service '%s' of type '%s' in domain '%s':\n", name, type, domain);
            avahi_address_snprint(a, sizeof(a), address);
            t = avahi_string_list_to_string(txt);
            fprintf(stderr,
                    "\t%s:%u (%s)\n"
                    "\tTXT=%s\n"
                    "\tcookie is %u\n"
                    "\tis_local: %i\n"
                    "\tour_own: %i\n"
                    "\twide_area: %i\n"
                    "\tmulticast: %i\n"
                    "\tcached: %i\n",
                    host_name, port, a,
                    t,
                    avahi_string_list_get_service_cookie(txt),
                    !!(flags & AVAHI_LOOKUP_RESULT_LOCAL),
                    !!(flags & AVAHI_LOOKUP_RESULT_OUR_OWN),
                    !!(flags & AVAHI_LOOKUP_RESULT_WIDE_AREA),
                    !!(flags & AVAHI_LOOKUP_RESULT_MULTICAST),
                    !!(flags & AVAHI_LOOKUP_RESULT_CACHED));
            unresolved--;
            if(unresolved == 0)
            {
                fprintf(stderr, "All addresses resolved\n");
            }
            avahi_free(t);
        }
    }
    avahi_service_resolver_free(r);
}

static void client_callback(AvahiClient *c, 
                            AvahiClientState state, 
                            AVAHI_GCC_UNUSED void * userdata) 
{
    assert(c);
    /* Called whenever the client or server state changes */
    if (state == AVAHI_CLIENT_FAILURE) 
    {
        fprintf(stderr, "Server connection failure: %s\n", avahi_strerror(avahi_client_errno(c)));
        avahi_simple_poll_quit(simple_poll);
    }
}

int ResolveService(char *name,
                   char *type,
                   char *domain,
                   GHashTable *dev_uri_by_name)
{
    AvahiClient *client = NULL;
    AvahiServiceBrowser *sb = NULL;
    int error;
    int ret = 1;
    /* Allocate main loop object */
    if (!(simple_poll = avahi_simple_poll_new())) 
    {
        fprintf(stderr, "Failed to create simple poll object.\n");
        goto fail;
    }
    /* Allocate a new client */
    client = avahi_client_new(avahi_simple_poll_get(simple_poll), 
                              0, 
                              client_callback, 
                              NULL, 
                              &error);
    /* Check wether creating the client object succeeded */
    if (!client) 
    {
        fprintf(stderr, "Failed to create client: %s\n", avahi_strerror(error));
        goto fail;
    }
    
    if (!(avahi_service_resolver_new(client, 
                                     AVAHI_IF_UNSPEC, 
                                     AVAHI_PROTO_UNSPEC, 
                                     name,
                                     type,
                                     domain,
                                     AVAHI_PROTO_UNSPEC, 
                                     0, 
                                     resolve_callback, 
                                     dev_uri_by_name)))///
        fprintf(stderr, "Failed to resolve service '': %s\n", avahi_strerror(avahi_client_errno(client)));

    /* Run the main loop */
    avahi_simple_poll_loop(simple_poll);
    ret = 0;
fail:
    /* Cleanup things */
    if (sb)
        avahi_service_browser_free(sb);
    if (client)
        avahi_client_free(client);
    if (simple_poll)
        avahi_simple_poll_free(simple_poll);
    return ret;
}