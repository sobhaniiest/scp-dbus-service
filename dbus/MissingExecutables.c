#include "MissingExecutables.h"

char *exes_to_install[MAX_SIZE];
int exes_index;

static void WriteFd(FILE *file, 
                    ppd_file_t *ppd, 
                    int fd);

static char *pathcheck(char *name, char *path);
static void add_missing(char *exe);

static void WriteFd(FILE *file, 
                    ppd_file_t *ppd, 
                    int fd)
{
    char *line = NULL;
    size_t linelen = 0;
    FILE *out;
    int dfd;
    dfd = dup (fd);

    out = fdopen (dfd, "w");

    rewind(file);
    while(!feof(file)) 
    {
        int written = 0;
        ssize_t got = getline (&line, &linelen, file);
        if (got == -1)
            break;

        if (!strncmp (line, "*Default", 8)) 
        {
            char *keyword;
            char *start = line + 8;
            char *end;
            ppd_choice_t *choice;
            for(end = start; *end; end++)
            if(isspace (*end) || *end == ':')
                break;
            keyword = calloc(1, (end - start) + 1);
            strncpy(keyword, start, end - start);
            choice = ppdFindMarkedChoice (ppd, keyword);

            /*
            **  Treat PageRegion, PaperDimension and ImageableArea specially:
            **  if not marked, use PageSize option.
            */

            if (!choice && (!strcmp (keyword, "PageRegion") || !strcmp (keyword, "PaperDimension") || !strcmp (keyword, "ImageableArea")))
                choice = ppdFindMarkedChoice (ppd, "PageSize");

            if (choice) 
            {
                fprintf (out, "*Default%s: %s", keyword, choice->choice);
                if (strchr (end, '\r'))
                    fputs ("\r", out);
                fputs ("\n", out);
                written = 1;
            }
        }

        if (!written)
            fputs (line, out);
    }

    fclose (out);
    if (line)
        free (line);
}

static char *pathcheck(char *name, char *path)
{
    if(!(strcmp(name, "-")))
        return "builtin";
    char *p = strstr(name, "%");
    if(p)
        strcpy(p, "");
    if(strlen(name) == 0)
        return "true";
    if(name[0] == '/')
    {
        if(!access(name, X_OK))
            return name;
        else
            return NULL;
    }
    if(strstr(name, "="))
        return "builtin";
    char *arraystr[ARRAY_STR_LEN] = {":", ".", "[", "alias", "bind", "break", "cd",
                                     "continue", "declare", "echo", "else", "eval",
                                     "exec", "exit", "export", "fi", "if", "kill", "let",
                                     "local", "popd", "printf", "pushd", "pwd", "read",
                                     "readonly", "set", "shift", "shopt", "source",
                                     "test", "then", "trap", "type", "ulimit", "umask",
                                     "unalias", "unset", "wait"};

    for(int i = 0; i < ARRAY_STR_LEN; i++)
    {
        if(!(strcmp(name, arraystr[i])))
            return "builtin";
    }
    char *new_path = (char *)malloc(strlen(path));
    strcpy(new_path, path);
    int count_paths;
    char **components;
    char *file;
    count_paths = count_tokens(new_path, ':');
    components = split(new_path, ":", count_paths);

    for(int i = 0; i <= count_paths; i++)
    {
        file = (char *)malloc(strlen(components[i])+1+strlen(name));
        strcpy(file, rstrstrip(components[i], '/'));
        strcat(file, "/");
        strcat(file, name);
        if(!access(file, X_OK))
            return file;
    }

    return NULL;
}

static void add_missing(char *exe)
{
    char *p = strstr(exe, "%");
    if(p)
        strcpy(p, "");
    exes_to_install[exes_index] = (char *)malloc(strlen(exe));
    strcpy(exes_to_install[exes_index], exe);
    exes_index++;
}

char **missingexecutables(const char *ppd_filename)
{
    exes_index = 0;
    FILE *file = fopen (ppd_filename, "r");
    if(!file)
    {
        fprintf(stderr, "fopen failed\n");
        exit(-1);
    }
    /* Read a PPD file into memory and return PPD file record */
    ppd_file_t *ppd = ppdOpenFile(ppd_filename);
    if(!ppd)
    {
        fprintf(stderr, "ppdOpenFile failed\n");
        exit(-1);
    }

    /*
    **  Find the first matching attribute and return Attribute or NULL if not found
    **  Find a 'FoomaticRIPCommandLine' attribute.
    */

    ppd_attr_t *attr = ppdFindAttr(ppd, "FoomaticRIPCommandLine", NULL);

    int count_pipes, count_cmds, count_args, count_lines;
    char *cmd, *arg, *exe, *exepath, *search, *buffer, *lline; /* *mimetype, *cost; */
    char **pipes, **cmds, **args, **lines;
    
    if(attr)
    {
        char *cmdline = attr->value;

        /* Foomatic RIP command line to check. */
        cmdline = replace(cmdline, "&&\n", "");
        cmdline = replace(cmdline, "&qout", "\"");
        cmdline = replace(cmdline, "&lt", "<");
        cmdline = replace(cmdline, "&gt", ">");

        /* Don't try to handle sub-shells or unreplaced HTML entities. */
        if((strstr(cmdline, "(")) || (strstr(cmdline, "&")))
            cmdline = "";

        /* Strip out foomatic '%'-style place-holders */
        count_pipes = count_tokens(cmdline, ';');
        pipes = split(cmdline, ";", count_pipes);

        for(int i = 0; i <= count_pipes; i++)
        {
            cmd = strstrip(pipes[i]);
            count_cmds = count_tokens(cmd, '|');
            cmds = split(cmd, "|", count_cmds);
            for(int j = 0; j <= count_cmds; j++)
            {
                arg = strstrip(cmds[j]);
                count_args = count_tokens(arg, ' ');
                args = split(arg, " ", count_args);
                exe = args[0];
                exepath = pathcheck(exe, "/usr/bin:/bin");
                if(!exepath)
                {
                    add_missing(exe);
                    continue;
                }

                /*
                **  Main executable found.  But if it's 'gs',
                **  perhaps there is an IJS server we also need
                **  to check.
                */

                if(!(strcmp(basename(exepath), "gs")))
                {
                    search = "-sIjsServer=";
                    for(int k = 1; k < count_args; k++)
                    {
                        if(startswith(search, args[k]))
                        {
                            buffer = (char *)malloc(strlen(args[k])-strlen(search)+1);
                            slice(args[k], buffer, strlen(search));
                            strcpy(exe, buffer);
                            free(buffer);
                            exepath = pathcheck(exe, "/usr/bin:/bin");
                            if(!exepath)
                                add_missing(exe);
                            break;
                        }
                    }
                }
            }
            /* Next pipe. */
            if(!exepath)
                break;
        }
        
    }
    
    if(exepath || !(exe))
    {
        char template[PATH_MAX];
        char line[MAX_SIZE];
        int tmpfd;
        snprintf(template, sizeof (template), "XXXXXX");
        tmpfd = mkstemp (template);
        unlink(template);
        WriteFd(file, ppd, tmpfd);
        lseek(tmpfd, 0, SEEK_SET);
        FILE *f = fdopen(tmpfd, "rt");
        search = "*cupsFilter:";
        while(fgets(line, sizeof(line), f))
        {
            if(startswith(search, line))
            {
                buffer = (char *)malloc(strlen(line) - strlen(search) + 1);
                slice(line, buffer, strlen(search));
                lline = strstrip(buffer);
                count_lines = count_tokens(lline, ' ');
                if(count_lines == 2)
                {
                    lines = split(lline, " ", count_lines);
                    /*
                        mimetype = lines[0];
                        cost = lines[1];
                    */
                    exe = lines[2];
                }
                else
                    continue;
                
                char *new_exe = rstrstrip(exe, '"');

                exepath = pathcheck(new_exe, "/usr/lib/cups/filter:/usr/lib64/cups/filter");
                if (!exepath)
                {
                    char *parameter = (char *)malloc(21 + strlen(new_exe));
                    strcpy(parameter, "/usr/lib/cups/filter/");
                    strcat(parameter, new_exe);
                    add_missing(parameter);
                }
            }
        }

    }
    
    return exes_to_install;
}

