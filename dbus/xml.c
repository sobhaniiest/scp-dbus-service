#include "xml.h"

char packagehint[1024];

static char *add_ppd_name(char *pattern);

static char *add_ppd_name(char *pattern)
{
    if(packagehint)
        return NULL;

    int reti, f;
    regex_t regex;
    reti = regcomp(&regex, (char *)pattern, REG_ICASE);
    char *scheme = NULL,
         *rest = NULL, 
         *drv = NULL;
    scheme = strtok(pattern, ":");
    rest = strtok(NULL, ":");

    if(!scheme)
    {
        if(!strcmp(scheme, "drv"))
        {
            if(startswith("///", rest))
            {
                drv = (char *)malloc(strlen(rest) - 3);
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
    return packagehint;
}

void DriverTypes_load(xmlNodePtr drivertypes)
{
    /* Load the list of driver types from an XML file. */
    // GPtrArray *drivertypes = g_ptr_array_new ();
    GPtrArray *types = g_ptr_array_new ();
    char *name;
    int count_type;
    char *phint, *buffer;
    char **fittype;
    xmlNodePtr drivertype = drivertypes->xmlChildrenNode;
    xmlNodePtr child, field;
    while (drivertype != NULL) 
    {
        name = xmlGetProp(drivertype, "name");
        GHashTable *attributes = g_hash_table_new(g_str_hash, g_str_equal);
        GPtrArray *deviceid = g_ptr_array_new ();
        GHashTable *fit = g_hash_table_new(g_str_hash, g_str_equal);

        child = drivertype->xmlChildrenNode;
        while (child != NULL) 
        {
            if ((!xmlStrcmp(child->name, (const xmlChar *)"ppdname")))
                phint = add_ppd_name(xmlGetProp(child, "match"));
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"attribute")))
                g_hash_table_insert(attributes, xmlGetProp(child, "name"), xmlGetProp(child, "match"));
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"deviceid")))
            {
                GHashTable *re = g_hash_table_new(g_str_hash, g_str_equal);
                field = child->xmlChildrenNode;
                while (field != NULL)
                {
                    if ((!xmlStrcmp(field->name, (const xmlChar *)"filed")))
                        g_hash_table_insert(re, strupr(xmlGetProp(field, "name")), xmlGetProp(field, "match"));
                    field = field->next;
                }
                g_ptr_array_add (deviceid, (gpointer) re);
            }
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"fit")))
            {
                buffer = (char *)malloc(strlen(xmlNodeGetContent(child)));
                strcpy(buffer, xmlNodeGetContent(child));
                count_type = count_tokens(buffer, '\n');
                fittype = split(buffer, "\n", count_type);

                for(int i = 0; i < count_type; i++)
                {
                     g_hash_table_insert(fit, strstrip(fittype[i]), "true");
                     if(!strcmp(strstrip(fittype[i]), FIT_EXACT))
                        g_hash_table_insert(fit, FIT_EXACT_CMD, "true");
                }
            } 
            child = child->next;
        }

        dtype *t = (dtype *)malloc(sizeof(dtype));
        t->name = name;
        t->phint = phint;
        t->attributes = attributes;
        t->deviceid = deviceid;
        t->fit = fit;
        g_ptr_array_add (types, (gpointer) t);
        drivertype = drivertype->next;
    }
}

/* A policy for choosing the preference order for drivers. */
/* Load the policy from an XML file. */

void PreferenceOrder_load(xmlNodePtr preferreddrivers)
{
    int reti;
    regex_t regex;
    GPtrArray *ptypes = g_ptr_array_new ();
    xmlNodePtr printer = preferreddrivers->xmlChildrenNode;
    xmlNodePtr child, field, drivertype;
    while (printer != NULL) 
    {
        /* 
            A make-and-model pattern and/or set of IEEE 1284 Device ID
            patterns for matching a set of printers, together with an ordered
            list of driver type names.
        */
        GPtrArray *make_and_model = g_ptr_array_new ();
        GPtrArray *deviceid = g_ptr_array_new ();
        GPtrArray *drivertype_patterns = g_ptr_array_new ();
        GPtrArray *avoid = g_ptr_array_new ();
        GPtrArray *blacklist = g_ptr_array_new ();

        child = printer->xmlChildrenNode;
        while (child != NULL) 
        {
            if ((!xmlStrcmp(child->name, (const xmlChar *)"make-and-model")))
            {
                reti = regcomp(&regex, (char *)(xmlGetProp(child, "match")), REG_ICASE);
                g_ptr_array_add (make_and_model, (gpointer) (xmlGetProp(child, "match")));
                // add_make_and_model(xmlGetProp(child, "match"));
            }
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"deviceid")))
            {
                GHashTable *re = g_hash_table_new(g_str_hash, g_str_equal);
                field = child->xmlChildrenNode;
                while (field != NULL)
                {
                    if ((!xmlStrcmp(field->name, (const xmlChar *)"filed")))
                        g_hash_table_insert(re, strupr(xmlGetProp(field, "name")), xmlGetProp(field, "match"));
                    field = field->next;
                }
                g_ptr_array_add (deviceid, (gpointer) re);
            }
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"drivers")))
            {
                drivertype = child->xmlChildrenNode;
                while (drivertype != NULL)
                {
                    g_ptr_array_add (drivertype_patterns, (gpointer) (xmlNodeGetContent(drivertype)));
                    drivertype = drivertype->next;
                }
            }
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"avoid")))
            {
                drivertype = child->xmlChildrenNode;
                while (drivertype != NULL)
                {
                    g_ptr_array_add (avoid, (gpointer) (xmlNodeGetContent(drivertype)));
                    drivertype = drivertype->next;
                }
            }
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"blacklist")))
            {
                drivertype = child->xmlChildrenNode;
                while (drivertype != NULL)
                {
                    g_ptr_array_add (blacklist, (gpointer) (xmlNodeGetContent(drivertype)));
                    drivertype = drivertype->next;
                }
            }  
            child = child->next;
        }
        ptype *pt = (ptype *)malloc(sizeof(ptype));
        pt->make_and_model = make_and_model;
        pt->deviceid = deviceid;
        pt->drivertype_patterns = drivertype_patterns;
        pt->avoid = avoid;
        pt->blacklist = blacklist;
        g_ptr_array_add (ptypes, (gpointer) pt);
        printer = printer->next;
    }
}

GPtrArray *PreferredDrivers(char *docname)
{
    GPtrArray *array = g_ptr_array_new ();
    xmlDocPtr doc;
    xmlNodePtr cur;

    doc = xmlParseFile(docname);
    
    if (doc == NULL ) 
    {
        fprintf(stderr,"Document not parsed successfully. \n");
        return NULL;
    }
    g_ptr_array_add (array, (gpointer) doc);
    cur = xmlDocGetRootElement(doc);
    
    if (cur == NULL) 
    {
        fprintf(stderr,"empty document\n");
        xmlFreeDoc(doc);
        return NULL;
    }
    
    if (xmlStrcmp(cur->name, (const xmlChar *) "preferreddrivers")) 
    {
        fprintf(stderr,"document of the wrong type, root node != preferreddrivers");
        xmlFreeDoc(doc);
        return NULL;
    }
    
    cur = cur->xmlChildrenNode;
    while (cur != NULL) 
    {
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"drivertypes")))
            g_ptr_array_add (array, (gpointer) ((xmlNodePtr)cur));
        
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"preferenceorder")))
            g_ptr_array_add (array, (gpointer) ((xmlNodePtr)cur));
         
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

