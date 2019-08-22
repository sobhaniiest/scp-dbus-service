SCP_SERVICE_FLAGS=$(shell pkg-config --cflags --libs SCPService)
GLIB_FLAGS=$(shell pkg-config --cflags --libs gio-2.0 gio-unix-2.0 glib-2.0 gtk+-3.0)
LIBS = -lcups -lpthread -lm -lSCPService
.PHONY = all 

all:test

test:test.c 
	gcc -g -pg -o $@ $^ $(SCP_SERVICE_FLAGS) $(GLIB_FLAGS) $(LIBS)

clean:
	rm -f test