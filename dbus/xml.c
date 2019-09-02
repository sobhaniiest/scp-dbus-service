#include "xml.h"

char *packagehint;

static char *add_ppd_name(char *pattern);

static bool DriverType_match(dtype *drivertype,
                             char *ppdname,
                             ppds_attr *attr,
                             char *fit_status);

static dtype *DriverTypes_match(GPtrArray *drivertypes,
                                char *ppdname,
                                ppds_attr *attr,
                                char *fit_status);



static char *add_ppd_name(char *pattern)
{
    packagehint = (char *)malloc(sizeof(char) * strlen(pattern) + 22);
    //fprintf(stderr, "%s\n", pattern);

    int reti, f;
    regex_t regex;
    reti = regcomp(&regex, (char *)pattern, REG_ICASE);
    char *scheme = NULL,
         *rest = NULL, 
         *drv = NULL;
    scheme = strtok(pattern, ":");
    rest = strtok(NULL, ":");

    //fprintf(stderr, "%s %s\n", scheme, rest);

    if(scheme)
    {
        if(!strcmp(scheme, "drv") && rest)
        {
            if(startswith("///", rest))
            {
                drv = (char *)malloc(sizeof(char) * (strlen(rest) - 2));
                slice(rest, drv, 3);
                f = rfind(drv, "/");
                if(f != -1)
                {
                    drv[f+1] = '\0';
                    strcpy(packagehint, "/usr/share/cups/drv/");
                    strcat(packagehint, drv);
                    //snprintf(packagehint, 1024, "/usr/share/cups/drv/%s", drv); 
                }
            }
        }
        else
        {
            strcpy(packagehint, "/usr/lib/cups/driver/");
            strcat(packagehint, scheme);
            //snprintf(packagehint, 1024, "/usr/lib/cups/driver/%s", scheme); 
        }
        return packagehint;
    }
    else
        return NULL;
}
/*
static bool DriverType_match(dtype *drivertype,
                             char *ppdname,
                             ppds_attr *attr,
                             char *fit_status)
{
    /*
        Return True if there is a match for all specified criteria.

        ppdname: string

        attr: dict

        fit: string
    */
/*
    bool matches, any_value_matches;
    char *values;

    if(g_hash_table_contains(drivertype->fit, (char *)fit_status))
        matches = true;
    else
        matches = false;

    if (matches && drivertype->name != NULL )///
        matches = false;

    if (matches)
    {
        GHashTableIter iter;
        gpointer name, match;
        g_hash_table_iter_init(&iter, drivertype->attributes);
        while (g_hash_table_iter_next(&iter, &name, &match))
        {
            if(!(strcmp((char *)name, "ppd-make-and-model")) && strlen(attr->ppd_make_and_model) == 0)
            {
                matches = false;
                break;
            }
            else if (!(strcmp((char *)name, "ppd-product")) && strlen(attr->ppd_product) == 0)
            {
                matches = false;
                break;
            }

            values = attr->ppd_make_and_model;
            any_value_matches = false;
        }
    }

    if (matches)
    {
        if((drivertype->deviceid)->len != 0 && strlen(attr->ppd_device_id) == 0)
            matches = false;
        else if ((drivertype->deviceid)->len != 0)
        {

        }
    }
    return matches;
}

static dtype *DriverTypes_match(GPtrArray *drivertypes,
                                char *ppdname,
                                ppds_attr *attr,
                                char *fit_status)
{
    /*
        Return the first matching drivertype for a PPD, given its name,
        attributes, and fitness, or None if there is no match.
    *//*
    for (int i = 0; i < drivertypes->len; i++)
    {
        dtpye *drivertype = (dtype *)g_ptr_array_index ((GPtrArray*)drivertypes, i);
        if (match (drivertype, ppdname, attr, fit_status))
            return drivertype;
    }
    return NULL;
}
*/
GPtrArray *DriverTypes_load(xmlNodePtr drivertypes)
{
    /* Load the list of driver types from an XML file. */
    // GPtrArray *drivertypes = g_ptr_array_new ();
    GPtrArray *types = g_ptr_array_new ();
    char *name;
    int count_type;
    char *pkghint, *buffer;
    char **fittype;
    xmlNodePtr drivertype = drivertypes->xmlChildrenNode;
    xmlNodePtr child, field;
    while (drivertype != NULL) 
    {
        name = xmlGetProp(drivertype, "name");
        //fprintf(stderr, "name : %s\n", name);

        GHashTable *attributes = g_hash_table_new(g_str_hash, g_str_equal);
        GPtrArray *deviceid = g_ptr_array_new ();
        GHashTable *fit = g_hash_table_new(g_str_hash, g_str_equal);

        child = drivertype->xmlChildrenNode;
        while (child != NULL) 
        {
            if ((!xmlStrcmp(child->name, (const xmlChar *)"ppdname")))
            {
                pkghint = add_ppd_name(xmlGetProp(child, "match"));
                //fprintf(stderr, "ppdname : %s\n", pkghint);

            }
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"attribute")))
            {
                g_hash_table_insert(attributes, xmlGetProp(child, "name"), xmlGetProp(child, "match"));
                //fprintf(stderr, "attribute : name : %s  match : %s\n", (char *)xmlGetProp(child, "name"), (char *)xmlGetProp(child, "match"));
            }
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"deviceid")))
            {
                GHashTable *re = g_hash_table_new(g_str_hash, g_str_equal);
                field = child->xmlChildrenNode;
                while (field != NULL)
                {
                    if ((!xmlStrcmp(field->name, (const xmlChar *)"field")))
                    {
                        g_hash_table_insert(re, strupr(xmlGetProp(field, "name")), xmlGetProp(field, "match"));
                        //fprintf(stderr, "field : name : %s  match : %s\n", (char *)xmlGetProp(field, "name"), (char *)xmlGetProp(field, "match"));
                    }
                    field = field->next;
                }
                g_ptr_array_add (deviceid, (gpointer) re);
            }
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"fit")))
            {
                buffer = (char *)malloc(sizeof(char) * strlen(xmlNodeGetContent(child)) + 1);
                strcpy(buffer, xmlNodeGetContent(child));
                count_type = count_tokens(buffer, '\n');
                fittype = split(buffer, "\n", count_type);

                //fprintf(stderr, "fit : ");
                for(int i = 0; i < count_type; i++)
                {
                    //fprintf(stderr, "%s ", strstrip(fittype[i]));
                    g_hash_table_insert(fit, strstrip(fittype[i]), "true");
                    if(!strcmp(strstrip(fittype[i]), FIT_EXACT))
                        g_hash_table_insert(fit, FIT_EXACT_CMD, "true");
                }
                //fprintf(stderr, "\n");
            } 
            child = child->next;
        }

        dtype *t = (dtype *)malloc(sizeof(dtype));
        t->name = name;
        t->pkghint = pkghint;
        t->attributes = attributes;
        t->deviceid = deviceid;
        t->fit = fit;
        g_ptr_array_add (types, (gpointer) t);
        drivertype = drivertype->next;
    }

    return types;
}
/*
GPtrArray *get_ordered_ppdnames(GPtrArray *drivertypes,
                                GHashTable *ppds_dict,
                                GHashTable *fit)
{
    /*
        Given a list of driver type names, a dict of PPD attributes by
        PPD name, and a dict of driver fitness status codes by PPD
        name, return a list of tuples in the form (driver-type-name,
        PPD-name), representing PPDs that match the list of driver
        types.

        The returned tuples will have driver types in the same order
        as the driver types given, with the exception that any
        blacklisted driver types will be omitted from the returned
        result.
    */
/*
    GPtrArray *ppdnames = g_ptr_array_new ();
    dtype *drivertype;

    /* First find out what driver types we have */
/*
    GHashTable *ppdtypes = g_hash_table_new(g_str_hash, g_str_equal);
    char *fit_default = FIT_CLOSE;

    GHashTableIter iter;
    gpointer ppdname, ppddict;
    g_hash_table_iter_init(&iter, ppds_dict);

    while (g_hash_table_iter_next(&iter, &ppdname, &ppddict))
    {
        if(g_hash_table_contains(fit, (char *)ppdname))
        {
            drivertype = match(drivertypes,
                               (char *)ppdname, 
                               (ppds_attr *)ppddict, 
                               (char *)g_hash_table_lookup(fit, (char *)ppdname));
        }
        else
        {
            drivertype = match(drivertypes,
                               (char *)ppdname, 
                               (ppds_attr *)ppddict, 
                               (char *)fit_default);
        }

        if(drivertype)
        {

        }
        else
        {

        }
    }
}
*/
/* A policy for choosing the preference order for drivers. */
/* Load the policy from an XML file. */

GPtrArray *PreferenceOrder_load(xmlNodePtr preferreddrivers)
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
                //fprintf(stderr, "make-and-model : %s\n", (char *)(xmlGetProp(child, "match")));
            }
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"deviceid")))
            {
                GHashTable *re = g_hash_table_new(g_str_hash, g_str_equal);
                field = child->xmlChildrenNode;
                while (field != NULL)
                {
                    if ((!xmlStrcmp(field->name, (const xmlChar *)"field")))
                    {
                        g_hash_table_insert(re, strupr(xmlGetProp(field, "name")), xmlGetProp(field, "match"));
                        //fprintf(stderr, "field : name : %s  match : %s\n", (char *)xmlGetProp(field, "name"), (char *)xmlGetProp(field, "match"));
                    }
                    field = field->next;
                }
                g_ptr_array_add (deviceid, (gpointer) re);
            }
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"drivers")))
            {
                //fprintf(stderr, "drivers :\n");
                drivertype = child->xmlChildrenNode;
                while (drivertype != NULL)
                {
                    if ((!xmlStrcmp(drivertype->name, (const xmlChar *)"drivertype")))
                    {
                        g_ptr_array_add (drivertype_patterns, (gpointer) (xmlNodeGetContent(drivertype)));
                        //fprintf(stderr, "%s\n", strstrip((xmlNodeGetContent(drivertype))));
                    }
                    drivertype = drivertype->next;
                }
            }
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"avoid")))
            {
                //fprintf(stderr, "avoid :\n");
                drivertype = child->xmlChildrenNode;
                while (drivertype != NULL)
                {
                    if ((!xmlStrcmp(drivertype->name, (const xmlChar *)"drivertype")))
                    {
                        g_ptr_array_add (avoid, (gpointer) (xmlNodeGetContent(drivertype)));
                        //fprintf(stderr, "%s\n", strstrip((xmlNodeGetContent(drivertype))));
                    }
                    drivertype = drivertype->next;
                }
            }
            else if ((!xmlStrcmp(child->name, (const xmlChar *)"blacklist")))
            {
                //fprintf(stderr, "blacklist :\n");
                drivertype = child->xmlChildrenNode;
                while (drivertype != NULL)
                {
                    if ((!xmlStrcmp(drivertype->name, (const xmlChar *)"drivertype")))
                    {
                        g_ptr_array_add (blacklist, (gpointer) (xmlNodeGetContent(drivertype)));
                        //fprintf(stderr, "%s\n", strstrip((xmlNodeGetContent(drivertype))));
                    }
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

    return ptypes;
}

/*
GPtrArray *get_ordered_types(GPtrArray *drivertypes,
                             GPtrArray *pordertypes,
                             char *make_and_model,
                             GHashTable *deviceid) //id_dict
{
    /*
        Return an accumulated list of driver types from all printer
        types that match a given printer's device-make-and-model and
        IEEE 1284 Device ID.

        The deviceid parameter must be None or a dict indexed by
        short-form upper-case field keys.
    */
/*
    if(deviceid == NULL)
        GHashTable *deviceid = g_hash_table_new(g_str_hash, g_str_equal);

    if(make_and_model == NULL)
        make_and_model = "";

    GPtrArray *orderedtypes = g_ptr_array_new();
    GPtrArray *blacklist = g_ptr_array_new();
    GPtrArray *avoidtype = g_ptr_array_new();

    for (int i = 0; i < pordertypes->len; i++)
    {
        ptype *data = (ptype *)g_ptr_array_index ((GPtrArray*)pordertypes, i);

    }
}
*/

GPtrArray *PreferredDrivers (char *docname)
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
int main(int argc, char **argv) 
{
    char *docname;

    docname = "test.xml";

    /*
    GPtrArray *arr = PreferredDrivers (docname);

    xmlNodePtr cur = (xmlNodePtr)g_ptr_array_index ((GPtrArray*)arr, 1);

    if(!xmlStrcmp(cur->name, (const xmlChar *)"drivertypes"))
        fprintf(stderr, "Okay\n" );

    cur = (xmlNodePtr)g_ptr_array_index ((GPtrArray*)arr, 2);

    if(!xmlStrcmp(cur->name, (const xmlChar *)"preferenceorder"))
        fprintf(stderr, "Okay\n" );
    */
    /*
    GPtrArray *dtypes = NULL;
    GPtrArray *ptypes = NULL;

    GPtrArray *child = PreferredDrivers (docname);

    if (!child)
       fprintf(stderr, "Error loading %s\n", docname); 
    else
    {
        xmlNodePtr drivertypes = (xmlNodePtr)g_ptr_array_index ((GPtrArray*)child, 1);
        dtypes = DriverTypes_load(drivertypes);

        xmlNodePtr preferenceorder = (xmlNodePtr)g_ptr_array_index ((GPtrArray*)child, 2);  
        ptypes = PreferenceOrder_load(preferenceorder);
    }

    return (1);
}
*/