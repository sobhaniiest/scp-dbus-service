#ifndef MISSINGEXECUTABLES_H
#define MISSINGEXECUTABLES_H

#define _GNU_SOURCE
#include <fcntl.h> 
#include <unistd.h>
#include <sys/types.h>
/*dup access unlink lseek*/
#include <stdio.h> 
/*fdopen fopen fgets rewind feof getline snprintf fprintf fclose fputs*/
#include <stdlib.h> 
/*malloc calloc free exit mkstemp */
#include <string.h> 
/*strncpy strcmp strchr strstr strlen strcpy strcat*/
#include <libgen.h> /*basename*/
#include <ctype.h> /*isspace*/


#include <cups/cups.h>
#include <cups/ppd.h>
/*
    ppdFindMarkedChoice
    ppdFindAttr
*/
#include <stdbool.h>
#include "nstring.h"
/* 
    count_tokens
    split
    replace
    strstrip
    rstrstrip
    startswith
    slice
*/
#include "MissingExecutables.h"

extern int exes_index;
extern char *exes_to_install[1024];
char **missingexecutables(const char *ppd_filename);

#endif