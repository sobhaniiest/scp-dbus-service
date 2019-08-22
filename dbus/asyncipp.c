#include "asyncipp.h"

static void op_error_handler();
static void set_ipp_error (ipp_status_t status, const char *message);

static void op_error_handler()
{

}

static void set_ipp_error (ipp_status_t status, const char *message)
{
    if (!message)
        message = ippErrorString (status);
    fprintf(stderr, "set_ipp_error: %d, %s\n",(int)status, message);
}


GHashTable *getURI(http_t *new)
{
    GHashTable *result = g_hash_table_new(g_str_hash, g_str_equal);
	ipp_t *request, *answer;
  	ipp_attribute_t *attr;
    const char *printer = NULL;
    const char *uri = NULL;
    //const char *attributes[] = { "printer-name", "device-uri" };
    bool flag_printer = false,
         flag_uri = false;

    request = ippNewRequest(CUPS_GET_PRINTERS);

    /*
  	ippAddStrings(request, 
  		          IPP_TAG_OPERATION, 
  		          IPP_TAG_KEYWORD,
                  "requested-attributes",
                  sizeof (attributes) / sizeof (attributes[0]),
                  NULL, 
                  attributes);
    */
  	answer = cupsDoRequest (new, request, "/");

  	if (!answer || ippGetStatusCode (answer) > IPP_OK_CONFLICT) 
    {
        if (answer && ippGetStatusCode (answer) == IPP_NOT_FOUND) 
        {
            fprintf(stderr, "No Printers\n");
            ippDelete (answer);
            return NULL;
        }

        set_ipp_error (answer ? ippGetStatusCode (answer) : cupsLastError (),
                       answer ? NULL : cupsLastErrorString ());
        if (answer)
            ippDelete (answer);
        fprintf(stderr, "Error\n");
        return NULL;
    }

    for(ippFirstAttribute (answer); attr; attr = ippNextAttribute (answer))
    {
 
      	while (attr && ippGetGroupTag (attr) != IPP_TAG_PRINTER)
        	   attr = ippNextAttribute (answer);

      	if(!attr)
        		break;

      	for (; attr && ippGetGroupTag (attr) == IPP_TAG_PRINTER; attr = ippNextAttribute (answer)) 
      	{
  	    	if (!strcmp (ippGetName (attr), "printer-name") && ippGetValueTag (attr) == IPP_TAG_NAME)
    	    {
                printer = (char *) ippGetString (attr, 0, NULL);
                flag_printer = true;
                //fprintf(stderr, "%s\n",printer);
            }   
  	      	else if ((!strcmp (ippGetName (attr), "device-uri")) && ippGetValueTag (attr) == IPP_TAG_URI) 
    	    {
                uri = ippGetString(attr, 0, NULL);
                flag_uri = true;
                //fprintf(stderr, "%s\n",uri);
            }

            if(flag_printer && flag_uri)
            {
                //fprintf(stderr, "%s : %s\n", printer, uri);
                g_hash_table_insert(result, (char *)printer, (char *)uri);
                flag_printer = false;
                flag_uri = false;
            }
    	  }

      	if(!attr)
        		break;
   	} 
   	//ippDelete (answer);
    return result;
}

GHashTable *getPPDs(http_t *new, int all_lists)
{
    ppds_attr *list = NULL;
    GHashTable *result = g_hash_table_new(g_str_hash, g_str_equal);

    ipp_t *request, *answer;
    ipp_attribute_t *attr;

    const char *ppdname = NULL;
    int ppd_model_number = -1;
    char *ppd_natural_language = NULL,
         *ppd_device_id = NULL,
         *ppd_make = NULL,
         *ppd_make_and_model = NULL,
         *ppd_product = NULL,
         *ppd_psversion = NULL,
         *ppd_type = NULL;

    bool flag_natural_language = false,
         flag_device_id = false,
         flag_make = false,
         flag_make_and_model = false,
         flag_product = false,
         flag_psversion = false,
         flag_type = false,
         flag_ppdname = false,
         flag_model_number = false;

    request = ippNewRequest(CUPS_GET_PPDS); 

    answer = cupsDoRequest (new, request, "/");

    if (!answer || ippGetStatusCode (answer) > IPP_OK_CONFLICT) 
    {
        set_ipp_error (answer ? ippGetStatusCode (answer) : cupsLastError (),
                       answer ? NULL : cupsLastErrorString ());
        if (answer)
            ippDelete (answer);
        fprintf(stderr, "getPPDs(): error\n");
        return NULL;
    }

    for (attr = ippFirstAttribute (answer); attr; attr = ippNextAttribute (answer)) 
    {
        while (attr && ippGetGroupTag (attr) != IPP_TAG_PRINTER)
            attr = ippNextAttribute (answer);

        if (!attr)
            break;

        for (; attr && ippGetGroupTag (attr) == IPP_TAG_PRINTER; attr = ippNextAttribute (answer)) 
        {
            if (!strcmp (ippGetName (attr), "ppd-name") && ippGetValueTag (attr) == IPP_TAG_NAME)
            {
                ppdname = (char *) ippGetString (attr, 0, NULL);
                flag_ppdname = true;
            }
            else if (!strcmp (ippGetName (attr), "ppd-natural-language") && ippGetValueTag (attr) == IPP_TAG_LANGUAGE)
            {
                ppd_natural_language = (char *) ippGetString (attr, 0, NULL);
                flag_natural_language = true;
            }
            else if (!strcmp (ippGetName (attr), "ppd-device-id") && ippGetValueTag (attr) == IPP_TAG_TEXT)
            {
                ppd_device_id = (char *) ippGetString (attr, 0, NULL);
                flag_device_id = true;
            }
            else if (!strcmp (ippGetName (attr), "ppd-make") && ippGetValueTag (attr) == IPP_TAG_TEXT)
            {
                ppd_make = (char *) ippGetString (attr, 0, NULL);
                flag_make = true;
            }
            else if (!strcmp (ippGetName (attr), "ppd-make-and-model") && ippGetValueTag (attr) == IPP_TAG_TEXT)
            {
                ppd_make_and_model = (char *) ippGetString (attr, 0, NULL);
                flag_make_and_model = true;
            }
            else if (!strcmp (ippGetName (attr), "ppd-product") && ippGetValueTag (attr) == IPP_TAG_TEXT)
            {
                ppd_product = (char *) ippGetString (attr, 0, NULL);
                flag_product = true;
            }
            else if (!strcmp (ippGetName (attr), "ppd-psversion") && ippGetValueTag (attr) == IPP_TAG_TEXT)
            {
                ppd_psversion = (char *) ippGetString (attr, 0, NULL);
                flag_psversion = true;
            }
            else if (!strcmp (ippGetName (attr), "ppd-type") && ippGetValueTag (attr) == IPP_TAG_KEYWORD)
            {
                ppd_type = (char *) ippGetString (attr, 0, NULL);
                flag_type = true;
            }
            else if (!strcmp (ippGetName (attr), "ppd-model-number") && ippGetValueTag (attr) == IPP_TAG_INTEGER)
            {
                ppd_model_number = ippGetInteger (attr, 0);
                flag_model_number = true;
            }

            if(flag_ppdname && flag_natural_language && flag_device_id 
               && flag_make && flag_make_and_model && flag_product
               && flag_psversion && flag_type && flag_model_number)
            {
                list = (ppds_attr *)malloc(sizeof(ppds_attr));
                list->ppd_natural_language = ppd_natural_language;
                list->ppd_device_id = ppd_device_id;
                list->ppd_make = ppd_make;
                list->ppd_make_and_model = ppd_make_and_model;
                list->ppd_model_number = ppd_model_number;
                list->ppd_product = ppd_product;
                list->ppd_psversion = ppd_psversion;
                list->ppd_type = ppd_type;
                g_hash_table_insert(result, (char *)ppdname, list);

                flag_ppdname = false;
                flag_natural_language = false;
                flag_device_id = false;
                flag_make = false;
                flag_make_and_model = false;
                flag_product = false;
                flag_psversion = false;
                flag_type = false;
                flag_model_number = false;
            }
        }

        if (!attr)
            break;
    }
    /*
    free(ppd_natural_language);
    free(ppd_device_id);
    free(ppd_make);
    free(ppd_make_and_model);
    free(ppd_product);
    free(ppd_psversion);
    free(ppd_type);
    */
    fprintf(stderr, "size : %d\n", g_hash_table_size(result));
    //ippDelete (answer);
    return result;
}

GHashTable *getDevices(http_t *new)
{
    devices_attr *list = NULL;
    GHashTable *result = g_hash_table_new(g_str_hash, g_str_equal);

    ipp_t *request, *answer;
    ipp_attribute_t *attr;
    
    char *device_uri = NULL;
    char *device_info = NULL;
    char *device_location = NULL;
    char *device_make_and_model = NULL;
    char *device_id = NULL;
    char *device_classes = NULL;
    const char *atrb = NULL;
    
    request = ippNewRequest(CUPS_GET_DEVICES); 

    answer = cupsDoRequest (new, request, "/");

    if (!answer || ippGetStatusCode (answer) > IPP_OK_CONFLICT) 
    {
        set_ipp_error (answer ? ippGetStatusCode (answer) : cupsLastError (),
                       answer ? NULL : cupsLastErrorString ());
        if (answer)
            ippDelete (answer);
        fprintf(stderr, "getDevices(): error\n");
        return NULL;
    }

    for (attr = ippFirstAttribute (answer); attr; attr = ippNextAttribute (answer)) 
    {
        while (attr && ippGetGroupTag (attr) != IPP_TAG_PRINTER)
            attr = ippNextAttribute (answer);

        if (!attr)
            break;

        for (; attr && ippGetGroupTag (attr) == IPP_TAG_PRINTER; attr = ippNextAttribute (answer)) 
        {
            atrb = ippGetName (attr);
            if (!strcmp (ippGetName (attr), "device-uri") && ippGetValueTag (attr) == IPP_TAG_URI)
                device_uri = (char *) ippGetString (attr, 0, NULL);
            else if (!strcmp (ippGetName (attr), "device-id") && ippGetValueTag (attr) == IPP_TAG_TEXT)
                device_id = (char *) ippGetString (attr, 0, NULL);
            else if (!strcmp (ippGetName (attr), "device-make-and-model") && ippGetValueTag (attr) == IPP_TAG_TEXT)
                device_make_and_model = (char *) ippGetString (attr, 0, NULL);
            else if (!strcmp (ippGetName (attr), "device-classes") && ippGetValueTag (attr) == IPP_TAG_TEXT)
                device_classes = (char *) ippGetString (attr, 0, NULL);
            else if (!strcmp (ippGetName (attr), "device-info") && ippGetValueTag (attr) == IPP_TAG_TEXT)
                device_info = (char *) ippGetString (attr, 0, NULL);
            else if (!strcmp (ippGetName (attr), "device-location") && ippGetValueTag (attr) == IPP_TAG_TEXT)
                device_location = (char *) ippGetString (attr, 0, NULL);


            if(device_uri != NULL && device_id != NULL && device_make_and_model != NULL)
            {
                list = (devices_attr *)malloc(sizeof(devices_attr));
                list->device_make_and_model = device_make_and_model;
                list->device_id = device_id;
                g_hash_table_insert(result, (char *)device_uri, list);

                device_uri = NULL;
                device_make_and_model = NULL;
                device_id = NULL;
            }
        }

        if (!attr)
            break;
    }
    /*
    free(device_make_and_model);
    free(device_id);
    free(device_info);
    free(device_location);
    */
    //ippDelete (answer);
    return result;
}

http_t *IPPAuthConnection(void(*reply_handler)(), 
                          void(*error_handler)(), 
                          void(*auth_handler)(), 
                          const char *host, 
                          int port, 
                          http_encryption_t encryption, 
                          bool try_as_root, 
                          bool prompt_allowed)
{
  	const char *user = NULL;
  	dict *creds = lookup_auth_info(host, port);
  	if(creds != NULL)
  	{
    		if(strcmp(creds->hname, "root") || try_as_root)
    			user = creds->hname;
    		free(creds);
  	}

  	http_t *new = IPPConnection(reply_handler, 
                                error_handler, 
                                auth_handler,
                                user, 
                                host, 
                                port,
                                encryption);

    if(new == NULL)
      op_error_handler();
    else if(reply_handler != NULL)
      reply_handler();

    return new;
}

http_t *IPPConnection(void(*reply_handler)(), 
                      void(*error_handler)(), 
                      void(*auth_handler)(),
                      const char *user, 
                      const char *host, 
                      int port, 
                      http_encryption_t encryption)
{
    http_t *http = httpConnectEncrypt (host, port, (http_encryption_t) encryption);

    if(http == NULL)
    {
        fprintf(stderr, "Connection Error!!!\n");
        return NULL;
    }
    //httpClose(http);
    return http;
}
