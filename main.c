
/*
For checking and testing purpose 
*/
#include <stdio.h>
#include <glib.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include "killtimer.h"

int main(int argc, char *argv[])
{
	printf("Hello!!!\n");
	KillTimer((void *)gtk_main_quit);
	return 0;
}

/*
gcc `pkg-config --cflags gtk+-3.0` main.c killtimer.c `pkg-config --cflags --libs glib-2.0 --libs gtk+-3.0` -o main
*/

