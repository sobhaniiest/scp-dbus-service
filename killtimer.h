#ifndef KILLTIMER_H
#define KILLTIMER_H

/* General Header file - fprintf() : debuging purpose */
#include <stdio.h>
/* GLib methods and data types */
#include <glib.h>
/* Thread mutex lock and unlock : to avoid critical section problem */
#include <pthread.h>

extern GSourceFunc killfunc, kill_func;
extern int holds;
extern pthread_mutex_t lock; 
extern guint timeout;
extern guint timer;

void KillTimer(GSourceFunc killfunc);
void add_timeout();
gboolean kil();
void add_hold();
void remove_hold();
void alive();

#endif