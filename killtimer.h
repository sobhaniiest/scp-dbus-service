#ifndef KILLTIMER_H
#define KILLTIMER_H

#include <stdio.h>
#include <glib.h>
#include <pthread.h>

extern GSourceFunc killfunc, kill_func;
extern int holds;
extern pthread_mutex_t lock; 
extern guint timeout;
extern guint timer;

void KillTimer(GSourceFunc killfunc);
void add_timeout();
void kil();
void add_hold();
void remove_hold();
void alive();

#endif