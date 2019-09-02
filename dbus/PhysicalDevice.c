#include "PhysicalDevice.h"

static make_model_data *canonical_id(device_obj *device);

static char *add_dot_local_if_needed(char *hostname);

static char *get_address(char *host);

static host_data *get_host_from_uri(char *uri);

static make_model_data *canonical_id(device_obj *device)
{
    char *mfg, *mdl, *mdll, *mfgl, *make_and_model;
    if(device->id_dict)
    {
        mfg = (char *)g_hash_table_lookup(device->id_dict, "MFG");
        mdl = (char *)g_hash_table_lookup(device->id_dict, "MDL");

        mfgl = (char *)malloc(sizeof(char) * strlen(mfg) + 1);
        strcpy(mfgl, mfg);
        mdll = (char *)malloc(sizeof(char) * strlen(mdl) + 1);
        strcpy(mdll, mdl);
        if(strlen(mfg) == 0 || startswith(strlwr(mfg), strlwr(mdll)))
        {
            make_and_model = (char *)malloc(sizeof(char) * strlen(mdl) + 1);
            strcpy(make_and_model, mdl);
        }
        else
        {
            make_and_model = (char *)malloc(sizeof(char) * (strlen(mfg) + strlen(mdl)) + 2);
            strcpy(make_and_model, mfg);
            strcat(make_and_model, " ");
            strcat(make_and_model, mdl);
        }
        free(mfgl);
        free(mdll);
    }
    else
    {
        make_and_model = (char *)malloc(sizeof(char) * strlen(device->make_and_model) + 1);
        strcpy(make_and_model, device->make_and_model);
    }

    return ppdMakeModelSplit (make_and_model); 
}

static char *add_dot_local_if_needed(char *hostname)
{
    if(hostname == NULL)
        return NULL;
    if((find(hostname, ".") == -1) && (find(hostname, ":") == -1) && strcmp(hostname, "localhost"))
    {
        char *hname = (char *)malloc(sizeof(char) * strlen(hostname) + 7);
        strcpy(hname, hostname);
        strcat(hname, ".local");
        return hname;
    }
    else
        return hostname;
}

static char *get_address(char *host)
{
    struct addrinfo hints, *res;
    int errcode;
    char addrstr[100];
    void *ptr;
    char *address;

    memset (&hints, 0, sizeof (hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;

    errcode = getaddrinfo (host, NULL, &hints, &res);
    if (errcode != 0)
    {
      perror ("getaddrinfo");
      return NULL;
    }

    //printf ("Host: %s\n", host);
    while (res)
    {
        inet_ntop (res->ai_family, res->ai_addr->sa_data, addrstr, 100);

        switch (res->ai_family)
        {
            case AF_INET:
                ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
                break;
            case AF_INET6:
                ptr = &((struct sockaddr_in6 *) res->ai_addr)->sin6_addr;
                break;
        }
        inet_ntop (res->ai_family, ptr, addrstr, 100);
        //printf ("IPv%d address: %s (%s)\n", res->ai_family == PF_INET6 ? 6 : 4, addrstr, res->ai_canonname);
        address = strdup(addrstr); 
        return address; // ?????
        res = res->ai_next;
    }
    return NULL;
}

static host_data *get_host_from_uri(char *uri)
{
    char *hostport = NULL,
         *host = NULL,
         *dnssdhost = NULL;

    char *ipparam, 
         *scheme, 
         *rest,
         *ip;

    host_data *data = (host_data *)malloc(sizeof(host_data));
    char *dev_uri = (char *)malloc(sizeof(char) * strlen(uri) + 1);
    strcpy(dev_uri, uri);

    scheme = strtok(dev_uri, ":");
    rest = strtok(NULL, ":");

    if(!strcmp(scheme, "hp") || !strcmp(scheme, "hpfax"))
    {
        ipparam = NULL;
        if(startswith("/net/", rest))
        {
            //(rest, ipparam) = urllib.parse.splitquery (rest[5:])
        }

        if(ipparam != NULL)
        {
            if(startswith("ip=", ipparam))
            {
                hostport = (char *)malloc(sizeof(char) * strlen(ipparam));
                slice(ipparam, hostport, 3);
            }  
            else if(startswith("hostname=", ipparam))
            {
                hostport = (char *)malloc(sizeof(char) * strlen(ipparam));
                slice(ipparam, hostport, 9);
            }
            else if(startswith("zc=", ipparam))
            {
                dnssdhost = (char *)malloc(sizeof(char) * strlen(ipparam));
                slice(ipparam, dnssdhost, 3);
            }
            else
            {
                data->host = NULL;
                data->dnssdhost = NULL;
                return data;
            }
        }
        else
        {
            data->host = NULL;
            data->dnssdhost = NULL;
            return data;
        }
    }
    else if (!strcmp(scheme, "dnssd") || !strcmp(scheme, "mdns"))
        return NULL;
    else
    {
        //(hostport, rest) = urllib.parse.splithost (rest)
        if(hostport == NULL)
        {
            data->host = NULL;
            data->dnssdhost = NULL;
            return data;
        }
    }

    if (hostport)
    {
        //(host, port) = urllib.parse.splitport (hostport)
    }

    if (host)
    {
        ip = NULL;
        ip = get_address(host);
        if(ip)
            host = ip;
    }
    else if(dnssdhost)
    {
        host = get_address(dnssdhost);
    }

    data->host = add_dot_local_if_needed(host);
    data->dnssdhost = add_dot_local_if_needed(dnssdhost);
    return data;
}

PhysicalDevice_data *PhysicalDevice()
{
    PhysicalDevice_data *data = (PhysicalDevice_data *)malloc(sizeof(PhysicalDevice_data));
    
    data->devices = NULL;
    data->network_host = NULL;
    data->dnssd_hostname = NULL;
    data->cupsserver = false;
    data->firsturi = NULL;
    data->user_data = g_hash_table_new(g_str_hash, g_str_equal);

    return data;
}

void add_device(device_obj *device, PhysicalDevice_data *pdata)
{
    char *mfg, *mdl, *mfg_lower, *mdl_lower, *sn, *host, *dnssdhost, *buffer;

    if (pdata->network_host != NULL || pdata->dnssd_hostname != NULL)
    {
        host_data *data = get_host_from_uri(device->uri);
        host = data->host;
        dnssdhost = data->dnssdhost;
        /*
            if (hasattr (device, 'address')):
                host = self._add_dot_local_if_needed(device.address)
            if (hasattr (device, 'hostname') and dnssdhost is None):
                dnssdhost = self._add_dot_local_if_needed(device.hostname)
        */
        if ((host == NULL && dnssdhost == NULL) ||
           (!((host && pdata->network_host && (!(strcmp(host, pdata->network_host)))) ||
           (host && pdata->dnssd_hostname && (!(strcmp(host, pdata->dnssd_hostname)))) ||
           (dnssdhost && pdata->network_host && (!(strcmp(dnssdhost, pdata->network_host)))) ||
           (dnssdhost && pdata->dnssd_hostname && (!(strcmp(dnssdhost, pdata->dnssd_hostname)))))) ||
           (host == NULL && pdata->dnssd_hostname == NULL) ||
           (dnssdhost == NULL && pdata->network_host))
        {
            fprintf(stderr, "Value Error!!!\n");
        }
    }
    else
    {
        make_model_data *data = canonical_id (device);
        mfg = data->make;
        mdl = data->model;
        if(pdata->devices == NULL)
        {
            mfg_lower = (char *)malloc(sizeof(char) * strlen(mfg) + 1);
            strcpy(mfg_lower, mfg);
            mdl_lower = (char *)malloc(sizeof(char) * strlen(mdl) + 1);
            strcpy(mdl_lower, mdl);

            strlwr(mfg_lower);
            strlwr(mdl_lower);

            sn = (char *)g_hash_table_lookup(device->id_dict, "SN");
            pdata->devices = g_ptr_array_new ();
        }
        else
        {

        }

        if(!strcmp(device->type, "socket"))
        {
            buffer = (char *)malloc(sizeof(char) * strlen(device->uri));
            strcpy(buffer, device->uri);
            device->uri = replace(buffer, ":9100", "");
        }
        if (((startswith("ipp:", device->uri) && 
           find(device->uri, "/printers/") != -1)) ||
           ((startswith("dnssd:", device->uri) || 
           startswith("mdns:", device->uri)) &&
           endswith("/cups", device->uri)))
        {
            pdata->cupsserver = true;
        }
        else if (pdata->cupsserver)
            return;
        /*
        for (int i = 0; i < (pdata->devices)->len; i++)
        {
            if(!strcmp((device_obj *)(g_ptr_array_index ((GPtrArray*)pdata->devices, i))->uri, device->uri))
                return;
        }
        */
        if(pdata->firsturi == NULL)
            pdata->firsturi = device->uri;

        g_ptr_array_add ((GPtrArray*)pdata->devices, (gpointer) device);

        //sort()

        /*

            if (not self._network_host or not self.dnssd_hostname) and \
           device.device_class == "network":
            # We just added a network device.
            self._network_host, dnssdhost = \
                self._get_host_from_uri (device.uri)
            if dnssdhost:
                self.dnssd_hostname = dnssdhost;

            if (hasattr (device, 'address') and self._network_host is None):
                address = device.address
                if address:
                    self._network_host = self._add_dot_local_if_needed(address)

            if (hasattr (device, 'hostname') and self.dnssd_hostname is None):
                hostname = device.hostname
                if hostname:
                    self.dnssd_hostname = self._add_dot_local_if_needed(hostname)

            if (self.dnssd_hostname and self._network_host is None):
                try:
                    self._network_host = self._get_address(hostname);
                except:
                    self._network_host = None

            debugprint("Device %s added to physical device: %s" %
                       (device.uri, repr(self)))
        */
    }
}

