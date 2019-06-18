#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cups/cups.h>
#include <cups/http.h>
#include <pthread.h>


void set_ipp_error (ipp_status_t status, const char *message)
{
    if (!message)
        message = ippErrorString (status);
    fprintf(stderr, "set_ipp_error: %d, %s\n",(int)status, message);
}

int test(http_t *n)
{
    ipp_t *request = ippNewRequest(CUPS_GET_PRINTERS);
    ipp_attribute_t *attr;
    const char *attributes[] = {
        "printer-name",
        "printer-type",
        "printer-location",
        "printer-info",
        "printer-make-and-model",
        "printer-state",
        "printer-state-message",
        "printer-state-reasons",
        "printer-uri-supported",
        "device-uri",
        "printer-is-shared",
    };
    ippAddStrings (request, 
                   IPP_TAG_OPERATION, 
                   IPP_TAG_KEYWORD,
                   "requested-attributes",
                   sizeof (attributes) / sizeof (attributes[0]),
                   NULL, 
                   attributes);
    ipp_t *answer = cupsDoRequest (n, request, "/");

    if (!answer || ippGetStatusCode (answer) > IPP_OK_CONFLICT) 
    {
        //fprintf(stderr, "#####\n");
        if (answer && ippGetStatusCode (answer) == IPP_NOT_FOUND) 
        {
            // No printers.
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
    //fprintf(stderr, "#####\n");
        

    if ((attr = ippFindAttribute(answer, "device-uri", IPP_TAG_TEXT)) != NULL)
    {
        printf("device-uri = \"%s\"\n", ippGetString(attr, 0, NULL));
    }
    else
        fprintf(stderr, "Unknown\n" );

    ippDelete (answer);
    return 1;
}


int main()
{
    int e;
    const char *host = cupsServer();
    int port = ippPort();
    int encryption = (http_encryption_t)cupsEncryption();

    fprintf(stderr, "host: %s\nport: %d\nencryption: %d\n",host,port,encryption);
  
    http_t *http = httpConnectEncrypt (host, port, (http_encryption_t) encryption);

    if(http == NULL)
    {
        fprintf(stderr, "Connection Error!!!\n");
        exit(0);
    }

    test(http);
}