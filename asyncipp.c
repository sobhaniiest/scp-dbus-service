#include "asyncipp.h"
#include "authinfocache.h"

void insert_uri(printer_uri **head, const char *name, const char *uri)
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

void set_ipp_error (ipp_status_t status, const char *message)
{
    if (!message)
        message = ippErrorString (status);
    fprintf(stderr, "set_ipp_error: %d, %s\n",(int)status, message);
}


printer_uri *getURI(http_t *new)
{
    printer_uri *puri = NULL;
	ipp_t *request = ippNewRequest(CUPS_GET_PRINTERS), *answer;
  	ipp_attribute_t *attr;
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
      	const char *printer = NULL;
      	const char *duri = NULL;
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
    	      		duri = ippGetString(attr, 0, NULL);
    	      		//fprintf(stderr, "Device URI : %s\n",duri);
  	      	}

            if(printer != NULL && duri != NULL)
            {
                insert_uri(&puri, printer, duri);
                printer = NULL;
                duri = NULL;
            }
    	  }

      	if(!attr)
        		break;
   	} 
   	//ippDelete (answer);
    return puri;
}

printer_uri *getPPDs(http_t *new, int all_lists)
{
    printer_uri *list = NULL;
    ipp_t *request, *answer;
    ipp_attribute_t *attr;

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
        const char *ppdname = NULL;

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
                insert_uri(&list, ppdname, "\0");
                //fprintf(stderr, "PPD_Name : %s\n",ppdname);
            }
        }

        if (!attr)
            break;
    }

    //ippDelete (answer);
    return list;
}

printer_uri *IPPAuthConnection(void(*reply_handler)(), 
                  					   void(*error_handler)(), 
                  					   void(*auth_handler)(), 
                  					   const char *host, 
                  					   int port, 
                  					   http_encryption_t encryption, 
                  					   bool try_as_root, 
                  					   bool prompt_allowed,
                               char *result)
{
  	const char *user = "\0";
  	dict *creds = lookup_auth_info(host, port);
  	if(creds != NULL)
  	{
    		if(strcmp(creds->hname, "root") || try_as_root)
    			user = creds->hname;
    		free(creds);
  	}

  	printer_uri *new = IPPConnection(reply_handler, 
                            				 error_handler, 
                            				 auth_handler,
                            				 user, 
                            				 host, 
                            				 port,
                            				 encryption,
                                     result);

    if(new == NULL)
      op_error_handler();
    else if(reply_handler != NULL)
      reply_handler();

    return new;
}

printer_uri *IPPConnection(void(*reply_handler)(), 
                				   void(*error_handler)(), 
                				   void(*auth_handler)(),
                				   const char *user, 
                				   const char *host, 
                				   int port, 
                				   http_encryption_t encryption,
                           char *result)
{
    http_t *http = httpConnectEncrypt (host, port, (http_encryption_t) encryption);

    if(http == NULL)
    {
        fprintf(stderr, "Connection Error!!!\n");
        return NULL;
    }

    printer_uri *new;

    if(!(strcmp(result, "getURI")))
        new = getURI(http);
    else if(!(strcmp(result, "getPPDs")))
        new = getPPDs(http, 1);

    httpClose(http);
    
    return new;
}

void op_error_handler()
{

}