#include "xml.h"

/*
void
parseStory (xmlDocPtr doc, xmlNodePtr cur) {

    xmlChar *key;
    cur = cur->xmlChildrenNode;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"keyword"))) {
            key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
            printf("keyword: %s\n", key);
            xmlFree(key);
        }
    cur = cur->next;
    }
    return;
}
*/

char packagehint[1024];

void add_field(GHashTable *re, char *field, char *pattern)
{
     g_hash_table_insert(re, strupr(field), pattern);
}

void add_ppd_name(char *pattern)
{
    if(packagehint)
        return;

    int reti, f;
    regex_t regex;
    reti = regcomp(&regex, (char *)pattern, REG_EXTENDED);
    char *scheme = NULL,
         *rest = NULL;
    scheme = strtok(pattern, ":");
    rest = strtok(NULL, ":");

    if(!scheme)
    {
        if(!strcmp(scheme, "drv"))
        {
            if(startswith("///", rest))
            {
                slice(rest, drv, 3);
                f = rfind(drv, "/");
                if(f != -1)
                {
                    drv[f+1] = '\0';
                    snprintf(packagehint, 1024, "/usr/share/cups/drv/%s", drv); 
                }

            }
        }
        else
            snprintf(packagehint, 1024, "/usr/lib/cups/driver/%s", scheme); 
    }

}

void add_fit(char *text)
{
    GHashTable *fit = g_hash_table_new(g_str_hash, g_str_equal);

}

void DriverTypes_load(xmlNodePtr drivertypes)
{
    /* Load the list of driver types from an XML file. */
    GPtrArray *types = g_ptr_array_new ();
    xmlNodePtr drivertype = drivertypes->xmlChildrenNode;
    xmlNodePtr child, field;
    while (drivertype != NULL) 
    {
        char *name = xmlGetProp(drivertype, "name");
        GPtrArray *attributes = g_ptr_array_new ();
        GPtrArray *deviceid = g_ptr_array_new ();

        child = drivertype->xmlChildrenNode;
        while (child != NULL) 
        {
            if ((!xmlStrcmp(child->name, (const xmlChar *)"ppdname")))
                add_ppd_name(xmlGetProp(child, "match"));
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"attribute")))
                add_attribute(xmlGetProp(child, "name"), xmlGetProp(child, "match"));
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"deviceid")))
            {
                GHashTable *re = g_hash_table_new(g_str_hash, g_str_equal);
                field = child->xmlChildrenNode;
                while (field != NULL)
                {
                    if ((!xmlStrcmp(filed->name, (const xmlChar *)"filed")))
                        add_field(re, xmlGetProp(field, "name"), xmlGetProp(field, "match"));
                    field = field->next;
                }
                add_deviceid_match (deviceid_match);
            }
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"fit")))
                add_fit(xmlNodeGetContent(child));  
            child = child->next;
        }

        g_ptr_array_add (types, (gpointer) t);

        drivertype = drivertype->next;
    }
}



PrinterType()
{

}

void PreferenceOrder_load(xmlNodePtr preferreddrivers)
{
    /* A policy for choosing the preference order for drivers. */
    GPtrArray *ptypes = g_ptr_array_new ();
    /* Load the policy from an XML file. */
    xmlNodePtr printer = preferreddrivers->xmlChildrenNode;
    xmlNodePtr child, field, drivertype;
    while (printer != NULL) 
    {
        // ptype = PrinterType();
        char *make_and_model = NULL;
        GPtrArray *deviceid = g_ptr_array_new ();
        GPtrArray *drivertype_patterns = g_ptr_array_new ();



        child = printer->xmlChildrenNode;
        while (child != NULL) 
        {
            if ((!xmlStrcmp(child->name, (const xmlChar *)"make-and-model")))
                add_make_and_model(xmlGetProp(child, "match"));
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"deviceid")))
            {
                deviceid_match = DeviceIDMatch ();
                field = child->xmlChildrenNode;
                while (field != NULL)
                {
                    if ((!xmlStrcmp(filed->name, (const xmlChar *)"filed")))
                        add_field(xmlGetProp(field, "name"), xmlGetProp(field, "match"));
                    field = field->next;
                }
                add_deviceid_match (deviceid_match);
            }
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"drivers")))
            {
                drivertype = child->xmlChildrenNode;
                while (drivertype != NULL)
                {
                    add_drivertype_pattern(xmlNodeGetContent(drivertype))
                    drivertype = drivertype->next;
                }
            }
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"avoid")))
            {
                drivertype = child->xmlChildrenNode;
                while (drivertype != NULL)
                {
                    add_avoidtype_pattern(xmlNodeGetContent(drivertype))
                    drivertype = drivertype->next;
                }
            }
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"blacklist")))
            {
                drivertype = child->xmlChildrenNode;
                while (drivertype != NULL)
                {
                    add_blacklisted(xmlNodeGetContent(drivertype))
                    drivertype = drivertype->next;
                }
            }  
            child = child->next;
        }
        g_ptr_array_add (ptypes, (gpointer) ptype);
        printer = printer->next;
    }
}


GPtrArray *PreferredDrivers(char *docname)
{
    GPtrArray *array = g_ptr_array_new ();
    xmlDocPtr doc;
    xmlNodePtr cur;

    doc = xmlParseFile(docname);
    
    if (doc == NULL ) {
        fprintf(stderr,"Document not parsed successfully. \n");
        return NULL;
    }
    g_ptr_array_add (array, (gpointer) doc);
    cur = xmlDocGetRootElement(doc);
    
    if (cur == NULL) {
        fprintf(stderr,"empty document\n");
        xmlFreeDoc(doc);
        return NULL;
    }
    
    if (xmlStrcmp(cur->name, (const xmlChar *) "preferreddrivers")) {
        fprintf(stderr,"document of the wrong type, root node != preferreddrivers");
        xmlFreeDoc(doc);
        return NULL;
    }
    
    cur = cur->xmlChildrenNode;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"drivertypes"))){
            g_ptr_array_add (array, (gpointer) ((xmlNodePtr)cur));
        }
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"preferenceorder"))){
            g_ptr_array_add (array, (gpointer) ((xmlNodePtr)cur));
        }
         
    cur = cur->next;
    }
    //xmlFreeDoc(doc);
    return array;

}

/*
int
main(int argc, char **argv) {

    char *docname;

    docname = "test.xml";
    GPtrArray *arr = PreferredDrivers (docname);

    xmlNodePtr cur = (xmlNodePtr)g_ptr_array_index ((GPtrArray*)arr, 1);

    if(!xmlStrcmp(cur->name, (const xmlChar *)"drivertypes"))
        fprintf(stderr, "Okay\n" );

    cur = (xmlNodePtr)g_ptr_array_index ((GPtrArray*)arr, 2);

    if(!xmlStrcmp(cur->name, (const xmlChar *)"preferenceorder"))
        fprintf(stderr, "Okay\n" );


    return (1);
}
*/

