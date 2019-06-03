#include "ppdcache.h"

cups = NULL; 
exc = NULL; 
dict *cache = NULL;
dict *modtimes = NULL;
list *queued = NULL;
const char *host = "\0";
int port = 0;
int encryption = 0;
bool connecting = false;
bool check_uptodate = true;

void insert(dict **head, char *str, float value)
{
    dict *c = (*head);
  
    if(*head == NULL)
    {
        (*head) = (dict *)malloc(sizeof(dict));
        (*head)->qname = str;
        (*head)->time = value;
        (*head)->next = NULL;
    }
    else
    {
        while(c->next != NULL)
            c = c->next;
        c->next = (dict *)malloc(sizeof(dict));
        c->next->qname = str;
        c->next->time = value;
        c->next->next = NULL;
    }
}

bool find_name(dict **head, char *str)
{
    bool found = false;
    dict *c = (*head);
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

void fetch_ppd(char *name, void(*callback)())
{
	if(check_uptodate & find_name(&modtimes, name))
	{

	}
}

void connect()
{
	callback = NULL;
	connecting = true;
	Connection(connected, connected, NULL, host, port, encryption, NULL, true, true);
}

void got_ppd()
{

}

void got_ppd3()
{

}

void connected()
{

}

void schedule_callback()
{
	
}
