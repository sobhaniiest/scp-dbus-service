#ifndef KILLTIMER_H
#define KILLTIMER_H

#include <stdio.h>
#include <glib.h>
#include <pthread.h>

extern GSourceFunc killfunc, _killfunc;
extern int _holds;
extern pthread_mutex_t lock; 
extern guint _timeout;
extern guint _timer;

void KillTimer(GSourceFunc killfunc);
void _add_timeout();
void _kill();
void add_hold();
void remove_hold();
void alive();

#endif