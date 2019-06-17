#include "ppdcache.h"

dict_cache *cache = NULL;
dict_modtimes *modtimes = NULL;
list *queued = NULL;

/* Function definations */

void insert_cache(dict_cache **head, char *str, FILE *fpname)
{
    dict_cache *c = (*head);
  
    if(*head == NULL)
    {
        (*head) = (dict_cache *)malloc(sizeof(dict_cache));
        (*head)->qname = str;
        (*head)->fp = fpname;
        (*head)->next = NULL;
    }
    else
    {
        while(c->next != NULL)
            c = c->next;
        c->next = (dict_cache *)malloc(sizeof(dict_cache));
        c->next->qname = str;
        c->next->fp = fpname;
        c->next->next = NULL;
    }
}

void insert_modtimes(dict_modtimes **head, char *str, time_t value)
{
    dict_modtimes *c = (*head);
  
    if(*head == NULL)
    {
        (*head) = (dict_modtimes *)malloc(sizeof(dict_modtimes));
        (*head)->qname = str;
        (*head)->time = value;
        (*head)->next = NULL;
    }
    else
    {
        while(c->next != NULL)
            c = c->next;
        c->next = (dict_modtimes *)malloc(sizeof(dict_modtimes));
        c->next->qname = str;
        c->next->time = value;
        c->next->next = NULL;
    }
}

void insert_list(list **head, char *str, void(*func)())
{
    list *c = (*head);
  
    if(*head == NULL)
    {
        (*head)=(list *)malloc(sizeof(list));
        (*head)->qname = str;
        (*head)->method = func;
        (*head)->next = NULL;
    }
    else
    {
        while(c->next != NULL)
            c = c->next;
        c->next = (list *)malloc(sizeof(list));
        c->next->qname = str;
        c->next->method = func;
        c->next->next = NULL;
    }
}

bool find_cache(dict_cache **head, char *str)
{
    bool found = false;
    dict_cache *c = (*head);
    while(c != NULL)
    {
        if(!strcmp(c->qname, str))
        {
        	found = true;
        	break;
        }
        c = c->next;
    }
    return found;
}

bool find_modtimes(dict_modtimes **head, char *str)
{
    bool found = false;
    dict_modtimes *c = (*head);
    while(c != NULL)
    {
        if(!strcmp(c->qname, str))
        {
            found = true;
            break;
        }
        c = c->next;
    }
    return found;
}

FILE *find_file(dict_cache **head, char *str)
{
    bool found = false;
    dict_cache *c = (*head);
    while(c != NULL)
    {
        if(!strcmp(c->qname, str))
        {
            found = true;
            break;
        }
        c = c->next;
    }
    if(found)
        return c->fp;
    else
        return NULL;
}

void fetch_ppd(char *name, 
               void(*callback)(),
               bool check_uptodate
               const char *host,
               int port,
               http_encryption_t encryption,
               int connecting)
{
	http_status_t http_status;
	char filename[1024] = "\0";
	FILE *f;

	if(check_uptodate & find_modtimes(&modtimes, name))
	{
		http_status = cupsGetPPD3(CUPS_HTTP_DEFAULT, modtimes->qname, &(modtimes->time), filename, sizeof(filename));
		got_ppd3(name, http_status, modtimes->time, filename, callback);
		return;
	}

	if(!find_cache(&cache, name))
	{
        //KeyError
		if(cups == NULL)
		{
			insert_list(&queued, name, callback);
			if(!connecting)
				self_connect(NULL, host, port, encryption, connecting);
			return;
		}
		http_status = cupsGetPPD3(CUPS_HTTP_DEFAULT, modtimes->qname, &(modtimes->time), filename, sizeof(filename));
		got_ppd3(name, http_status, modtimes->time, filename, callback);
		return;
	}
	else
	{
        //RuntimeError
		f = find_file(&cache, name);
		if(f == NULL)
			schedule_callback(callback, name, "\0");
		return;
	}

	/* fseek(f, 0, SEEK_SET); 
    
    f.seek (0)
        with NamedTemporaryFile () as tmpf:
            copyfileobj (f, tmpf)

            try:
                ppd = cups.PPD (tmpf.file)
                self._schedule_callback (callback, name, ppd, None)
            except Exception as e:
                self._schedule_callback (callback, name, None, e)
    */
}

void connected()
{
	connecting = false;
    
    /*

    if isinstance (exc, Exception):
        self._cups = None
        self._exc = exc
    else:
        self._cups = connection

    queued = self._queued
    self._queued = list()
    for name, callback in queued:
        self.fetch_ppd (name, callback)
	
    */
}

void self_connect(void(*callback)(),
                  const char *host,
                  int port,
                  http_encryption_t encryption,
                  int connecting)
{
	connecting = true;
	Async_Connection(connected, 
                     connected, 
                     NULL, 
                     host, 
                     port, 
                     encryption,  
                     true, 
                     true);
}

void got_ppd3(char *name, 
              http_status_t status, 
              time_t time, 
              char *fname, 
              void(*callback)())
{
	if(status == HTTP_STATUS_NOT_MODIFIED)
		remove(fname);
	else if(status == HTTP_STATUS_OK)
	{
        FILE *fn;
		fn = fopen(fname,"rb");
		if(fn == NULL)
		{
			schedule_callback(callback, name, "\0");
			return;
		}
		insert_cache(&cache, name, fn);
		remove(fname);
		insert_modtimes(&modtimes, name, time);

		check_uptodate = false;
		fetch_ppd(name, callback, false);
	}
	else
		schedule_callback(callback, name, "\0");
}


void schedule_callback(void(*callback)(), char *name, char *ppd)
{
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_lock(&lock);
    callback(name, ppd);
    pthread_mutex_unlock(&lock);
}
