#include "ppds.h"

GHashTable *ids = NULL;

static void init_ids(GHashTable *ppds);

GHashTable *parseDeviceID(const char *device_id)
{
    /*
        Parse an IEEE 1284 Device ID, so that it may be indexed by field name.

        @param id: IEEE 1284 Device ID, without the two leading length bytes
        @type id: string
        @returns: dict indexed by field name
    */

    GHashTable *id_dict = g_hash_table_new(g_str_hash, g_str_equal);

    int count_id, count_cmd, i;
    char **pieces, **cmds;
    char *name, *value;
    char *device_name[9] = {"MFG", "MDL", "CMD", "CLS", "DES", "SN", "S", "P", "J"};

    char *id = (char *)malloc(strlen(device_id));
    strcpy(id, device_id);

    count_id = count_tokens(id, ';');
    pieces = split(id, ";", count_id);

    for(i=0;i<count_id;i++)
    {
        if(!(strstr(pieces[i], ":")))
            continue;

        name = strtok(pieces[i], ":");
        value = strtok(NULL, ":");
        g_hash_table_insert(id_dict, strstrip(name), strstrip(value));
    }


    if(g_hash_table_contains(id_dict, "MANUFACTURER"))
        g_hash_table_insert(id_dict, "MFG", g_hash_table_lookup(id_dict, "MANUFACTURER"));
    if(g_hash_table_contains(id_dict, "MODEL"))
        g_hash_table_insert(id_dict, "MDL", g_hash_table_lookup(id_dict, "MODEL"));
    if(g_hash_table_contains(id_dict, "COMMAND SET"))
        g_hash_table_insert(id_dict, "CMD", g_hash_table_lookup(id_dict, "COMMAND SET"));

    for(i=0;i<9;i++)
    {
        if(!(g_hash_table_contains(id_dict, device_name[i])))
            g_hash_table_insert(id_dict, device_name[i], "");
    }

    GPtrArray *array = g_ptr_array_new ();

    if(!(strcmp(g_hash_table_lookup(id_dict, "CMD"), "")))
        g_hash_table_insert(id_dict, "CMD", array);
    else
    {
        count_cmd = count_tokens(g_hash_table_lookup(id_dict, "CMD"), ',');
        cmds = split(g_hash_table_lookup(id_dict, "CMD"), ",", count_cmd);
        g_hash_table_insert(id_dict, "CMD", array);
        for(i=0;i<=count_cmd;i++)
            g_ptr_array_add (array, cmds[i]);
    }
    //free(id);
    return id_dict;
}

void PPDs(GHashTable *ppds, char *language, const char *xml_dir)
{
    /*
        @type ppds: struct of type 'ppds_attr' define in asyncipp.h
        @param ppds: dict of PPDs as returned getPPDs() in asyncipp.c

        @type language: string
        @param language: language name, as given by the setlocale()
    */

    //self.ppds = ppds.copy () ***

    if(!xml_dir)
    {
        xml_dir = getenv("CUPSHELPERS_XMLDIR");
        if(!xml_dir)
            xml_dir = "/etc/cupshelpers/";
    }

    char *xmlfile = (char *)malloc(strlen(xml_dir)+strlen("preferreddrivers.xml"));
    strcpy(xmlfile, xml_dir);
    strcat(xmlfile, "preferreddrivers.xml");

    
    /*


    */

    if((!language) || (!(strcmp(language, "C"))) || (!(strcmp(language, "POSIX"))))
        language = "en_US";

    char *short_language = (char *)malloc(strlen(language));
    
    if(strstr(language, "_"))
    {
        int i = 0;
        while(language[i] != '_')
        {
            short_language[i] = language[i];
            i++;
        }
        short_language[i] = '\0';
    }
    else
        short_language = language;
    

    GHashTableIter iter;
    gpointer key, value;

    int i;
    GPtrArray *array = g_ptr_array_new ();
    g_hash_table_iter_init(&iter, ppds);
    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        if((!strcmp(((ppds_attr *)value)->ppd_natural_language, "en")) || 
           (!strcmp(((ppds_attr *)value)->ppd_natural_language, language)) ||
           (!strcmp(((ppds_attr *)value)->ppd_natural_language, short_language)))
        {
            /*
                Some manufacturer's PPDs are only available in this
                language, so always let them though.
            */
            g_ptr_array_add (array, (gpointer)((char *)key));
        }
    }
    for(i=0;i<array->len;i++)
        g_hash_table_remove(ppds, (char *)g_ptr_array_index ((GPtrArray *)array, i));
    g_ptr_array_free(array, false);

    /*
        CUPS sets the 'raw' model's ppd-make-and-model to 'Raw Queue'
        which unfortunately then appears as manufacturer Raw and
        model Queue.  Use 'Generic' for this model.
    */

    char *makemodel, *gmakemodel;
    g_hash_table_iter_init(&iter, ppds);
    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        if((!strcmp((char *)key, "raw")))
        {
            makemodel = ((ppds_attr *)value)->ppd_make_and_model;
            if(!(startswith("Generic ", makemodel)))
            {
                gmakemodel = (char *)malloc(strlen(makemodel)+strlen("Generic "));
                strcpy(gmakemodel, "Generic ");
                strcat(gmakemodel, makemodel);
                ((ppds_attr *)value)->ppd_make_and_model = gmakemodel;
            }
        }
    }
}


char *ppdMakeModelSplit(char *ppd_make_and_model)
{
    /*  
        Split a ppd-make-and-model string into a canonical make and model pair.

        @type ppd_make_and_model: string
        @param ppd_make_and_model: IPP ppd-make-and-model attribute
        @return: a string pair representing the make and the model

        If the string starts with a known model name (like "LaserJet") assume
        that the manufacturer name is missing and add the manufacturer name
        corresponding to the model name
    */

    char *MFR_BY_RANGE[24] = {"HP", "deskjet""|dj[ 0-9]?""|laserjet""|lj""|color laserjet""|color lj""|designjet""|officejet""|oj""|photosmart""|ps ""|psc""|edgeline",
                              "Epson", "stylus|aculaser",
                              "Apple", "stylewriter""|imagewriter""|deskwriter""|laserwriter",
                              "Canon", "pixus""|pixma""|selphy""|imagerunner""|bj""|lbp",
                              "Brother", "hl|dcp|mfc",
                              "Xerox", "docuprint""|docupage""|phaser""|workcentre""|homecentre",
                              "Lexmark", "optra|(:color )?jetprinter",
                              "KONICA MINOLTA", "magicolor""|pageworks""|pagepro",
                              "Kyocera", "fs-""|km-""|taskalfa",
                              "Ricoh", "aficio",
                              "Oce", "varioprint",
                              "Oki", "okipage|microline"};

    char *make_and_model = strstrip(ppd_make_and_model);
    char *make = NULL;
    char *model = NULL;
    bool cleanup_make = false,
         found_mfr = false;
    char *l = (char *)malloc(strlen(make_and_model));
    strcpy(l, make_and_model);
    l = strlwr(l);
    int i, j;
    int reti;
    regex_t regex;
    char msgbuf[100];

    for(i=0;i<12;i++)
    {
        /* Compile regular expression */
        reti = regcomp(&regex, MFR_BY_RANGE[2*i+1], REG_EXTENDED);
        if( reti ) 
            fprintf(stderr, "Could not compile regex\n");
        
        /* Execute regular expression */
        reti = regexec(&regex, l, 0, NULL, 0);
        if( !reti )
        {
            //fprintf(stderr, "Match\n");
            make = MFR_BY_RANGE[2*i];
            model = make_and_model;
            break;
        }
        else if( reti == REG_NOMATCH )
        {
            //fprintf(stderr, "Not match\n");
        }
        else
        {
            regerror(reti, &regex, msgbuf, sizeof(msgbuf));
            fprintf(stderr, "Regex match failed: %s\n", msgbuf);
            exit(1);
        }
        //regfree(&regex);
    }

    /* Handle PPDs provided by Turboprint */
    char *buffer;
    char *RE_turboprint = "turboprint";
    reti = regcomp(&regex, RE_turboprint, REG_EXTENDED);
    reti = regexec(&regex, l, 0, NULL, 0);

    if((!reti) && (!make))
    {
        fprintf(stderr, "Match\n");
        int t1, t2;
        t1 = find(make_and_model, " TurboPrint");
        if(t1 != -1)
        {
            t2 = rfind(make_and_model, " TurboPrint");
            if(t1 != t2)
            {/////////////////////////////////////////
                j = 0;
                buffer = (char *)malloc(t2 - (t1+11));
                for(i=t1+12;i<=t2;i++)
                    buffer[j++] = make_and_model[i];
                strcpy(make_and_model, buffer);
                free(buffer);
            }
            else
            {
                j = 0;
                buffer = (char *)malloc(strlen(make_and_model) - (t1+1));
                for(i=0;i<=t1;i++)
                    buffer[j++] = make_and_model[i];
                strcpy(make_and_model, buffer);
                free(buffer);
            }

            make = strtok(make_and_model, "_");
            model = strtok(NULL, "_");
            if((!make) || (!model))
            {
                make = make_and_model;
                model = "";
            }
        }
        /*
        make = re.sub (r"(?<=[a-z])(?=[0-9])", " ", make)
        make = re.sub (r"(?<=[a-z])(?=[A-Z])", " ", make)
        model = re.sub (r"(?<=[a-z])(?=[0-9])", " ", model)
        model = re.sub (r"(?<=[a-z])(?=[A-Z])", " ", model)
        model = re.sub (r" Jet", "Jet", model)
        model = re.sub (r"Photo Smart", "PhotoSmart", model)
        cleanup_make = True
        */ 
    }
    /* Special handling for two-word manufacturers */
    else if(startswith("konica minolta ", l))
    {
        make = "KONICA MINOLTA";
        buffer = (char *)malloc(strlen(make_and_model)-14);
        slice(make_and_model, buffer, 15);
        strcpy(model, buffer);
        free(buffer);
    }
    else if(startswith("lexmark international ", l))
    {
        make = "Lexmark";
        buffer = (char *)malloc(strlen(make_and_model)-21);
        slice(make_and_model, buffer, 22);
        strcpy(model, buffer);
        free(buffer);
    }
    else if(startswith("kyocera mita ", l))
    {
        make = "Kyocera";
        buffer = (char *)malloc(strlen(make_and_model)-12);
        slice(make_and_model, buffer, 13);
        strcpy(model, buffer);
        free(buffer);
    }
    else if(startswith("kyocera ", l))
    {
        make = "Kyocera";
        buffer = (char *)malloc(strlen(make_and_model)-7);
        slice(make_and_model, buffer, 8);
        strcpy(model, buffer);
        free(buffer);
    }
    else if(startswith("fuji xerox ", l))
    {
        make = "Fuji Xerox";
        buffer = (char *)malloc(strlen(make_and_model)-10);
        slice(make_and_model, buffer, 11);
        strcpy(model, buffer);
        free(buffer);
    }
    else
    {
        /* Finally, take the first word as the name of the manufacturer. */
        cleanup_make = true;
        make = strtok(make_and_model, " ");
        model = strtok(NULL, " ");
        if((!make) || (!model))
        {
            make = make_and_model;
            model = "";
        }
    }

    /* Standardised names for manufacturers. */
    char *makel = strlwr(make);
    if(cleanup_make)
    {
        if(startswith("hewlett", makel) && endswith("packard", makel))
        {
            make = "HP";
            makel = "hp";
        }
        else if(startswith("konica", makel) && endswith("minolta", makel))
        {
            make = "KONICA MINOLTA";
            makel = "konica minolta";
        }
        else
        {
            /* Fix case errors. */
            for(i=0;i<12;i++)
            {
                if(!(strcmp(strlwr(MFR_BY_RANGE[2*i]), makel)))
                {
                    make = MFR_BY_RANGE[2*i];
                    break;
                }
            }

        }
    }

    /*

        # HP and Canon PostScript PPDs give NickNames like:
        # *NickName: "HP LaserJet 4 Plus v2013.111 Postscript (recommended)"
        # *NickName: "Canon MG4100 series Ver.3.90"
        # Find the version number and truncate at that point.  But beware,
        # other model names can legitimately look like version numbers,
        # e.g. Epson PX V500.
        # Truncate only if the version number has only one digit, or a dot
        # with digits before and after.

    */
    /*
    char *modell = strlwr(model);
    if(strstr(modell, " v"))
    {
        /*
        # Look for " v" or " ver." followed by a digit, optionally
        # followed by more digits, a dot, and more digits; and
        # terminated by a space of the end of the line.
        */
    /*
        RE_version_numbers = " v(?:er\\.)?\d(?:\d*\\.\\d+)?(?: |$)"
        reti = regcomp(&regex, RE_turboprint, REG_EXTENDED);
        reti = regexec(&regex, l, 0, NULL, 0);

    }
    
    /*
        # Remove the word "Series" if present.  Some models are referred
        # to as e.g. HP OfficeJet Series 300 (from hpcups, and in the
        # Device IDs of such models), and other groups of models are
        # referred to in drivers as e.g. Epson Stylus Color Series (CUPS).

        (model, n) = _RE_ignore_series.subn ("", model, count=1)
        if n:
            modell = model.lower ()

        if makel == "hp":
            for name, fullname in _HP_MODEL_BY_NAME.items ():
                if modell.startswith (name):
                    model = fullname + model[len (name):]
                    modell = model.lower ()
                    break
    */
    model = strstrip(model);
    char *mfg_mdl = (char *)malloc(strlen(make)+strlen(model)+1);
    strcpy(mfg_mdl, make);
    strcat(mfg_mdl, ":");
    strcat(mfg_mdl, model);
    
    return mfg_mdl;
}


static void init_ids(GHashTable *ppds)
{
    if(ids)
        return;

    ids = g_hash_table_new(g_str_hash, g_str_equal);

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, ppds);
    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        if(!(((ppds_attr *)value)->ppd_device_id))
            continue;
        
        GHashTable *id_dict = parseDeviceID(((ppds_attr *)value)->ppd_device_id);
        
        char *lmfg, *lmdl;
        lmfg = strlwr(g_hash_table_lookup(id_dict, "MFG"));
        lmdl = strlwr(g_hash_table_lookup(id_dict, "MDL"));

        fprintf(stderr, "MFG : %s  MDL : %s\n",lmfg, lmdl);

        bool bad = false;
        if(strlen(lmfg) == 0)
            bad = true;
        if(strlen(lmdl) == 0)
            bad = true;
        if(bad)
            continue;

        GHashTable *lmfg_dict = NULL;
        GPtrArray *array = NULL;

        if(!(g_hash_table_contains(ids, lmfg)))
        {
            lmfg_dict = g_hash_table_new(g_str_hash, g_str_equal);
            g_hash_table_insert(ids, lmfg, lmfg_dict);
        }

        lmfg_dict = g_hash_table_lookup(ids, lmfg);

        if(!(g_hash_table_contains(lmfg_dict, lmdl)))
        {
            array = g_ptr_array_new ();
            g_hash_table_insert(lmfg_dict, lmdl, array);
        }
       
        if(g_hash_table_contains(ids, lmfg))
        {
            lmfg_dict = g_hash_table_lookup(ids, lmfg);
            if(g_hash_table_contains(lmfg_dict, lmdl))
                g_ptr_array_add ((GPtrArray *)g_hash_table_lookup(lmfg_dict, lmdl), (gpointer)((char *)key));
        }  
    }
}


GHashTable *getPPDNamesFromDeviceID(GHashTable *ppds,
                                    char *mfg,
                                    char *mdl,
                                    char *description,
                                    char *commandsets,
                                    const char *uri,
                                    const char *make_and_model)
{
    /*
        Obtain a best-effort PPD match for an IEEE 1284 Device ID.

        @param mfg: MFG or MANUFACTURER field
        @type mfg: string
        @param mdl: MDL or MODEL field
        @type mdl: string
        @param description: DES or DESCRIPTION field, optional
        @type description: string
        @param commandsets: CMD or COMMANDSET field, optional
        @type commandsets: string
        @param uri: device URI, optional (only needed for debugging)
        @type uri: string
        @param make_and_model: device-make-and-model string
        @type make_and_model: string
        @returns: a dict of fit (string) indexed by PPD name

    */
    
    GHashTableIter iter;
    gpointer key, value;
    GHashTable *id_dict = NULL;

    fprintf(stderr, "%s \n%s\n",mfg, mdl);
    char *orig_mfg = mfg,
         *orig_mdl = mdl;
    
    init_ids(ppds);


    if(!commandsets)
        commandsets = g_ptr_array_new ();

    /*
        # Start with an empty result list and build it up using
        # several search methods, in increasing order of fuzziness.
    */

    GHashTable *fit = g_hash_table_new(g_str_hash, g_str_equal);
    GHashTable *mfgl_dict = NULL;
    GPtrArray *array = NULL;

    /*
        # First, try looking up the device using the manufacturer and
        # model fields from the Device ID exactly as they appear (but
        # case-insensitively).
    */
    
    char *mfgl, *mdll;
    mfgl = strlwr(mfg);
    mdll = strlwr(mdl);

    bool id_matched = false;
    guint each ;

    if(g_hash_table_contains(ids, mfgl))
    {
        lmfg_dict = g_hash_table_lookup(ids, mfgl);
        if(g_hash_table_contains(mfgl_dict, mdll))
        {
            array = g_hash_table_lookup(mfgl_dict, mdll);
            
            for(each=0; each<array->len; each++)
                g_hash_table_insert(fit, (char *)g_ptr_array_index ((GPtrArray*)array, each), FIT_EXACT);
            id_matched = true;
        }
    } 

    /* 
        # The HP PPDs say "HP" not "Hewlett-Packard", so try that.
    */

    if(!(strcmp(mfgl, "hewlett-packard")))
    {
        if(g_hash_table_contains(ids, "hp"))
        {
            lmfg_dict = g_hash_table_lookup(ids, mfgl);
            if(g_hash_table_contains(mfgl_dict, mdll))
            {
                array = g_hash_table_lookup(mfgl_dict, mdll);
                
                for(each=0; each<array->len; each++)
                {
                    g_hash_table_insert(fit, (char *)g_ptr_array_index ((GPtrArray*)array, each), FIT_EXACT);
                    fprintf(stderr, "**** Incorrect IEEE 1284 Device ID: %s\n", (char *)g_ptr_array_index ((GPtrArray*)array, each));
                }
                fprintf(stderr, "**** Actual ID is MFG:%s;MDL:%s;\n",mfg, mdl);
                fprintf(stderr, "**** Please report a bug against the HPLIP component\n");
                id_matched = true;
            }
        } 
    }

    /*
        # Now try looking up the device by ppd-make-and-model.
    */

    fprintf(stderr, "Trying make/model names\n");
    char *mdls = NULL;
    init_makes();
    char *make = NULL;
    char *rmfg;

    if(!(strcmp(mfgl, "")))
    {
        char *mfg_mdl = ppdMakeModelSplit(mdl);
        mfg = strtok(mfg_mdl, ":");
        mdl = strtok(NULL, ":");
        mfgl = normalize(mfg);
        mdll = normalize(mdl);
    }

    fprintf(stderr, "mfgl: %s\n",mfgl);
    fprintf(stderr, "mdll: %s\n",mdll);


    GHashTable *mfgrepl = g_hash_table_new(g_str_hash, g_str_equal);
    g_hash_table_insert(mfgrepl, "hewlett-packard", "hp");
    g_hash_table_insert(mfgrepl, "lexmark international", "lexmark");
    g_hash_table_insert(mfgrepl, "kyocera", "kyocera mita");

    if(g_hash_table_contains(lmakes, mfgl))
    {
        /* Found manufacturer. */
        make = g_hash_table_lookup(lmakes, mfgl);
    }
    else if(g_hash_table_contains(mfgrepl, mfgl))
    {
        rmfg = g_hash_table_lookup(mfgrepl, mfgl);
        if(g_hash_table_contains(lmakes, rmfg))
        {
            mfg = rmfg;
            mfgl = mfg;
            /* Found manufacturer (after mapping to canonical name) */
            fprintf(stderr, "remapped mfgl: %s\n",mfgl);
            make = g_hash_table_lookup(lmakes, mfgl);
        }
    }

    fprintf(stderr, "make: %s\n",make);

    if(make)
    {
        mdls = g_hash_table_lookup(makes, make);
        mdlsl = g_hash_table_lookup(lmodels, normalize(make));

        /* Remove manufacturer name from model field */


    }




    

    GPtrArray *failed = NULL,
              *exact_cmd = NULL;

    /* 
        # Check by the URI whether our printer is connected via IPP
        # and if not, remove the PPD entries for driverless printing
        # (ppdname = "driverless:..." from the list)
    */

    if((!uri) || 
        (!(startswith("ipp:", uri)) && 
            !(startswith("ipps:", uri)) && 
            (!(startswith("dnssd", uri)) 
                || strcmp(uri, "._ipp"))))
    {
        failed = g_ptr_array_new ();
        g_hash_table_iter_init(&iter, fit);
        while (g_hash_table_iter_next(&iter, &key, &value))
        {
            if(startswith("driverless:", (char *)key))
                g_ptr_array_add (failed, (gpointer)(char *)key);
        }
        if(failed->len)
        {
            fprintf(stderr, "Removed ");
            for(i=0;i<failed->len;i++)
            {
                fprintf(stderr, "%s, ", (char *)g_ptr_array_index ((GPtrArray*)failed, i));
                g_hash_table_remove(fit, (char *)g_ptr_array_index ((GPtrArray*)failed, i));
            }
            fprintf(stderr, "due to non-IPP connection\n");
        }
        failed = g_ptr_array_free(failed, false);
    }


    /*
        # What about the CMD field of the Device ID?  Some devices
        # have optional units for page description languages, such as
        # PostScript, and they will report different CMD strings
        # accordingly.
        #
        # By convention, if a PPD contains a Device ID with a CMD
        # field, that PPD can only be used whenever any of the
        # comma-separated words in the CMD field appear in the
        # device's ID.
        # (See Red Hat bug #630058).
        #
        # We'll do that check now, and any PPDs that fail
        # (e.g. PostScript PPD for non-PostScript printer) can be
        # eliminated from the list.
        #
        # The reason we don't do this check any earlier is that we
        # don't want to eliminate PPDs only to have the fuzzy matcher
        # add them back in afterwards.
        #
        # While doing this, any drivers that we can positively confirm
        # as using a command set understood by the printer will be
        # converted from FIT_EXACT to FIT_EXACT_CMD.
    */

    GPtrArray *ppd_cmd_field;
    bool usable;

    if(id_matched && (commandsets->len > 0))
    {
        failed = g_ptr_array_new ();
        exact_cmd = g_ptr_array_new ();
        g_hash_table_iter_init(&iter, fit);
        while (g_hash_table_iter_next(&iter, &key, &value))
        {
            ppd_cmd_field  = NULL;
            ppds_attr *list = g_hash_table_lookup(ppds, (char *)key);
            
            if(list->ppd_device_id)
            {   
                ppd_cmd_field = g_ptr_array_new ();
                id_dict = parseDeviceID(list->ppd_device_id);
                g_ptr_array_add (ppd_cmd_field, (gpointer) g_hash_table_lookup(id_dict, "CMD"));
            }

            /*
                # ppd-type is not reliable for driver-generated
                # PPDs (see CUPS STR #3720).  Neither gutenprint
                # nor foomatic specify ppd-type in their CUPS
                # drivers.
                    
                # If this is a PostScript PPD we know which
                # command set it will use.
            */

            if((!ppd_cmd_field) && !(strstr((char *)key, ":")))
            {
                ppd_cmd_field = g_ptr_array_new ();
                if(!(strcmp((list->ppd_type), "postscript")))
                    g_ptr_array_add (ppd_cmd_field, (gpointer)((char *)"POSTSCRIPT"));
            }
            /* We can't be sure which command set this driver uses */
            if(!ppd_cmd_field)
                continue;

            usable = false;
            guint *index;
            for(i=0;i<ppd_cmd_field->len;i++)
            {
                pdl = (char *)g_ptr_array_index ((GPtrArray *)ppd_cmd_field, i);
                if(g_ptr_array_find(commandsets, pdl, index))
                {
                    usable = true;
                    break;
                }
            }

            if(usable)
                g_ptr_array_add (exact_cmd, (gpointer)((char *)key));
            else
                g_ptr_array_add (failed, (gpointer)((char *)key));
        }

        /* 
            # Assign the more specific fit "exact-cmd" to those that
            # positively matched the CMD field.
        */

        for(i=0;i<exact_cmd->len;i++)
        {
            if(!(strcmp(g_hash_table_lookup(fit, (char *)g_ptr_array_index ((GPtrArray *)exact_cmd, i)), FIT_EXACT)))
            {
                g_hash_table_insert(fit, (char *)g_ptr_array_index ((GPtrArray *)exact_cmd, i), FIT_EXACT_CMD);
                fprintf(stderr, "FIT_EXACT_CMD : %s  %s\n",FIT_EXACT_CMD, (char *)g_ptr_array_index ((GPtrArray *)exact_cmd, i));
            }
        }

    }























    return fit;
}




GPtrArray *orderPPDNamesByPreference(GPtrArray *ppdnamelist,
                                     GPtrArray *downloadedfiles,
                                     const char *make_and_model,
                                     const char *devid,
                                     GHashTable *fit)
{
    /*
        Sort a list of PPD names by preferred driver type.

        @param ppdnamelist: PPD names
        @type ppdnamelist: string list
            @param downloadedfiles: Filenames from packages downloaded
            @type downloadedfiles: string list
            @param make_and_model: device-make-and-model name
            @type make_and_model: string
            @param devid: Device ID dict
            @type devid: dict indexed by Device ID field name, of strings;
            except for CMD field which must be a string list
            @param fit: Driver fit string for each PPD name
            @type fit: dict of PPD name:fit
        @returns: string list
    */

    if(!ppdnamelist)
        ppdnamelist = g_ptr_array_new ();
    if(!downloadedfiles)
        downloadedfiles = g_ptr_array_new ();

    if(!fit)
        fit = g_hash_table_new(g_str_hash, g_str_equal);


}







