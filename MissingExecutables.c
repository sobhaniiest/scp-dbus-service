#include "MissingExecutables.h"

char *replace(char *str, char *old, char *new)
{
    char *result;
    int i, count = 0;
    int newlen = strlen(new);
    int oldlen = strlen(old);

    for(i=0;str[i]!='\0';i++)
    {
        if(strstr(&str[i], old) == &str[i])
        {
            count++;
            i += oldlen-1;
        }
    }

    result = (char *)malloc(i+count*(newlen-oldlen)+1);

    i = 0;
    while(*str)
    {
        if(strstr(str, old) == str)
        {
            strcpy(&result[i], new);
            i += newlen;
            str += oldlen;
        }
        else
            result[i++] = *str++;
    }
    result[i] = '\0';
    return result;
}

char **split(char *str, char *delim, int count)
{
    int i = 0;
    char **tokens = malloc(count * sizeof(char *));
    char *token = strtok(str, delim);
    while(token ! = NULL)
    {
        tokens[i++] = token;
        token = strtok(NULL, delim);
    }
    return tokens;
}

int count_tokens(char *str, char ch)
{
    int i,count = 0;
    for(i=0;i<strlen(str);i++)
    {
        if(str[i] == ch)
            count++;
    }
    return count;
}

int missingexecutables(const char *ppd_filename)
{
    ppd_file_t *ppd = ppdOpenFile(ppd_filename);
    if(!ppd)
    {
        fprintf(stderr, "ppdOpenFile failed\n");
        return -1;
    }

    ppd_attr_t *attr = ppdFindAttr(ppd, "FoomaticRIPCommandLine", NULL);

    if(attr)
    {
        char *cmdline = attr->value;
        cmdline = replace(cmdline, "&&\n", "");
        cmdline = replace(cmdline, "&qout", "\"");
        cmdline = replace(cmdline, "&lt", "<");
        cmdline = replace(cmdline, "&gt", ">");

        if(!(strstr(cmdline, "(")) || !(strstr(cmdline, "&")))
        {
            free(cmdline);
            char *cmdline = "";
        }
        
        int i, j, count_pipes, count_cmds, count_args;
        char *cmd, *arg, *exc, *exepath;
        char **pipes, **cmds, **args;

        count_pipes = count_tokens(cmdline, '/');
        pipes = split(cmdline, "/", count_pipes);

        for(i=0;i<=count_pipes;i++)
        {
            cmd = strstrip(pipes[i], " ");
            count_cmds = count_tokens(cmd, '|');
            cmds = split(cmd, "|", count_cmds);
            for(j=0;j<=count_cmds;j++)
            {
                arg = strstrip(cmd[j], " ");
                count_args = count_tokens(arg, ' ');
                args = split(arg, " ", count_args);
                exc = args[0];

                exepath = pathcheck(exc);
                if(!exepath)
                {
                    add_missing(exc);
                    continue;
                }




            }
        }


    }

}