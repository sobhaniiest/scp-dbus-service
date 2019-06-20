.PHONY: all

CC = gcc
FLAGS = $(shell pkg-config --libs --cflags gio-2.0 gio-unix-2.0 glib-2.0 gtk+-3.0)
LIBS = -lcups 

all: main demo

gen:
	gdbus-codegen --generate-c-code newprinterdialog_dbus --c-namespace NewPrinterDialog_DBus --interface-prefix org.fedoraproject.Config.Printing. org.fedoraproject.Config.Printing.xml
	$(CC) -g -o $@ $^ -c $(FLAGS)

%.o: %.c
	$(CC) -g -o $@ $^ -c $(FLAGS)

demo: test.o newprinterdialog_dbus.o newprinter.o
	$(CC) -o $@ $^ $(FLAGS) $(LIBS)

main: ConfigPrinting.o ConfigPrintingNewPrinterDialog.o killtimer.o newprinterdialog_dbus.o ppdcache.o asyncconn.o asyncpk1.o asyncipp.o newprinter.o
	$(CC) -g -o $@ $^ $(FLAGS) $(LIBS)

clean:
	rm -f *.o demo main