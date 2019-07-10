#ifndef KILLTIMER_H
#define KILLTIMER_H

#include <stdio.h> /*fprintf*/
#include <glib.h>
/*
    g_timeout_add_seconds
    g_source_remove
*/
#include <pthread.h>
/*
    pthread_mutex_init
    pthread_mutex_lock
    pthread_mutex_unlock
*/

void(*killfunc)();
void(*kill_func)();
extern int holds;
extern pthread_mutex_t lock; 
extern guint timeout;
extern guint timer;

void KillTimer(void(*killfunc)());
void add_timeout();
gboolean kil();
void add_hold();
void remove_hold();
void alive();

#endif