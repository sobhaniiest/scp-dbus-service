#include "nstring.h"

/*
static void computeLPSArray(char* pat, int M, int* lps);
static int KMPSearch(char* pat, char* txt);
*/
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
/*
bool endswith(const char *post, const char *str)
{
    return startswith(reverse(post), reverse(str));
}
*/
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
/*
static void computeLPSArray(char* pat, int M, int* lps) 
{ 
    int len = 0; 
    lps[0] = 0; 
    int i = 1; 
    while (i < M) 
    { 
        if (pat[i] == pat[len]) 
        { 
            len++; 
            lps[i] = len; 
            i++; 
        } 
        else 
        { 
            if (len != 0) 
                len = lps[len - 1]; 
            else
            { 
                lps[i] = 0; 
                i++; 
            } 
        } 
    } 
}

static int KMPSearch(char* pat, char* txt) 
{ 
    int M = strlen(pat); 
    int N = strlen(txt); 
    int lps[M]; 

    computeLPSArray(pat, M, lps); 
  
    int i = 0; 
    int j = 0; 
    while (i < N) 
    { 
        if (pat[j] == txt[i]) 
        { 
            j++; 
            i++; 
        } 
        if (j == M) 
        { 
            return (i-j);
            //printf("Found pattern at index %d ", i - j); 
            //j = lps[j - 1]; 
        } 
        else if (i < N && pat[j] != txt[i]) 
        { 
            if (j != 0) 
                j = lps[j - 1]; 
            else
                i = i + 1; 
        } 
    } 
} 
  
char *reverse(char *str)
{
    int i;
    char temp;
    size_t n = strlen(str);
    char *new = (char *)malloc(n);
    strcpy(new, str);
    for(i = 0; i < n / 2; i++) 
    {
        temp = new[i];
        new[i] = new[n - i - 1];
        new[n - i - 1] = temp;
    }
    return new;
} 

int find(char *str, char *substr)
{
    if(!strstr(str, substr))
        return -1;
    else
        KMPSearch(substr, str);
}

int rfind(char *str, char *substr)
{
    if(!strstr(str, substr))
        return -1;
    else
    {
        int t = KMPSearch(reverse(substr), reverse(str));
        return(strlen(str) - (t+strlen(substr)));
    }
}
*/