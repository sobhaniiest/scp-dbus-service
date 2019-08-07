#include "ppds.h"

GHashTable *ids = NULL;
GHashTable *makes = NULL;
GHashTable *lmakes = NULL;
GHashTable *lmodels = NULL;
char *device_name[9] = {"MFG", "MDL", "CMD", "CLS", "DES", "SN", "S", "P", "J"};

static void init_ids(GHashTable *ppds);
static void init_makes(GHashTable *ppds);
static char *normalize (char *strin);

/*
    Parse an IEEE 1284 Device ID, so that it may be indexed by field name.

    @param device_id: IEEE 1284 Device ID, without the two leading length bytes
    @type device_id: string
    @returns: dict indexed by field name
*/
GHashTable *parseDeviceID(const char *device_id)
{
    GHashTable *id_dict = g_hash_table_new(g_str_hash, g_str_equal);

    int count_id, count_cmd;
    char **pieces, **cmds;
    char *name, *val, *key, *value;

    char *id = (char *)malloc(strlen(device_id));
    strcpy(id, device_id);

    count_id = count_tokens(id, ';');
    pieces = split(id, ";", count_id);

    for(int i = 0; i < count_id; i++)
    {
        if(!(strstr(pieces[i], ":")))
            continue;

        name = strtok(pieces[i], ":");
        val = strtok(NULL, ":");

        key = (char *)malloc(strlen(name));
        strcpy(key, strstrip(name));

        value = (char *)malloc(strlen(val));
        strcpy(value, strstrip(val));
        g_hash_table_insert(id_dict, key, value);
        //free(key);
        //free(value);
    }

    //free(id);
    //free(pieces);
    
    if(g_hash_table_contains(id_dict, "MANUFACTURER"))
        g_hash_table_insert(id_dict, "MFG", g_hash_table_lookup(id_dict, "MANUFACTURER"));
    if(g_hash_table_contains(id_dict, "MODEL"))
        g_hash_table_insert(id_dict, "MDL", g_hash_table_lookup(id_dict, "MODEL"));
    if(g_hash_table_contains(id_dict, "COMMAND SET"))
        g_hash_table_insert(id_dict, "CMD", g_hash_table_lookup(id_dict, "COMMAND SET"));

    for(int i = 0; i < 9; i++)
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
        for(int i = 0; i <= count_cmd; i++)
            g_ptr_array_add (array, (gpointer)cmds[i]);
        g_hash_table_insert(id_dict, "CMD", array);
    }
    
    return id_dict;
}

char *ppdMakeModelSplit(const char *ppd_make_and_model)
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

    char *MFR_BY_RANGE[MFR_BY_RANGE_LEN] = {"HP", "deskjet""|dj[ 0-9]?""|laserjet""|lj""|color laserjet""|color lj""|designjet""|officejet""|oj""|photosmart""|ps ""|psc""|edgeline",
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

    GHashTable *HP_MODEL_BY_NAME = g_hash_table_new(g_str_hash, g_str_equal);
    g_hash_table_insert(HP_MODEL_BY_NAME, "dj", "DeskJet");
    g_hash_table_insert(HP_MODEL_BY_NAME, "lj", "LaserJet");
    g_hash_table_insert(HP_MODEL_BY_NAME, "oj", "OfficeJet");
    g_hash_table_insert(HP_MODEL_BY_NAME, "color lj", "Color LaserJet");
    g_hash_table_insert(HP_MODEL_BY_NAME, "ps", "PhotoSmart");
    g_hash_table_insert(HP_MODEL_BY_NAME, "hp", "");
              
    char *make_and_model = strstrip(ppd_make_and_model);
    char *make = NULL;
    char *model = NULL;
    bool cleanup_make = false,
         found_mfr = false;
    char *l = (char *)malloc(strlen(make_and_model));
    strcpy(l, make_and_model);
    l = strlwr(l);
    int reti, j;
    regex_t regex;
    char msgbuf[100];

    for(int i = 0; i < MFR_BY_RANGE_LEN/2 ; i++)
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
            {
                j = 0;
                buffer = (char *)malloc(t2 - (t1+11));
                for(int i = t1+12; i <= t2; i++)
                    buffer[j++] = make_and_model[i];
                strcpy(make_and_model, buffer);
                free(buffer);
            }
            else
            {
                j = 0;
                buffer = (char *)malloc(strlen(make_and_model) - (t1+1));
                for(int i = 0; i <= t1; i++)
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
        buffer = (char *)malloc(strlen(make_and_model) - 21);
        slice(make_and_model, buffer, 22);
        strcpy(model, buffer);
        free(buffer);
    }
    else if(startswith("kyocera mita ", l))
    {
        make = "Kyocera";
        buffer = (char *)malloc(strlen(make_and_model) - 12);
        slice(make_and_model, buffer, 13);
        strcpy(model, buffer);
        free(buffer);
    }
    else if(startswith("kyocera ", l))
    {
        make = "Kyocera";
        buffer = (char *)malloc(strlen(make_and_model) - 7);
        slice(make_and_model, buffer, 8);
        strcpy(model, buffer);
        free(buffer);
    }
    else if(startswith("fuji xerox ", l))
    {
        make = "Fuji Xerox";
        buffer = (char *)malloc(strlen(make_and_model) - 10);
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
    char *makel = (char *)malloc(strlen(make));
    strcpy(makel, make);
    makel = strlwr(makel);
    char *mfr_by_range;
    
    if(cleanup_make)
    {
        if(startswith("hewlett", makel) && endswith("packard", makel))
        {
            make = "HP";
            free(makel);
            makel = (char *)malloc(strlen("hp"));
            strcpy(makel, "hp");
        }
        else if(startswith("konica", makel) && endswith("minolta", makel))
        {
            make = "KONICA MINOLTA";
            free(makel);
            makel = (char *)malloc(strlen("konica minolta"));
            strcpy(makel, "konica minolta");
        }
        else
        {
            /* Fix case errors. */
            for(int i = 0; i < MFR_BY_RANGE_LEN/2; i++)
            {
                mfr_by_range = (char *)malloc(strlen(MFR_BY_RANGE[2*i]));
                strcpy(mfr_by_range, MFR_BY_RANGE[2*i]);
                if(!(strcmp(strlwr(mfr_by_range), makel)))
                {
                    make = MFR_BY_RANGE[2*i];
                    break;
                }
                free(mfr_by_range);
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
        modell = model.lower ()
        v = modell.find (" v")
        if v != -1:
            # Look for " v" or " ver." followed by a digit, optionally
            # followed by more digits, a dot, and more digits; and
            # terminated by a space of the end of the line.
            vmatch = _RE_version_numbers.search (modell)
            if vmatch:
                # Found it -- truncate at that point.
                vstart = vmatch.start ()
                modell = modell[:vstart]
                model = model[:vstart]

        suffix = _RE_ignore_suffix.search (modell)
        if suffix:
            suffixstart = suffix.start ()
            modell = modell[:suffixstart]
            model = model[:suffixstart]
    */
    char *modell = (char *)malloc(strlen(model));
    strcpy(modell, model);
    modell = strlwr(modell);
    /*
        if(strstr(modell, " v"))
        {
            /*
            # Look for " v" or " ver." followed by a digit, optionally
            # followed by more digits, a dot, and more digits; and
            # terminated by a space of the end of the line.
            *//*
        
            RE_version_numbers = " v(?:er\\.)?\d(?:\d*\\.\\d+)?(?: |$)"
            
            reti = regcomp(&regex, RE_turboprint, REG_EXTENDED);
            reti = regexec(&regex, l, 0, NULL, 0);

        }
    */
    /*
        # Remove the word "Series" if present.  Some models are referred
        # to as e.g. HP OfficeJet Series 300 (from hpcups, and in the
        # Device IDs of such models), and other groups of models are
        # referred to in drivers as e.g. Epson Stylus Color Series (CUPS).

        (model, n) = _RE_ignore_series.subn ("", model, count=1)
        if n:
            modell = model.lower ()
    */

    if(!(strcmp(makel, "hp")))
    {
        GHashTableIter iter;
        gpointer name, fullname;
        g_hash_table_iter_init(&iter, HP_MODEL_BY_NAME);
        while (g_hash_table_iter_next(&iter, &name, &fullname))
        {
            if(startswith(name, modell))
            {
                buffer = (char *)malloc(strlen(model) - strlen((char *)name));
                slice(model, buffer, strlen((char *)name));
                model = (char *)malloc(strlen((char *)fullname) + strlen(buffer));
                strcpy(model, fullname);
                strcat(model, buffer);
                modell = (char *)malloc(strlen(model));
                strcpy(modell, model);
                modell = strlwr(modell);
                free(buffer);
                break;
            }
        }
    }

    model = strstrip(model);
    char *mfg_mdl = (char *)malloc(strlen(make) + strlen(model) + 1);
    strcpy(mfg_mdl, make);
    strcat(mfg_mdl, ":");
    strcat(mfg_mdl, model);
    
    return mfg_mdl;
}

static char *normalize (char *strin)
{
    /*
        # This function normalizes manufacturer and model names for comparing.
        # The string is turned to lower case and leading and trailing white
        # space is removed. After that each sequence of non-alphanumeric
        # characters (including white space) is replaced by a single space and
        # also at each change between letters and numbers a single space is added.
        # This makes the comparison only done by alphanumeric characters and the
        # words formed from them. So mostly two strings which sound the same when
        # you pronounce them are considered equal. Printer manufacturers do not
        # market two models whose names sound the same but differ only by
        # upper/lower case, spaces, dashes, ..., but in printer drivers names can
        # be easily supplied with these details of the name written in the wrong
        # way, especially if the IEEE-1284 device ID of the printer is not known.
        # This way we get a very reliable matching of printer model names.
        # Examples:
        # - Epson PM-A820 -> epson pm a 820
        # - Epson PM A820 -> epson pm a 820
        # - HP PhotoSmart C 8100 -> hp photosmart c 8100
        # - hp Photosmart C8100  -> hp photosmart c 8100
    */

    char *lstrin = strstrip(strin);
    lstrin = strlwr(lstrin);

    int BLANK = 0,
        ALPHA = 1,
        DIGIT = 2,
        lastchar = BLANK;

    size_t len = 0;
    char *normalized = (char *)malloc(strlen(lstrin));
    bool alnumfound = false;

    for(int i = 0; i < strlen(lstrin); i++)
    {
        if(isalpha(lstrin[i]))
        {
            if(lastchar != ALPHA && alnumfound)
            {
                normalized[len] = ' ';
                len++;
            }
            lastchar = ALPHA;
        }
        else if(isdigit(lstrin[i]))
        {
            if(lastchar != DIGIT && alnumfound)
            {
                normalized[len] = ' ';
                len++;
            }
            lastchar = DIGIT;
        }
        else
            lastchar = BLANK;

        if(isalnum(lstrin[i]))
        {
            normalized[len] = lstrin[i];
            alnumfound = true;
            len++;
        }
    }

    normalized[len] = '\0';
    return normalized;
}

void PPDs(GHashTable *ppds, 
          char *language, 
          const char *xml_dir)
{
    /*
        This function is for handling the list of PPDs returned by CUPS.  It
        indexes by PPD name and device ID, filters by natural language so
        that foreign-language PPDs are not included, and sorts by driver
        type.  If an exactly-matching PPD is not available, it can
        substitute with a PPD for a similar model or for a generic driver.

        @type ppds: dict : index by PPD name and value is a struct of 
        type 'ppds_attr' define in asyncipp.h
        @param ppds: dict of PPDs as returned by the getPPDs() in asyncipp.c

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
    
    GPtrArray *child = PreferredDrivers (xmlfile);

    if(!child)
       fprintf(stderr, "Error loading %s\n" ,xmlfile); 
    else
    {
        xmlNodePtr drivertypes = (xmlNodePtr)g_ptr_array_index ((GPtrArray*)child, 1);
        DriverTypes_load(drivertypes);

        xmlNodePtr preferenceorder = (xmlNodePtr)g_ptr_array_index ((GPtrArray*)child, 2);  
        PreferenceOrder_load(preferenceorder);
    }
    
    if((!language) || (!(strcmp(language, "C"))) || (!(strcmp(language, "POSIX"))))
        language = "en_US"; //

    char *short_language = (char *)malloc(strlen(language));
    // use find !!!
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
    gpointer ppdname, ppddict;

    GPtrArray *to_remove = g_ptr_array_new ();
    g_hash_table_iter_init(&iter, ppds);
    while (g_hash_table_iter_next(&iter, &ppdname, &ppddict))
    {
        if(((ppds_attr *)ppddict)->ppd_natural_language == NULL ||
           (!strcmp(((ppds_attr *)ppddict)->ppd_natural_language, "en")) || 
           (!strcmp(((ppds_attr *)ppddict)->ppd_natural_language, language)) ||
           (!strcmp(((ppds_attr *)ppddict)->ppd_natural_language, short_language)))
        {
            /*
                Some manufacturer's PPDs are only available in this
                language, so always let them though.
            */
            continue;
        }
        g_ptr_array_add (to_remove, (gpointer)((char *)ppdname));
    }
    for(int i = 0; i < to_remove->len; i++)
        g_hash_table_remove(ppds, (char *)g_ptr_array_index ((GPtrArray *)to_remove, i));
    g_ptr_array_free(to_remove, false);

    /*
        CUPS sets the 'raw' model's ppd-make-and-model to 'Raw Queue'
        which unfortunately then appears as manufacturer Raw and
        model Queue.  Use 'Generic' for this model.
    */

    char *makemodel, *gmakemodel;
    g_hash_table_iter_init(&iter, ppds);
    while (g_hash_table_iter_next(&iter, &ppdname, &ppddict))
    {
        if((!strcmp((char *)ppdname, "raw")))
        {
            makemodel = ((ppds_attr *)ppddict)->ppd_make_and_model;
            if(!(startswith("Generic ", makemodel)))
            {
                gmakemodel = (char *)malloc(strlen(makemodel)+strlen("Generic "));
                strcpy(gmakemodel, "Generic ");
                strcat(gmakemodel, makemodel);
                ((ppds_attr *)ppddict)->ppd_make_and_model = gmakemodel; //
            }
        }
    }
}

static void init_ids(GHashTable *ppds)
{
    
    if(ids)
        return;

    ids = g_hash_table_new(g_str_hash, g_str_equal);
    char *lmfg, *lmdl;
    fprintf(stderr, "size : %d\n",g_hash_table_size(ppds) );
    GHashTableIter iter;
    gpointer ppdname, ppddict;
    g_hash_table_iter_init(&iter, ppds);
    while (g_hash_table_iter_next(&iter, &ppdname, &ppddict))
    {
        if(!strcmp((((ppds_attr *)ppddict)->ppd_device_id), ""))
            continue;

        GHashTable *id_dict = parseDeviceID(((ppds_attr *)ppddict)->ppd_device_id);
        
        lmfg = strlwr(g_hash_table_lookup(id_dict, "MFG"));
        lmdl = strlwr(g_hash_table_lookup(id_dict, "MDL"));

        //fprintf(stderr, "MFG : %s  MDL : %s\n",lmfg, lmdl);

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
                g_ptr_array_add ((GPtrArray *)g_hash_table_lookup(lmfg_dict, lmdl), (gpointer)((char *)ppdname));
        } 
        g_hash_table_unref (id_dict);
        //g_hash_table_destroy(id_dict);
                          
    }
}

static void init_makes(GHashTable *ppds)
{
    if(makes)
        return;
    //tstart = time.time ()
    makes = g_hash_table_new(g_str_hash, g_str_equal);
    GHashTable *lmakes = g_hash_table_new(g_str_hash, g_str_equal);
    GHashTable *lmodels = g_hash_table_new(g_str_hash, g_str_equal);
    // Generic model name: set(specific model names)
    GHashTable *aliases = g_hash_table_new(g_str_hash, g_str_equal);
    GPtrArray *ppd_makes_and_models = g_ptr_array_new ();
    GPtrArray *models = NULL;

    char *ppd_make_and_model;
    char *ppd_products, *ppd_product, *product;
    char *ppd_mm_split;
    char *make, *lmake;
    char *model, *lmodel;
    char *make_model;
    char *lprod;

    GHashTableIter iter;
    gpointer ppdname, ppddict;
    g_hash_table_iter_init(&iter, ppds);
    while (g_hash_table_iter_next(&iter, &ppdname, &ppddict))
    {
        // One entry for ppd-make-and-model
        ppd_make_and_model = ((ppds_attr *)ppddict)->ppd_make_and_model;
        ppd_mm_split = ppdMakeModelSplit (ppd_make_and_model);
        g_ptr_array_add (ppd_makes_and_models, (gpointer) ppd_mm_split);

        /* 
            # The ppd-product IPP attribute contains values from each
            # Product PPD attribute as well as the value from the
            # ModelName attribute if present.  The Product attribute
            # values are surrounded by parentheses; the ModelName
            # attribute value is not.

            # Add another entry for each ppd-product that came from a
            # Product attribute in the PPD file.
        */

        /*
        *   check this part : incomplete!!!
        */
        ppd_products = ((ppds_attr *)ppddict)->ppd_product;
        /*
            if not isinstance (ppd_products, list):
                    ppd_products = [ppd_products]
            ppd_products = set ([x for x in ppd_products if x.startswith ("(")])
        */
        if(ppd_products)
        {
            /*
                # If there is only one ppd-product value it is
                # unlikely to be useful.
            */
            make = ((ppds_attr *)ppddict)->ppd_make;
            
            if(make)
            {
                lmake = (char *)malloc(strlen(make)+1);
                strcpy(lmake, normalize(rstrstrip(make, ' ')));
            }
            // *Product: attribute is "(text)"
            if(startswith(ppd_products, "(") && endswith(ppd_products, ")"))
            {
                ppd_product = (char *)malloc(strlen(ppd_products)-2);
                slice(ppd_products, ppd_product, 1);
                ppd_product[strlen(ppd_product)-1] = '\0';
            }
            // If manufacturer name missing, take it from ppd-make
            lprod = normalize(ppd_product);
            if(!startswith(lprod, lmake))
            {
                product = (char *)malloc(strlen(make) + strlen(ppd_product));
                strcpy(product, make);
                strcat(product, ppd_product);
            }
            g_ptr_array_add (ppd_makes_and_models, (gpointer) (ppdMakeModelSplit (product)));
        }
        //  Add the entries to our dictionary
        for(int i = 0; i < ppd_makes_and_models->len; i++)
        {
            make_model = (char *)g_ptr_array_index ((GPtrArray*)ppd_makes_and_models, i);
            make = strtok(make_model, ":");
            model = strtok(NULL, ":");

            lmake = normalize(make);
            lmodel = normalize(model);

            if(!g_hash_table_contains(lmakes, lmake))
            {

                g_hash_table_insert(lmakes, lmake, make);
                g_hash_table_insert(lmodels, lmake, g_hash_table_new(g_str_hash, g_str_equal));
                g_hash_table_insert(makes, make, g_hash_table_new(g_str_hash, g_str_equal));
            }
            else
                make = g_hash_table_lookup(lmakes, lmake);

            if(!g_hash_table_contains(g_hash_table_lookup(lmodels, lmake), lmodel))
            {
                g_hash_table_insert(g_hash_table_lookup(lmodels, lmake), lmodel, model);
                g_hash_table_insert(g_hash_table_lookup(makes, make), model, g_hash_table_new(g_str_hash, g_str_equal));
            }
            else
                model = g_hash_table_lookup(g_hash_table_lookup(lmodels, lmake), lmodel);

            g_hash_table_insert(g_hash_table_lookup(g_hash_table_lookup(makes, make), model),
                               (char *)ppdname, (ppds_attr *)ppddict);
        }

        // Build list of model aliases
        for(int i = 0; i < ppd_makes_and_models->len; i++)
        {
            if(!strcmp((char *)g_ptr_array_index ((GPtrArray*)ppd_makes_and_models, i), ppd_mm_split))
                g_ptr_array_remove(ppd_makes_and_models, (gpointer)ppd_mm_split);
        }

        if(ppd_makes_and_models->len)
        {
            make = strtok(ppd_mm_split, ":");
            model = strtok(NULL, ":");   
            if(g_hash_table_contains(aliases, make))
            {
                models = g_hash_table_lookup(g_hash_table_lookup(aliases, make), model);
                if(!models)
                    models = g_ptr_array_new ();
            }
            else
            {
                g_hash_table_insert(aliases, make, g_hash_table_new(g_str_hash, g_str_equal));
                models = g_ptr_array_new ();
            }

            /*
                models = models.union ([x[1] for x in ppd_makes_and_models])
                aliases[make][model] = models
            */
        }
    }
    /*
        # Now, for each set of model aliases, add all drivers from the
        # "main" (generic) model name to each of the specific models.
    */
}

GHashTable *getPPDNamesFromDeviceID(GHashTable *ppds,
                                    char *mfg,
                                    char *mdl,
                                    char *description,
                                    GPtrArray *commandsets,
                                    const char *uri,
                                    const char *make_and_model)
{
    /*
        Obtain a best-effort PPD match for an IEEE 1284 Device ID.
        @param ppds: a dict indexed by PPD name and value - ppds_attr (look asyncipp.h)
        @type ppds: dict
        @param mfg: MFG or MANUFACTURER field
        @type mfg: string
        @param mdl: MDL or MODEL field
        @type mdl: string
        @param description: DES or DESCRIPTION field, optional
        @type description: string
        @param commandsets: CMD or COMMANDSET field, optional
        @type commandsets: array of string
        @param uri: device URI, optional (only needed for debugging)
        @type uri: string
        @param make_and_model: device-make-and-model string
        @type make_and_model: string
        @returns: a dict of fit (string) indexed by PPD name

    */
    
    GHashTableIter iter;
    gpointer key, value;
    GHashTable *id_dict = NULL;

    fprintf(stderr, "mfg : %s\nmdl : %s\n",mfg, mdl);
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
    GHashTable *mfgl_dict = NULL,
               *lmfg_dict = NULL;
    GPtrArray *array = NULL;

    /*
        # First, try looking up the device using the manufacturer and
        # model fields from the Device ID exactly as they appear (but
        # case-insensitively).
    */
    
    char *mfgl, *mdll, *mfg_mdl;
    mfgl = (char *)malloc(strlen(mfg));
    strcpy(mfgl, mfg);
    mfgl = strlwr(mfgl);
    mdll = (char *)malloc(strlen(mdl));
    strcpy(mdll, mdl);
    mdll = strlwr(mdll);

    bool id_matched = false;

    if(g_hash_table_contains(ids, mfgl))
    {
        lmfg_dict = g_hash_table_lookup(ids, mfgl);
        if(g_hash_table_contains(mfgl_dict, mdll))
        {
            array = g_hash_table_lookup(mfgl_dict, mdll);
            for(int each = 0; each < array->len; each++)
            {
                g_hash_table_insert(fit, (char *)g_ptr_array_index ((GPtrArray*)array, each), FIT_EXACT);
                fprintf(stderr, "%s\n", (char *)g_ptr_array_index ((GPtrArray*)array, each));
            }
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
                
                for(int each = 0; each < array->len; each++)
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
    GHashTable *mdls = NULL;
    GHashTable *mdlsl = NULL;
    init_makes(ppds);
    char *make = NULL;
    char *rmfg, *model;

    if(!(strcmp(mfgl, "")))
    {
        mfg_mdl = ppdMakeModelSplit(mdl);
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
    char *prefix, *buffer;
    char *mfg2, *mdl2, *mfg_mdl2, *mdl2l;

    if(make)
    {
        mdls = g_hash_table_lookup(makes, make);
        mdlsl = g_hash_table_lookup(lmodels, normalize(make));

        /* Remove manufacturer name from model field */
        prefix = (char *)malloc(strlen(mfgl) + 1);
        strcpy(prefix, mfgl);
        strcat(prefix, " ");
        if(startswith(prefix, mdll))
        {
            buffer = (char *)malloc(strlen(mdl) - strlen(prefix));
            slice(mdl, buffer, strlen(prefix));
            mdl = buffer;
            mdll = normalize(mdl);
            fprintf(stderr, "unprefixed mdll: %s\n", mdll);
        }
        else if(startswith("hewlett-packard ", mdll))
        {
            buffer = (char *)malloc(strlen(mdl) - strlen("hewlett-packard "));
            slice(mdl, buffer, strlen("hewlett-packard "));
            mdl = buffer;
            mdll = normalize(mdl);
            fprintf(stderr, "unprefixed mdll: %s\n", mdll);
        }
        else if(startswith("hp ", mdll))
        {
            buffer = (char *)malloc(strlen(mdl) - strlen("hp "));
            slice(mdl, buffer, strlen("hp "));
            mdl = buffer;
            mdll = normalize(mdl);
            fprintf(stderr, "unprefixed mdll: %s\n", mdll);
        }

        if(g_hash_table_contains(g_hash_table_lookup(lmodels, mfgl), mdll))
        {
            model = g_hash_table_lookup(mdlsl, mdll);
            g_hash_table_iter_init(&iter, g_hash_table_lookup(mdlsl, mdll));
            while (g_hash_table_iter_next(&iter, &key, &value))
            {
                g_hash_table_insert(fit, (char *)key, FIT_EXACT);
                fprintf(stderr, "%s: %s\n", FIT_EXACT, (char *)key);
            }
        }
        else
        {
            /*
                # Make use of the model name clean-up in the
                # ppdMakeModelSplit () function
            */
            mfg_mdl = (char *)malloc(strlen(mfg) + strlen(mdl) + 1);
            strcpy(mfg_mdl, mfg);
            strcat(mfg_mdl, " ");
            strcat(mfg_mdl, mdl);
            mfg_mdl2 = ppdMakeModelSplit(mfg_mdl);

            mfg2 = strtok(mfg_mdl2, ":");
            mdl2 = strtok(NULL, ":");

            mdl2l = normalize(mdl2);
            fprintf(stderr, "re-split mdll: %s\n", mdl2l);
            if(g_hash_table_contains(g_hash_table_lookup(lmodels, mfgl), mdl2l))
            {
                model = g_hash_table_lookup(mdlsl, mdl2l);
                g_hash_table_iter_init(&iter, g_hash_table_lookup(mdlsl, mdll));
                while (g_hash_table_iter_next(&iter, &key, &value))
                {
                    g_hash_table_insert(fit, (char *)key, FIT_EXACT);
                    fprintf(stderr, "%s: %s\n", FIT_EXACT, (char *)key);
                }
            }
        }
    }
//////////////////////////////////////////////////////////////////////////
    if((!fit) && mdls)
    {

    }

/////////////////////////////////////////////////////////////////////////
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
            for(int i = 0; i < failed->len; i++)
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
            char *pdl;
            for(int i = 0; i < ppd_cmd_field->len; i++)
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

        for(int i = 0; i < exact_cmd->len; i++)
        {
            if(!(strcmp(g_hash_table_lookup(fit, (char *)g_ptr_array_index ((GPtrArray *)exact_cmd, i)), FIT_EXACT)))
            {
                g_hash_table_insert(fit, (char *)g_ptr_array_index ((GPtrArray *)exact_cmd, i), FIT_EXACT_CMD);
                fprintf(stderr, "FIT_EXACT_CMD : %s  %s\n",FIT_EXACT_CMD, (char *)g_ptr_array_index ((GPtrArray *)exact_cmd, i));
            }
        }

        /*
        if len (failed) < len ([d for (d, m) in fit.items ()
                                    if m != 'generic']):
                _debugprint ("Removed %s due to CMD mis-match" % failed)
                for each in failed:
                    del fit[each]
            else:
                _debugprint ("Not removing %s " % failed +
                             "due to CMD mis-match as it would "
                             "leave nothing good")
        */
    }

    if(!fit)
    {
        char *fallback, *fallbackgz;
        GPtrArray *fallbacks = g_ptr_array_new ();
        g_ptr_array_add (fallbacks, (gpointer) "textonly.ppd");
        g_ptr_array_add (fallbacks, (gpointer) "postscript.ppd");
        bool found = false;
        for(int i = 0; i < fallbacks->len; i++)
        {
            fallback = (char *)g_ptr_array_index ((GPtrArray*)fallbacks, i);
            fprintf(stderr, "'%s' fallback\n", fallback);
            fallbackgz = (char *)malloc(strlen(fallback)+strlen(".gz"));
            strcpy(fallbackgz, fallback);
            strcat(fallbackgz, ".gz");
            g_hash_table_iter_init(&iter, ppds);
            while (g_hash_table_iter_next(&iter, &key, &value))
            {
                if(endswith(fallback, (char *)key) || endswith(fallbackgz, (char *)key))
                {
                    g_hash_table_insert(fit, (char *)key, FIT_NONE);
                    found = true;
                    break;
                }
            }
            if(found)
                break;

            fprintf(stderr, "Fallback '%s' not available\n", fallback);
        }
        
        /*
         if not found:
                _debugprint ("No fallback available; choosing any")
                fit[list(self.ppds.keys ())[0]] = self.FIT_NONE
        */
    }

    /*
        if not id_matched:
            sanitised_uri = re.sub (pattern="//[^@]*@/?", repl="//",
                                    string=str (uri))
            try:
                cmd = reduce (lambda x, y: x + ","+ y, commandsets)
            except TypeError:
                cmd = ""
            id = "MFG:%s;MDL:%s;" % (orig_mfg, orig_mdl)
            if cmd:
                id += "CMD:%s;" % cmd
            if description:
                id += "DES:%s;" % description

            _debugprint ("No ID match for device %s:" % sanitised_uri)
            _debugprint (id)
    */

    return fit;
    
}

fBMP_data *findBestMatchPPDs(GHashTable *mdls, char *mdl)
{
    /*
        Find the best-matching PPDs based on the MDL Device ID.
        This function could be made a lot smarter.
    */

    fprintf(stderr, "Trying best match\n");
    char *mdll = (char *)malloc(strlen(mdl));
    strcpy(mdll, mdl);
    mdll = strlwr(mdll);
    int len;
    if(endswith(" series", mdll))
    {
        /* Strip " series" from the end of the MDL field. */
        len = strlen(mdll) - 7; 
        mdll[len] = '\0';
        mdl[len] = '\0'; 
    }

    GPtrArray *best_mdl = g_ptr_array_new ();
    int best_matchlen = 0; 

    GPtrArray *mdlnames = g_ptr_array_new ();
    GPtrArray *mdlnamesl = g_ptr_array_new ();

    GHashTableIter iter;
    gpointer key, value;
    char *buffer;

    g_hash_table_iter_init(&iter, mdls);
    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        g_ptr_array_add (mdlnames, (gpointer) ((char *)key));
        buffer = (char *)malloc(strlen((char *)key));
        strcpy(buffer, (char *)key);
        g_ptr_array_add (mdlnamesl, (gpointer) (strlwr(buffer)));
        free(buffer);
    }
    g_ptr_array_add (mdlnames, (gpointer) mdl);
    g_ptr_array_add (mdlnamesl, (gpointer) mdll);


    // Perform a case-insensitive model sort on the names.

    char *namei, *namej, *temp;

    for (int i = 0; i < mdlnamesl->len - 1; i++)
    {
        for (int j = i + 1; j < mdlnamesl->len; j++)
        {
            namei = (char *)g_ptr_array_index ((GPtrArray*)mdlnamesl, i);
            namej = (char *)g_ptr_array_index ((GPtrArray*)mdlnamesl, j);
            if (g_ascii_strcasecmp(namei, namej) > 0) 
            {
                temp = namei;
                g_ptr_array_remove_index(mdlnamesl, i);
                g_ptr_array_insert(mdlnamesl, i, (gpointer) namej);
                g_ptr_array_remove_index(mdlnamesl, j);
                g_ptr_array_insert(mdlnamesl, j, (gpointer) temp);
            }
        }
    }

    int index;
    GHashTable *candidates = g_hash_table_new(g_str_hash, g_str_equal);
    for(int i = 0; i < mdlnamesl->len; i++)
    {
        if(!(g_ascii_strcasecmp((char *)g_ptr_array_index ((GPtrArray*)mdlnamesl, i), mdll)))
        {
            index = i;
            namei = (char *)g_ptr_array_index ((GPtrArray*)mdlnames, i-1);
            namej = (char *)g_ptr_array_index ((GPtrArray*)mdlnamesl, i-1);
            g_hash_table_insert(candidates, namei, namej);
            break;
        }
    }

    if((index + 1) < mdlnamesl->len)
    {
        namei = (char *)g_ptr_array_index ((GPtrArray*)mdlnames, index+1);
        namej = (char *)g_ptr_array_index ((GPtrArray*)mdlnamesl, index+1);
        g_hash_table_insert(candidates, namei, namej);
        namei = (char *)g_ptr_array_index ((GPtrArray*)mdlnames, index-1);
        namej = (char *)g_ptr_array_index ((GPtrArray*)mdlnames, index+1);
        fprintf(stderr, "%s <= %s <= %s\n", namei, mdl, namej);
    }
    else
    {
        namei = (char *)g_ptr_array_index ((GPtrArray*)mdlnames, index-1);
        fprintf(stderr, "%s <= %s\n", namei, mdl);
    }

    /*
        # Look at the models immediately before and after ours in the
        # sorted list, and pick the one with the longest initial match.
    */
    char *prefix;
    GHashTableIter itrr;
    gpointer candidate, candidatel;
    gpointer ppdname, ppddict;
    g_hash_table_iter_init(&iter, candidates);
    while (g_hash_table_iter_next(&iter, &candidate, &candidatel))
    {

        prefix = commonprefix((char *)candidatel, mdll);
        if(strlen(prefix) > best_matchlen)
        {
            g_hash_table_iter_init(&itrr, g_hash_table_lookup(mdls, (char *)candidate));
            while (g_hash_table_iter_next(&iter, &ppdname, &ppddict))
                g_ptr_array_add (best_mdl, (gpointer) ((char *)ppdname));

            best_matchlen = strlen(prefix);
            fprintf(stderr, "%s: match length %d\n", (char *)candidate, best_matchlen);
        }
    }

    /* Did we match more than half of the model name? */

    GPtrArray *ppdnamelist = g_ptr_array_new ();
    char *fit;

    if((best_mdl->len != 0) && (best_matchlen > strlen(mdll)/2))
    {
        for(int i = 0; i < best_mdl->len; i++)
            g_ptr_array_add (ppdnamelist, (gpointer) ((char *)g_ptr_array_index ((GPtrArray*)best_mdl, i)));

        if(best_matchlen == strlen(mdll))
            fit = FIT_EXACT;
        else
            fit = FIT_CLOSE;
    }
    else
    {
        fit = FIT_NONE;
        ppdnamelist = NULL;
        /*
            # Last resort.  Find the "most important" word in the MDL
            # field and look for a match based solely on that.  If
            # there are digits, try lowering the number of
            # significant figures.
        */

        for (int i = 0; i < mdlnames->len - 1; i++)
        {
            for (int j = i + 1; j < mdlnames->len; j++)
            {
                namei = (char *)g_ptr_array_index ((GPtrArray*)mdlnames, i);
                namej = (char *)g_ptr_array_index ((GPtrArray*)mdlnames, j);
                if (g_ascii_strcasecmp(namei, namej) > 0) 
                {
                    temp = namei;
                    g_ptr_array_remove_index(mdlnames, i);
                    g_ptr_array_insert(mdlnames, i, (gpointer) namej);
                    g_ptr_array_remove_index(mdlnames, j);
                    g_ptr_array_insert(mdlnames, j, (gpointer) temp);
                }
            }
        }

        GHashTable *mdlitems = g_hash_table_new(g_str_hash, g_str_equal);
        for(int i = 0; i < mdlnames->len; i++)
        {
            namei = (char *)g_ptr_array_index ((GPtrArray*)mdlnamesl, i);
            namej = (char *)g_ptr_array_index ((GPtrArray*)mdlnames, i);
            g_hash_table_insert(mdlitems, namei, g_hash_table_lookup(mdls, namej));
        }

        char *modelid = NULL;
        bool have_digits;

        int count_words = count_tokens(mdll, ' ');
        char **words = split(mdll, " ", count_words);

        for(int i = 0; i < count_words; i++)
        {
            if(!modelid)
            {
                modelid = (char *)malloc(strlen(words[i]));
                strcpy(modelid, words[i]);
            }
            have_digits = false;

            for(int j = 0; j < strlen(words[i]); j++)
            {
                if(isdigit(words[i][j]))
                {
                    have_digits = true;
                    break;
                }
            }

            if(have_digits)
            {
                if(modelid)
                    free(modelid);
                modelid = (char *)malloc(strlen(words[i]));
                strcpy(modelid, words[i]);
                break;
            }
        }

        int digits = 0,
            digits_start = -1,
            digits_end = -1,
            modelnumber = 0,
            ignore_digits,
            div;

        char *modelpattern;
        bool found;

        for(int i = 0; i < strlen(modelid); i++)
        {
            if(isdigit(modelid[i]))
            {
                if(digits_start == -1)
                    digits_start = i;
                digits_end = i;
                digits++;
            }
            else if(digits_start != -1)
                break;
        }
        digits_end++;
        if(digits > 0)
        {
            for(int i = digits_start; i < digits_end; i++)
                modelnumber = (modelnumber * 10) + ((int)modelid[i] - 48);

            modelpattern = (char *)malloc(strlen(modelid)+2);
            int k = 0;
            for(int i = 0; i < digits_start; i++)
                modelpattern[k++] = modelid[i];

            modelpattern[k] = '%';
            modelpattern[k+1] = 'd';

            for(int i = digits_end; i < strlen(modelid); i++)
                modelpattern[k++] = modelid[i];

            modelpattern[k] = '\0';

            fprintf(stderr, "Searching for model ID '%s', '%s' %% %d\n", modelid, modelpattern, modelnumber);
            ignore_digits = 0;
            best_mdl = g_ptr_array_free(best_mdl, false);
            found = false;
            while(ignore_digits < digits)
            {
                /*
                div = (int)(pow(10, ignore_digits));
                div = pow (10, ignore_digits)
                modelid = modelpattern % ((modelnumber / div) * div)
                _debugprint ("Ignoring %d of %d digits, trying %s" %
                             (ignore_digits, digits, modelid))
                */
                gpointer name, ppds;
                g_hash_table_iter_init(&iter, mdlitems);
                while (g_hash_table_iter_next(&iter, &name, &ppds))
                {
                    count_words = count_tokens((char *)name, ' ');
                    words = split((char *)name, " ", count_words);

                    for(int i = 0; i < count_words; i++)
                    {
                        if(!strcmp(strlwr(words[i]), modelid))
                        {
                            found = true;
                            break;
                        }
                    }
                    if(found)
                    {
                        gpointer ppdname, ppddict;
                        g_hash_table_iter_init(&iter, g_hash_table_lookup(mdlitems, (char *)name));
                        while (g_hash_table_iter_next(&iter, &ppdname, &ppddict))
                            g_ptr_array_add (best_mdl, (gpointer) ((char *)ppdname));
                        break;
                    }
                    
                }

                if(found)
                    break;
                ignore_digits++;
                if(digits < 2)
                    break;
            }

            if(found)
            {
                for(int i = 0; i < best_mdl->len; i++)
                    g_ptr_array_add (ppdnamelist, (gpointer) ((char *)g_ptr_array_index ((GPtrArray*)best_mdl, i)));
                fit = FIT_CLOSE;
            }
        } 
    }

    fBMP_data *data = (fBMP_data *)malloc(sizeof(fBMP_data));
    data->fit = fit;
    data->ppdnamelist = ppdnamelist;
    return data;
}

static GPtrArray *get(GHashTable *models, char *model)
{
    fBMP_data *data = findBestMatchPPDs(models, model);
    if(!strcmp(data->fit, FIT_EXACT))
        return data->ppdnamelist;
    else
        return NULL;
}

/*
    Return ppd-name list or None, given a list of strings 
    representing the command sets supported.
*/

GPtrArray *getPPDNameFromCommandSet(GPtrArray *commandsets, GHashTable *ppds)
{

    if(!commandsets)
        commandsets = g_ptr_array_new();

    init_makes(ppds);
    GHashTable *models;

    if(g_hash_table_contains(makes, "Generic"))
        models = g_hash_table_lookup(makes, "Generic");
    else
        return NULL;

    char *buffer;
    GPtrArray *return_list;
    GHashTable *cmdsets = g_hash_table_new(g_str_hash, g_str_equal);
    for(int i = 0; i < commandsets->len; i++)
    {
        buffer = (char *)malloc(strlen((char *)g_ptr_array_index ((GPtrArray*)commandsets, i)));
        strcpy(buffer, (char *)g_ptr_array_index ((GPtrArray*)commandsets, i));
        g_hash_table_insert(cmdsets, strlwr(buffer), "true");
        free(buffer);
    }

    if(g_hash_table_contains(cmdsets,"postscript") ||
       g_hash_table_contains(cmdsets,"postscript2") ||
       g_hash_table_contains(cmdsets,"postscript level 2 emulation"))
    {
        return get(models, "PostScript");
    }
    else if(g_hash_table_contains(cmdsets,"pclxl") ||
            g_hash_table_contains(cmdsets,"pcl-xl") ||
            g_hash_table_contains(cmdsets,"pcl6") ||
            g_hash_table_contains(cmdsets,"pcl 6 emulation"))
    {
        return_list = get(models, "PCL 6/PCL XL");
        if(return_list)
            return return_list;
        else
            return get(models, "PCL Laser");
    }
    else if(g_hash_table_contains(cmdsets,"pcl5e"))
    {
        return_list = get(models, "PCL 5e");
        if(return_list)
            return return_list;
        else
            return get(models, "PCL Laser");
    }
    else if(g_hash_table_contains(cmdsets,"pcl5c"))
    {
        return_list = get(models, "PCL 5c");
        if(return_list)
            return return_list;
        else
            return get(models, "PCL Laser");
    }
    else if(g_hash_table_contains(cmdsets,"pcl5") ||
            g_hash_table_contains(cmdsets,"pcl 5 emulation"))
    {
        return_list = get(models, "PCL 5");
        if(return_list)
            return return_list;
        else
            return get(models, "PCL Laser");
    }
    else if(g_hash_table_contains(cmdsets,"pcl"))
    {
        return_list = get(models, "PCL 3");
        if(return_list)
            return return_list;
        else
            return get(models, "PCL Laser");
    }
    else if(g_hash_table_contains(cmdsets,"escpl2") ||
            g_hash_table_contains(cmdsets,"esc/p2") ||
            g_hash_table_contains(cmdsets,"escp2e"))
    {
        return get(models, "ESC/P Dot Matrix");
    }

    return NULL;
}
/*
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
    /*
    if(!ppdnamelist)
        ppdnamelist = g_ptr_array_new ();
    if(!downloadedfiles)
        downloadedfiles = g_ptr_array_new ();

    if(!fit)
        fit = g_hash_table_new(g_str_hash, g_str_equal);

}
*/





