#include "nstring.h"

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
    while(token != NULL)
    {
        tokens[i++] = token;
        token = strtok(NULL, delim);
    }
    return tokens;
}

int count_tokens(char *str, char ch)
{
    int i, count = 0;
    for(i=0;i<strlen(str);i++)
    {
        if(str[i] == ch)
            count++;
    }
    return count;
}

char *strstrip(char *s)
{
    size_t size;
    char *end;

    size = strlen(s);

    if (!size)
        return s;

    end = s + size - 1;
    while (end >= s && isspace(*end))
        end--;
    *(end + 1) = '\0';

    while (*s && isspace(*s))
        s++;

    return s;
}

char *rstrstrip(char *s, char r)
{
    size_t size;
    char *end;

    size = strlen(s);

    if (!size)
        return s;

    end = s + size - 1;

    while (end >= s && (isspace(*end) || (*end) == r))
        end--;
    *(end + 1) = '\0';

    while (*s && (isspace(*s)))
        s++;
    return s;
}

bool startswith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

void slice(char *str, char *buffer, int len)
{
    size_t i,j;
    j = 0;
    for(i=len;i<strlen(str);i++)
    {
        buffer[j] = str[i];
        j++;
    }
    buffer[j] = '\0';
}