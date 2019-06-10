.PHONY: all

CC = gcc
FLAGS = $(shell pkg-config --libs --cflags gio-2.0 gio-unix-2.0 glib-2.0 gtk+-3.0)
LIBS = -lcups -lpython3.6m

all: main demo

gen:
	gdbus-codegen --generate-c-code newprinterdialog_dbus --c-namespace NewPrinterDialog_DBus --interface-prefix com.test. com.test.xml
	$(CC) -o $@ $^ -c $(FLAGS)

%.o: %.c
	$(CC) -o $@ $^ -c $(FLAGS)

demo: test.o newprinterdialog_dbus.o
	$(CC) -o $@ $^ $(FLAGS)

main: ConfigPrintingNewPrinterDialog.o asyncconn.o ppdcache.o killtimer.o authinfocache.o newprinterdialog_dbus.o
	$(CC) -o $@ $^ $(FLAGS) $(LIBS)

clean:
	rm -f *.o demo main