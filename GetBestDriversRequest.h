#ifndef GETBESTDRIVERSREQUEST_H
#define GETBESTDRIVERSREQUEST_H

#include <stdio.h>
#include <locale.h>
#include "GetBestDriversRequest.h"

void GBDRequest(const gchar *device_id,
                const gchar *device_make_and_model,
                const gchar *device_uri,
                const gchar *language);

#endif