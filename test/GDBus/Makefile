.PHONY: all
FLAGS=$(shell pkg-config --libs --cflags gio-2.0 gio-unix-2.0 glib-2.0)

all: server client recve

gen:
	gdbus-codegen --generate-c-code example --c-namespace Example --interface-prefix com.test. com.test.xml
		gcc -o $@ $^ -c $(FLAGS)

%.o: %.c
	gcc -o $@ $^ -c $(FLAGS)

server: server.o example.o 
	gcc -o $@ $^ $(FLAGS)
	
client: client.o example.o 
	gcc -o $@ $^ $(FLAGS)

recve: recve.o example.o
	gcc -o $@ $^ $(FLAGS)

clean:
	rm -f *.o server client recve