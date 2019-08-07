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

/* 
    returns a copy of the string where all occurrences 
    of a substring is replaced with another substring 
*/
char *replace(char *str, char *old, char *new);
/* 
    split a string into tokens by the given "delim" 
*/
char **split(char *str, char *delim, int count);
/* 
    count no. of tokens can be obtained from the strings 
*/
int count_tokens(char *str, char ch);
/* 
    remove white space from the both end of a string 
*/
char *strstrip(char *s);
/* 
    remove white space and specified char - 
    'r' from the end of a string. 
*/
char *rstrstrip(char *s, char r);
/* 
    check a string starts with a substring or not 
*/
bool startswith(const char *pre, const char *str);
/* 
    check a string ends with a substring or not 
*/
bool endswith(const char *post, const char *str);
/* 
    slice a string from the given index - len 
*/
void slice(char *str, char *buffer, int len);
/* 
    reverse a string 
*/
char *reverse(const char *str);
/* 
    This function returns the lowest index in the string 
    "str" where substring "substr" is found else return -1 
*/
int find(char *str, char *substr);
/* 
    This function returns the highest index in the string 
    "str" where substring "substr" is found else return -1 
*/
int rfind(char *str, char *substr);
/* 
    Convert an uppercase string to a lowercase string 
*/
char *strlwr(char *s);
/* 
    Convert an lowercase string to a uppercase string 
*/
char *strupr(char *s);
/* 
    return common prefix of two given string if exist
    else return NULL
*/
char *commonprefix(char *s1, char *s2);

#endif