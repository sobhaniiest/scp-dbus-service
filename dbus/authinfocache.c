#include "authinfocache.h"

/* Global */
dict *creds = NULL;

/* Function Definations */

/* Internal Functions */
static void insert(dict **head, 
                   const char *host, 
                   int port, 
                   const char *user, 
                   char *password);

static dict *find(dict **head, 
                  const char *host, 
                  int port);

static void del(dict **head, 
                const char *host, 
                int port);

static void insert(dict **head, 
                   const char *host, 
                   int port, 
                   const char *user, 
                   char *password)
{
    dict *c = (*head);
  
    if(*head == NULL)
    {
        (*head) = (dict *)malloc(sizeof(dict));
        (*head)->hname = host;
        (*head)->p = port;
        (*head)->uname = user;
        (*head)->pword = password;
        (*head)->next = NULL;
    }
    else
    {
        while(c->next != NULL)
            c = c->next;
        c->next = (dict *)malloc(sizeof(dict));
        c->next->hname = host;
        c->next->p = port;
        c->next->uname = user;
        c->next->pword = password;
        c->next->next = NULL;
    }
}

static dict *find(dict **head, 
                  const char *host, 
                  int port)
{
    bool found = false;
    dict *c = (*head);
    while(c != NULL)
    {
        if(!strcmp(c->hname, host) && c->p == port)
        {
            found = true;
            break;
        }
        c = c->next;
    }
    if(found)
        return c;
    else
        return NULL;
}

static void del(dict **head, 
                const char *host, 
                int port)
{
    dict *c,*p;
    c = (*head);
    if(!strcmp((*head)->hname, host) && (*head)->p == port)
        (*head)=(*head)->next;
    else
    {
        while(!strcmp(c->hname, host) && c->p == port)
        {
            p = c;
            c = c->next;
        }
        p->next = c->next;
    }
}

/* External Functions */

void cache_auth_info(const char *user, 
                     char *password, 
                     const char *host, 
                     int port)
{
	if(port == 0)
		port = 631;

	insert(&creds, host, port, user, password);
}

dict *lookup_auth_info(const char *host, int port)
{
	if(port == 0)
		port = 631;

	if(!find(&creds, host, port))
		return NULL;
	else
		return find(&creds, host, port);
}

void remove_auth_info(const char *host, int port)
{
	if(port == 0)
		port = 631;

	if(!find(&creds, host, port))
		return;
	else
		del(&creds, host, port);
}
