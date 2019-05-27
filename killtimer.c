#include "killtimer.h"

GSourceFunc killfunc, _killfunc;
int _holds;
pthread_mutex_t lock; 
guint _timeout;
guint _timer;

void KillTimer(GSourceFunc killfunc)
{
	_timeout = 30;
	_killfunc = killfunc;
	_holds = 0;
	_add_timeout();
	pthread_mutex_init(&lock, NULL);
}

void _add_timeout()
{
	_timer = g_timeout_add_seconds(_timeout, (void *)_kill, NULL);
}

void _kill()
{
	fprintf(stderr, "Timeout (%ds), exiting\n", _timeout);
	if(_killfunc)
		_killfunc;
	else
		exit(0);
}

void add_hold()
{
	pthread_mutex_lock(&lock);
	if(_holds == 0)
	{
		fprintf(stderr, "Kill timer stopped\n");
		g_source_remove(_timer);
	}
	_holds += 1;
	pthread_mutex_unlock(&lock);
}

void remove_hold()
{
	pthread_mutex_lock(&lock);
	if(_holds > 0)
	{
		_holds -= 1;
		if(_holds == 0)
		{
			fprintf(stderr, "Kill timer started\n");
			_add_timeout();
		}
	}
	pthread_mutex_unlock(&lock);
}

void alive()
{
	pthread_mutex_lock(&lock);
	if(_holds == 0)
	{
		g_source_remove(_timer);
		_add_timeout();
	}
	pthread_mutex_unlock(&lock);
}
