#ifndef MISSINGEXECUTABLES_H
#define MISSINGEXECUTABLES_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h> 
#include <sys/types.h>
#include <cups/cups.h>
#include <cups/ppd.h>
#include <iconv.h>
#include <stdbool.h>
#include "MissingExecutables.h"

typedef struct _exes_to_install
{
    char *exes;
    struct _exes_to_install *next;
}exes_to_install;

exes_to_install *missingexecutables(const char *ppd_filename);

#endif