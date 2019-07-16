#include "ppds.h"

static void delete_ppdname(ppds_attr **head, char *data);

static void delete_ppdname(ppds_attr **head, char *data)
{
    ppds_attr *c,*p;
    c=(*head);
    if(!(strcmp((*head)->ppdname, data)))
        (*head)=(*head)->next;
    else
    {
        while(!(strcmp(c->ppdname, data)))
        {
            p = c;
            c = c->next;
        }
        p->next=c->next;
    }
}

void PPDs(ppds_attr *ppds, char *language, const char *xml_dir)
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
            xml_dir = "/etc/cupshelpers/"
    }

    char *xmlfile = (char *)malloc(strlen(xml_dir)+strlen("preferreddrivers.xml"));
    strcpy(xmlfile, xml_dir);
    strcat(xmlfile, "preferreddrivers.xml");

    /*


    */

    if((!language) || (!(strcmp(language, "C"))) || (!(strcmp(language, "POSIX"))))
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

    ppds_attr *ptr = (*ppds);
    while(ptr != NULL)
    {
        if((!strcmp(ptr->ppd_natural_language, "en")) || 
           (!strcmp(ptr->ppd_natural_language, language)) ||
           (!strcmp(ptr->ppd_natural_language, short_language)))
        {
            /*
                Some manufacturer's PPDs are only available in this
                language, so always let them though.
            */
            ptr = ptr->next;
            continue;
        }
        delete_ppdname(ppds, ptr->name);
        //ptr = ptr->next;
    }

    /*
        CUPS sets the 'raw' model's ppd-make-and-model to 'Raw Queue'
        which unfortunately then appears as manufacturer Raw and
        model Queue.  Use 'Generic' for this model.
    */

    ptr = (*ppds);
    char *makemodel, *gmakemodel;
    while(ptr != NULL)
    {
        if((!strcmp(ptr->ppdname, "raw")))
        {
            makemodel = ptr->ppd-make-and-model;
            if(!(startswith("Generic ", makemodel)))
            {
                gmakemodel = (char *)malloc(strlen(makemodel)+strlen("Generic "));
                strcpy(gmakemodel, "Generic ");
                strcat(gmakemodel, makemodel);
                ptr->ppd-make-and-model = gmakemodel;
            }
        }
        ptr = ptr->next;
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
    char *model;
    bool cleanup_make = false,
         found_mfr = false;
    char *l = strlwr(make_and_model);
    int i, j;
    int reti;
    regex_t regex;
    char msgbuf[100];
    bool cleanup_make;

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
            fprintf(stderr, "Match\n");
            make = MFR_BY_RANGE[2*i];
            model = make_and_model;
            break;
        }
        else if( reti == REG_NOMATCH )
            fprintf(stderr, "Not match\n");
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
    /* Finally, take the first word as the name of the manufacturer. */
    else
    {
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

    char *modell = strlwr(model);
    if(strstr(modell, " v"))
    {
        /*
        # Look for " v" or " ver." followed by a digit, optionally
        # followed by more digits, a dot, and more digits; and
        # terminated by a space of the end of the line.
        */
        RE_version_numbers = " v(?:er\\.)?\d(?:\d*\\.\\d+)?(?: |$)"
        reti = regcomp(&regex, RE_turboprint, REG_EXTENDED);
        reti = regexec(&regex, l, 0, NULL, 0);

    }



}

getPPDNamesFromDeviceID(char *mfg, 
                        char *mdl,
                        char *description,
                        char *commandsets,
                        char *uri,
                        char *make_and_model)
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

    fprintf(stderr, "%s %s\n",mfg, mdl);
    char *orig_mfg = mfg,
         *orig_mdl = mdl;

    init_ids();



































    

}

orderPPDNamesByPreference()
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
}


init_ids(ppds_attr **head)
{
    if(ids)
        return;

    ppds_attr *p = (*head);
    char *id;

    while(p != NULL)
    {
        id = p_>ppd_device_id;
        if(!id)
            continue;

        device_id_dict *id_dict = parseDeviceID(id);

        char *lmfg, *lmdl;

        device_id_dict *c = id_dict;
        while(c != NULL)
        {
            if(!(strcmp(c->name, "MFG")))
                lmfg = strlwr(c->value);
            if(!(strcmp(c->name, "MDL")))
                lmdl = strlwr(c->value);
            c = c->next;
        }

        bool bad = false;
        if(strlen(lmfg) == 0)
            bad = true;
        if(strlen(lmdl) == 0)
            bad = true;
        if(bad)
            continue;






        p = p->next;

    }

}
