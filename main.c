
/*
For checking and testing purpose 
*/
#include <python3.6m/Python.h>
#include <stdio.h>
#include <glib.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include "killtimer.h"
#include "ppdcache.h"
#include "asyncconn.h"
#include "authinfocache.h"

typedef struct _dict_main
{
    char *hname;
    int p;
    char *uname;
    char *pword;
    struct _dict *next;
}dict_main;

void add()
{
	printf("%d\n", 2+3);
}

int main(int argc, char *argv[])
{
	printf("Hello!!!\n");
	KillTimer((void *)gtk_main_quit);

	printf("calling ppdcache!!!!\n");
	PPDCache((char *)"\0", 0, 0);
	fetch_ppd((char*)"test", add);

	printf("calling asyncconn connection!!!!\n");
	Asyn_Connection();

	printf("calling authinfocache!!!!\n");
	cache_auth_info((char *)"inro", (char *)"inro18", (char *)"test", 631);

	remove_hold();
	dict_main *new = lookup_auth_info((char *)"test", 631);
	printf("user :: %s\npassword :: %s\n",new->uname,new->pword);
	add_hold();
	
	return 0;
}

/*
gcc `pkg-config --cflags gtk+-3.0` main.c killtimer.c `pkg-config --cflags --libs glib-2.0 --libs gtk+-3.0` -o main
*/

/*

gcc `pkg-config --cflags gtk+-3.0` main.c ppdcache.c killtimer.c `pkg-config --cflags --libs glib-2.0 --libs gtk+-3.0` -lcups -lpython3.6m  -o main

*/