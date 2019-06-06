#include "ppdcache.h"

void(*cups)();

/* Global variables */

dict_cache *cache = NULL;
dict_modtimes *modtimes = NULL;
list *queued = NULL;

/* default host, port and encryption */
const char *host = "\0";
int port = 0;
int encryption = 0;

bool connecting = false;
bool check_uptodate = true;

/* Function definations */

/* Initializing Function */
void PPDCache(const char *h, int p, int e)
{
    host = h;
    port = p;
    encryption = e;
}


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

void fetch_ppd(char *name, void(*callback)())
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
		if(cups == NULL)
		{
			insert_list(&queued, name, callback);
			if(!connecting)
				self_connect(callback);
			return;
		}
		http_status = cupsGetPPD3(CUPS_HTTP_DEFAULT, modtimes->qname, &(modtimes->time), filename, sizeof(filename));
		got_ppd3(name, http_status, modtimes->time, filename, callback);
		return;
	}
	else
	{
		f = find_file(&cache, name);
		if(f == NULL)
			schedule_callback(callback, name);
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

void self_connect(void(*callback)())
{
	connecting = true;
	//Asyn_Connection((void *)connected, (void *)connected, NULL, host, port, encryption, NULL, true, true);
}

void got_ppd3(char *name, http_status_t status, time_t time, char *fname, void(*callback)())
{
	if(status == HTTP_STATUS_NOT_MODIFIED)
		remove(fname);
	else if(status == HTTP_STATUS_OK)
	{
        FILE *fn;
		fn = fopen(fname,"rb");
		if(fn == NULL)
		{
			schedule_callback(callback, name);
			return;
		}
		insert_cache(&cache, name, fn);
		remove(fname);
		insert_modtimes(&modtimes, name, time);

		check_uptodate = false;
		fetch_ppd(name, callback);
	}
	else
		schedule_callback(callback, name);
}


void schedule_callback(void(*callback)(), char *name)
{
    /*

	g_idle_add()

    def _schedule_callback (self, callback, name, result, exc):
        def cb_func (callback, name, result, exc):
            Gdk.threads_enter ()
            callback (name, result, exc)
            Gdk.threads_leave ()
            return False

        GLib.idle_add (cb_func, callback, name, result, exc)
    */
}
