#ifndef NSTRING_H
#define NSTRING_H

#include <stdlib.h> /*malloc*/
#include <string.h>
/*
    strlen
    strstr
    strcpy
    strtok
    strcmp
*/  
#include <ctype.h> /*isspace*/
#include <stdbool.h>
#include "nstring.h"

char *replace(char *str, char *old, char *new);
char **split(char *str, char *delim, int count);
int count_tokens(char *str, char ch);
char *strstrip(char *s);
char *rstrstrip(char *s, char r);
bool startswith(const char *pre, const char *str);
bool endswith(const char *post, const char *str);
void slice(char *str, char *buffer, int len);
char *reverse(const char *str);
int find(char *str, char *substr);
int rfind(char *str, char *substr);
char *strlwr(char *s);

#endif