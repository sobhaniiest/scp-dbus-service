#include "killtimer.h"

GSourceFunc killfunc, kill_func;
int holds;
pthread_mutex_t lock; 
guint timeout;
guint timer;

void KillTimer(GSourceFunc killfunc)
{
	timeout = 30;
	kill_func = killfunc;
	holds = 0;
	add_timeout();
	pthread_mutex_init(&lock, NULL);
}

void add_timeout()
{
	timer = g_timeout_add_seconds(timeout, (void *)kil, NULL);
}

void kil()
{
	fprintf(stderr, "Timeout (%ds), exiting\n", timeout);
	if(kill_func)
		kill_func;
	else
		exit(0);
}

void add_hold()
{
	pthread_mutex_lock(&lock);
	if(holds == 0)
	{
		fprintf(stderr, "Kill timer stopped\n");
		g_source_remove(timer);
	}
	holds += 1;
	pthread_mutex_unlock(&lock);
}

void remove_hold()
{
	pthread_mutex_lock(&lock);
	if(holds > 0)
	{
		holds -= 1;
		if(holds == 0)
		{
			fprintf(stderr, "Kill timer started\n");
			add_timeout();
		}
	}
	pthread_mutex_unlock(&lock);
}

void alive()
{
	pthread_mutex_lock(&lock);
	if(holds == 0)
	{
		g_source_remove(timer);
		add_timeout();
	}
	pthread_mutex_unlock(&lock);
}
