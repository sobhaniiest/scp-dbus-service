#ifndef ASYNCCONN_H
#define ASYNCCONN_H

#include <python3.6m/Python.h>
#include <stdio.h>
#include <stdbool.h>
#include <cups/cups.h>
#include <unistd.h>
#include <sys/types.h>


extern void(*fn_asyn)();
extern void(*reply_handler_asyn)();
extern void(*error_handler_asyn)();
extern void(*auth_handler_asyn)();
extern void(*reply_data_asyn)();
extern void(*error_data_asyn)();
extern void(*auth_data_asyn)();
extern bool destroyed_asyn, destroyed_cnn, try_as_root, prompt_allowed;
extern const char *host_asyn;
extern int port_asyn, encryption_asyn;
//extern ??? parent

void Asyn_Connection();


#endif
