#include "asyncipp.h"

static void op_error_handler();
static void insert_ppd(ppds_attr **head, 
                       const char *ppdname,  
                       char *ppd_natural_language, 
                       char *ppd_device_id, 
                       char *ppd_make, 
                       char *ppd_make_and_model, 
                       int ppd_model_number,
                       char *ppd_product,
                       char *ppd_psversion, 
                       char *ppd_type);
static void insert_uri(printer_uri **head, const char *name, const char *uri);
static void set_ipp_error (ipp_status_t status, const char *message);

static void op_error_handler()
{

}

static void insert_ppd(ppds_attr **head, 
                       const char *ppdname,  
                       char *ppd_natural_language, 
                       char *ppd_device_id, 
                       char *ppd_make, 
                       char *ppd_make_and_model, 
                       int ppd_model_number,
                       char *ppd_product,
                       char *ppd_psversion, 
                       char *ppd_type)
{
    ppds_attr *c = (*head);
  
    if(*head == NULL)
    {
        (*head) = (ppds_attr *)malloc(sizeof(ppds_attr));
        (*head)->ppdname = ppdname;
        (*head)->ppd_natural_language = ppd_natural_language;
        (*head)->ppd_device_id = ppd_device_id;
        (*head)->ppd_make = ppd_make;
        (*head)->ppd_make_and_model = ppd_make_and_model;
        (*head)->ppd_model_number = ppd_model_number;
        (*head)->ppd_product = ppd_product;
        (*head)->ppd_psversion = ppd_psversion;
        (*head)->ppd_type = ppd_type;
        (*head)->next = NULL;
    }
    else
    {
        while(c->next != NULL)
            c = c->next;
        c->next = (ppds_attr *)malloc(sizeof(ppds_attr));
        c->next->ppdname = ppdname;
        c->next->ppd_natural_language = ppd_natural_language;
        c->next->ppd_device_id = ppd_device_id;
        c->next->ppd_make = ppd_make;
        c->next->ppd_make_and_model = ppd_make_and_model;
        c->next->ppd_model_number = ppd_model_number;
        c->next->ppd_product = ppd_product;
        c->next->ppd_psversion = ppd_psversion;
        c->next->ppd_type = ppd_type;
        c->next->next = NULL;
    }
}

static void insert_uri(printer_uri **head, const char *name, const char *uri)
{
    printer_uri *c = (*head);
  
    if(*head == NULL)
    {
        (*head) = (printer_uri *)malloc(sizeof(printer_uri));
        (*head)->name = name;
        (*head)->uri = uri;
        (*head)->next = NULL;
    }
    else
    {
        while(c->next != NULL)
            c = c->next;
        c->next = (printer_uri *)malloc(sizeof(printer_uri));
        c->next->name = name;
        c->next->uri = uri;
        c->next->next = NULL;
    }
}

static void set_ipp_error (ipp_status_t status, const char *message)
{
    if (!message)
        message = ippErrorString (status);
    fprintf(stderr, "set_ipp_error: %d, %s\n",(int)status, message);
}


printer_uri *getURI(http_t *new)
{
    printer_uri *list = NULL;
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
  	        {
    	        	printer = (char *) ippGetString (attr, 0, NULL);
    	        	//fprintf(stderr, "Printer Name : %s\n",printer);
  	        }
  	      	else if ((!strcmp (ippGetName (attr), "device-uri")) && ippGetValueTag (attr) == IPP_TAG_URI) 
  	      	{
    	      		uri = ippGetString(attr, 0, NULL);
    	      		//fprintf(stderr, "Device URI : %s\n",uri);
  	      	}

            if(printer != NULL && uri != NULL)
            {
                insert_uri(&list, printer, uri);
                printer = NULL;
                uri = NULL;
            }
    	  }

      	if(!attr)
        		break;
   	} 
   	//ippDelete (answer);
    return list;
}

ppds_attr *getPPDs(http_t *new, int all_lists)
{
    ppds_attr *list = NULL;

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
            //fprintf(stderr, "Attribute: %s\n", ippGetName (attr));
            if (!strcmp (ippGetName (attr), "ppd-name") && ippGetValueTag (attr) == IPP_TAG_NAME)
            {
                ppdname = (char *) ippGetString (attr, 0, NULL);
                //fprintf(stderr, "PPD_Name : %s\n",ppdname);
            }
            else if (!strcmp (ippGetName (attr), "ppd-natural-language") && ippGetValueTag (attr) == IPP_TAG_LANGUAGE)
            {
                ppd_natural_language = (char *) ippGetString (attr, 0, NULL);
                //fprintf(stderr, "ppd_natural_language : %s\n",ppd_natural_language);
            }
            else if (!strcmp (ippGetName (attr), "ppd-device-id") && ippGetValueTag (attr) == IPP_TAG_TEXT)
            {
                ppd_device_id = (char *) ippGetString (attr, 0, NULL);
                //fprintf(stderr, "ppd_device_id : %s\n",ppd_device_id);
            }
            else if (!strcmp (ippGetName (attr), "ppd-make") && ippGetValueTag (attr) == IPP_TAG_TEXT)
            {
                ppd_make = (char *) ippGetString (attr, 0, NULL);
                //fprintf(stderr, "ppd_make : %s\n",ppd_make);
            }
            else if (!strcmp (ippGetName (attr), "ppd-make-and-model") && ippGetValueTag (attr) == IPP_TAG_TEXT)
            {
                ppd_make_and_model = (char *) ippGetString (attr, 0, NULL);
                //fprintf(stderr, "ppd_make_and_model : %s\n",ppd_make_and_model);
            }
            else if (!strcmp (ippGetName (attr), "ppd-product") && ippGetValueTag (attr) == IPP_TAG_TEXT)
            {
                ppd_product = (char *) ippGetString (attr, 0, NULL);
                //fprintf(stderr, "ppd_product : %s\n",ppd_product);
            }
            else if (!strcmp (ippGetName (attr), "ppd-psversion") && ippGetValueTag (attr) == IPP_TAG_TEXT)
            {
                ppd_psversion = (char *) ippGetString (attr, 0, NULL);
                //fprintf(stderr, "ppd_psversion : %s\n",ppd_psversion);
            }
            else if (!strcmp (ippGetName (attr), "ppd-type") && ippGetValueTag (attr) == IPP_TAG_KEYWORD)
            {
                ppd_type = (char *) ippGetString (attr, 0, NULL);
                //fprintf(stderr, "ppd_type : %s\n", ppd_type);
            }
            else if (!strcmp (ippGetName (attr), "ppd-model-number") && ippGetValueTag (attr) == IPP_TAG_INTEGER)
            {
                ppd_model_number = ippGetInteger (attr, 0);
                //fprintf(stderr, "ppd_model_number : %d\n",ppd_model_number);
            }

            if(ppdname != NULL && ppd_natural_language != NULL && ppd_device_id != NULL 
              && ppd_make != NULL && ppd_make_and_model != NULL && ppd_product != NULL 
              && ppd_psversion != NULL && ppd_type != NULL && ppd_model_number != -1)
            {
                insert_ppd(&list, ppdname, ppd_natural_language, ppd_device_id,
                                  ppd_make, ppd_make_and_model, ppd_model_number,
                                  ppd_product, ppd_psversion, ppd_type);
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
    return list;
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
