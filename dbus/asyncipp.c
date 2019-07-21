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
	  ipp_t *request = ippNewRequest(CUPS_GET_PRINTERS), *answer;
  	ipp_attribute_t *attr;
    const char *printer = NULL;
    const char *uri = NULL;
    const char *attributes[] = { "printer-name", "device-uri" };

  	ippAddStrings(request, 
  		            IPP_TAG_OPERATION, 
  		            IPP_TAG_KEYWORD,
                  "requested-attributes",
                  sizeof (attributes) / sizeof (attributes[0]),
                  NULL, 
                  attributes);
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
    	        	printer = (char *) ippGetString (attr, 0, NULL);
  	      	else if ((!strcmp (ippGetName (attr), "device-uri")) && ippGetValueTag (attr) == IPP_TAG_URI) 
    	      		uri = ippGetString(attr, 0, NULL);

            if(printer != NULL && uri != NULL)
            {
                g_hash_table_insert(result, (char *)printer, (char *)uri);
                printer = NULL;
                uri = NULL;
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
    char *ppd_natural_language = NULL;
    char *ppd_device_id = NULL;
    char *ppd_make = NULL;
    char *ppd_make_and_model = NULL;
    int ppd_model_number = -1;
    char *ppd_product = NULL;
    char *ppd_psversion = NULL;
    char *ppd_type = NULL;

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
                ppdname = (char *) ippGetString (attr, 0, NULL);
            else if (!strcmp (ippGetName (attr), "ppd-natural-language") && ippGetValueTag (attr) == IPP_TAG_LANGUAGE)
                ppd_natural_language = (char *) ippGetString (attr, 0, NULL);
            else if (!strcmp (ippGetName (attr), "ppd-device-id") && ippGetValueTag (attr) == IPP_TAG_TEXT)
                ppd_device_id = (char *) ippGetString (attr, 0, NULL);
            else if (!strcmp (ippGetName (attr), "ppd-make") && ippGetValueTag (attr) == IPP_TAG_TEXT)
                ppd_make = (char *) ippGetString (attr, 0, NULL);
            else if (!strcmp (ippGetName (attr), "ppd-make-and-model") && ippGetValueTag (attr) == IPP_TAG_TEXT)
                ppd_make_and_model = (char *) ippGetString (attr, 0, NULL);
            else if (!strcmp (ippGetName (attr), "ppd-product") && ippGetValueTag (attr) == IPP_TAG_TEXT)
                ppd_product = (char *) ippGetString (attr, 0, NULL);
            else if (!strcmp (ippGetName (attr), "ppd-psversion") && ippGetValueTag (attr) == IPP_TAG_TEXT)
                ppd_psversion = (char *) ippGetString (attr, 0, NULL);
            else if (!strcmp (ippGetName (attr), "ppd-type") && ippGetValueTag (attr) == IPP_TAG_KEYWORD)
                ppd_type = (char *) ippGetString (attr, 0, NULL);
            else if (!strcmp (ippGetName (attr), "ppd-model-number") && ippGetValueTag (attr) == IPP_TAG_INTEGER)
                ppd_model_number = ippGetInteger (attr, 0);

            if(ppdname != NULL && ppd_natural_language != NULL && ppd_device_id != NULL 
              && ppd_make != NULL && ppd_make_and_model != NULL && ppd_product != NULL 
              && ppd_psversion != NULL && ppd_type != NULL && ppd_model_number != -1)
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

                ppdname = NULL;
                ppd_natural_language = NULL;
                ppd_device_id = NULL;
                ppd_make = NULL;
                ppd_make_and_model = NULL;
                ppd_product = NULL;
                ppd_psversion = NULL;
                ppd_type = NULL;
                ppd_model_number = -1;
            }
        }

        if (!attr)
            break;
    }
    free(ppd_natural_language);
    free(ppd_device_id);
    free(ppd_make);
    free(ppd_make_and_model);
    free(ppd_product);
    free(ppd_psversion);
    free(ppd_type);
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
