#include "ppds.h"


void PPDs(printer_uri *ppds, char *language, const char *xml_dir)
{
    if(!xml_dir)
    {
        xml_dir = getenv("CUPSHELPERS_XMLDIR");
        if(!xml_dir)
            xml_dir = "/etc/cupshelpers/"
    }

    char *xmlfile = (char *)malloc(strlen(xml_dir)+20);
    strcpy(xmlfile, xml_dir);
    strcat(xmlfile, "preferreddrivers.xml");

    /*


    */

    if((!language) || (!strcmp(language, "C")) || (!(strcmp(language, "POSIX"))))
        language = "en_US";

    char *short_language = (char *)malloc(strlen(language));
    
    if(strtsr(language, "_"))
    {
        int i = 0;
        while(language[i] != '_')
        {
            short_language[i] = language[i];
            i++;
        }
    }
    else
        short_language = language;

    



}