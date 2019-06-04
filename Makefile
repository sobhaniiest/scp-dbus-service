#variables

CC = gcc
CFLAGS = -c 
FLAGS = $(shell pkg-config --cflags gtk+-3.0)
LIBS = $(shell pkg-config --cflags --libs glib-2.0 --libs gtk+-3.0)

all: demo

demo: main.c killtimer.c
	$(CC) $(FLAGS) main.c killtimer.c $(LIBS) -o demo

#main: main.o asyncconn.o ppdcache.o killtimer.o 
#	$(CC) main.o asyncconn.o ppdcache.o killtimer.o -o demo

#main.o: main.c
	#$(CC) $(CFLAGS) $(FLAGS) main.c $(LIBS)

#killtimer.o: killtimer.c
	#$(CC) $(CFLAGS) killtimer.c $(LIBS)

#asyncconn.o: asyncconn.c
#	$(CC) $(CFLAGS) $(LIBS) asyncconn.c

#ppdcache.o: ppdcache.c
#	$(CC) $(CFLAGS) $(LIBS) ppdcache.c

clean:
	rm -f demo
