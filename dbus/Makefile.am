EXTRA_DIST = interface
bin_PROGRAMS = scp
lib_LTLIBRARIES = libSCPService.la
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
	MissingExecutables.c MissingExecutables.h \
	PPDialog.c PPDialog.h \
   GetBestDriversRequest.c GetBestDriversRequest.h \
	ppds.c ppds.h \
	xml.c xml.h \
   JobApplet.c JobApplet.h \
   GroupPhysicalDevicesRequest.c GroupPhysicalDevicesRequest.h 


scp_CPPFLAGS  = $(GLIB_CFLAGS)
scp_CPPFLAGS += $(GIO_CFLAGS)
scp_CPPFLAGS += $(GIOUNIX_CFLAGS)
scp_CPPFLAGS += $(GTK_CFLAGS)
scp_CPPFLAGS += $(XML_CFLAGS)

scp_LDADD  = -lcups -lxml2 -lz -lpthread -lm
#scp_LDADD  = -lcups -lz -lpthread -lm
scp_LDADD += $(GLIB_LIBS)
scp_LDADD += $(GIO_LIBS)
scp_LDADD += $(GIOUNIX_LIBS)
scp_LDADD += $(GTK_LIBS)
scp_LDADD += $(CUPS_LIBS)
scp_LDADD += $(CUPS_TEMP)
scp_LDADD += $(LIB_AVAHI)

scp_LDFLAGS = -I/usr/include/libxml2 -L/usr/lib -lavahi-common -lavahi-client
#scp_LDFLAGS = -L/usr/lib
scp_LDFLAGS += $(XML_LIBS)
scp_LDFLAGS += $(CUPS_CFLAGS)

scp_interface.c scp_interface.h: interface/org.fedoraproject.Config.Printing.xml
	$(AM_V_GEN)gdbus-codegen --generate-c-code scp_interface \
	--c-namespace scp_interface \
    --interface-prefix org.fedoraproject.Config.Printing \
    interface/org.fedoraproject.Config.Printing.xml

libSCPService_la_SOURCES = nstring.c \
                           MissingExecutables.c \
                           asyncconn.c \
                           killtimer.c \
                           newprinter.c \
                           scp_interface.c \
                           PPDialog.c \
                           authinfocache.c \
                           asyncpk1.c \
                           asyncipp.c \
                           ConfigPrintingNewPrinterDialog.c \
                           ppdcache.c \
                           GroupPhysicalDevicesRequest.c \
                           ppds.c \
                           xml.c \
                           JobApplet.c

libSCPService_la_CPPFLAGS  = $(GLIB_CFLAGS)
libSCPService_la_CPPFLAGS += $(GIO_CFLAGS)
libSCPService_la_CPPFLAGS += $(GIOUNIX_CFLAGS)
libSCPService_la_CPPFLAGS += $(GTK_CFLAGS)
libSCPService_la_CPPFLAGS += $(XML_CFLAGS)

libSCPService_la_LIBADD = -lcups -lz -lpthread -lm
libSCPService_la_LIBADD += $(GLIB_LIBS)
libSCPService_la_LIBADD += $(GIO_LIBS)
libSCPService_la_LIBADD += $(GIOUNIX_LIBS)
libSCPService_la_LIBADD += $(GTK_LIBS)
libSCPService_la_LIBADD += $(CUPS_LIBS)
libSCPService_la_LIBADD += $(CUPS_TEMP)
libSCPService_la_LIBADD += $(LIB_AVAHI)

libSCPService_la_LDFLAGS = -I/usr/include/libxml2 -L/usr/lib -lavahi-common -lavahi-client

##INSTALL HEADERS
scp_service_headersdir = $(includedir)/scp-services-headers
scp_service_headers_HEADERS = nstring.h \
                              MissingExecutables.h \
                              asyncconn.h \
                              killtimer.h \
                              newprinter.h \
                              scp_interface.h \
                              PPDialog.h \
                              authinfocache.h \
                              asyncpk1.h \
                              asyncipp.h \
                              ConfigPrintingNewPrinterDialog.h \
                              ppdcache.h \
                              GroupPhysicalDevicesRequest.h \
                              ppds.h \
                              xml.h \
                              JobApplet.h
                              
include_HEADERS = SCPService.h
