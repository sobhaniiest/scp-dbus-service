#include "asyncipp.h"
#include "authinfocache.h"

printer_uri *puri = NULL;

void insert_uri(printer_uri **head, char *n, char *u)
{
    printer_uri *c = (*head);
  
    if(*head == NULL)
    {
        (*head) = (printer_uri *)malloc(sizeof(printer_uri));
        (*head)->name = n;
        (*head)->uri = u;
        (*head)->next = NULL;
    }
    else
    {
        while(c->next != NULL)
            c = c->next;
        c->next = (printer_uri *)malloc(sizeof(printer_uri));
        c->next->name = n;
        c->next->uri = u;
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
            exit(0);
        }

        set_ipp_error (answer ? ippGetStatusCode (answer) : cupsLastError (),
                       answer ? NULL : cupsLastErrorString ());
        if (answer)
            ippDelete (answer);
        fprintf(stderr, "Error\n");
        exit(0);
    }

    for(ippFirstAttribute (answer); attr; attr = ippNextAttribute (answer))
    {
    	char *printer = NULL;
    	char *duri = NULL;
    	while (attr && ippGetGroupTag (attr) != IPP_TAG_PRINTER)
      		attr = ippNextAttribute (answer);

    	if (!attr)
      		break;

      	for (; attr && ippGetGroupTag (attr) == IPP_TAG_PRINTER; attr = ippNextAttribute (answer)) 
      	{
	    	if (!strcmp (ippGetName (attr), "printer-name") && ippGetValueTag (attr) == IPP_TAG_NAME)
	        {
	        	printer = (char *) ippGetString (attr, 0, NULL);
	        	fprintf(stderr, "Printer Name : %s\n",printer);
	        }
	      	else if ((!strcmp (ippGetName (attr), "device-uri")) && ippGetValueTag (attr) == IPP_TAG_URI) 
	      	{
	      		duri = ippGetString(attr, 0, NULL);
	      		fprintf(stderr, "Device URI : %s\n",duri);
	      	}

	      	insert_uri(&puri, printer, duri);
    	}

    	if (!attr)
      		break;
   	} 
   	ippDelete (answer);
   	return puri;
}

void IPPAuthConnection(void(*reply_handler)(), 
					   void(*error_handler)(), 
					   void(*auth_handler)(), 
					   char *host, 
					   int port, 
					   http_encryption_t encryption, 
					   bool try_as_root, 
					   bool prompt_allowed)
{
	char *user = "\0";
	dict *creds = NULL;
	//lookup_auth_info(host, port);
	if(creds != NULL)
	{
		if(strcmp(creds->hname, "root") || try_as_root)
			user = creds->hname;
		free(creds);
	}

	//IPPAuthOperation();

	IPPConnection(reply_handler, 
				  error_handler, 
				  auth_handler,
				  user, 
				  host, 
				  port,
				  encryption);
}

void IPPConnection(void(*reply_handler)(), 
				   void(*error_handler)(), 
				   void(*auth_handler)(),
				   char *user, 
				   char *host, 
				   int port, 
				   http_encryption_t encryption)
{
	char *uri;
    fprintf(stderr, "host: %s\nport: %d\nencryption: %d\n",host,port,encryption);
  
    http_t *http = httpConnectEncrypt (host, port, (http_encryption_t) encryption);

    if(http == NULL)
    {
        fprintf(stderr, "Connection Error!!!\n");
        exit(0);
    }

    getURI(http);
}