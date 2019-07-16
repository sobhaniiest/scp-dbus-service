EXTRA_DIST = interface
bin_PROGRAMS = scp
scp_SOURCES = ConfigPrinting.c ConfigPrinting.h \
	ConfigPrintingNewPrinterDialog.c ConfigPrintingNewPrinterDialog.h \
	killtimer.c killtimer.h \
	scp_interface.c scp_interface.h \
	ppdcache.c ppdcache.h \
	asyncconn.c asyncconn.h \
	asyncpk1.c asyncpk1.h \
	asyncipp.c asyncipp.h \
	newprinter.c newprinter.h \
	authinfocache.c authinfocache.h \
	nstring.c nstring.h \
	MissingExecutables.c MissingExecutables.h 

scp_CPPFLAGS  = $(GLIB_CFLAGS)
scp_CPPFLAGS += $(GIO_CFLAGS)
scp_CPPFLAGS += $(GIOUNIX_CFLAGS)
scp_CPPFLAGS += $(GTK_CFLAGS)

scp_LDADD  = -lcups -lpthread -lm
scp_LDADD += $(GLIB_LIBS)
scp_LDADD += $(GIO_LIBS)
scp_LDADD += $(GIOUNIX_LIBS)
scp_LDADD += $(GTK_LIBS)
scp_LDADD += $(CUPS_LIBS)
scp_LDADD += $(CUPS_TEMP)


scp_interface.c scp_interface.h: interface/org.fedoraproject.Config.Printing.xml
	$(AM_V_GEN)gdbus-codegen --generate-c-code scp_interface \
	--c-namespace scp_interface \
    --interface-prefix org.fedoraproject.Config.Printing \
    interface/org.fedoraproject.Config.Printing.xml