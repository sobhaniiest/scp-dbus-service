#                                               -*- Autoconf -*- 
# Process this file with autoconf to produce a configure script. 
 
AC_PREREQ([2.69]) 
AC_INIT([scp-dbus-service],[0.01],[sobhaniiest@gmail.com],[https://github.com/sobhaniiest/scp-dbus-service]) 
AM_INIT_AUTOMAKE([-Wall -Werror foreign])  

: ${CFLAGS=""}

# Check for a C compiler 
AC_PROG_CC 

#Check for archiver
AM_PROG_AR

LT_INIT

LIBS="$LIBS -I/usr/include/libxml2 -L/usr/lib -lxml2 -lz -lcups"
AC_MSG_CHECKING(for libxml)
AC_TRY_LINK([#include <libxml/parser.h> #include <libxml/xmlmemory.h>], [xmlDocPtr doc; xmlNodePtr cur; xmlChar *key;], AC_MSG_RESULT(OK), AC_MSG_ERROR([***** Libxml is required to build scp-dbus-service; Available at http://www.libxml.org/]))

AC_ARG_WITH(gtk,
    AS_HELP_STRING([--with-gtk],
        [use GTK+ to determine mimetypes if available @<:@default=yes@:>@]),
    [use_gtk=$withval],
    [use_gtk=yes])

if test "$use_gtk" = "yes"; then
    if test "x$(which pkg-config)" != "x"; then
        if pkg-config --exists glib-2.0 gtk+-3.0; then
            CFLAGS="$CFLAGS $(pkg-config --cflags gio-2.0 gio-unix-2.0 glib-2.0 gtk+-3.0)"
            LIBS="$LIBS $(pkg-config --libs gio-2.0 gio-unix-2.0 glib-2.0 gtk+-3.0)"
            AC_CHECK_HEADER([gio/gio.h], AC_MSG_RESULT(OK), [AC_MSG_ERROR([gio/gio.h header not found.])], [[#include <gio/gio.h>]])
            AC_CHECK_HEADER([glib.h], AC_MSG_RESULT(OK), [AC_MSG_ERROR([glib.h header not found.])], [[#include <glib.h>]])
            AC_CHECK_HEADER([gtk/gtk.h], AC_MSG_RESULT(OK), [AC_MSG_ERROR([gtk/gtk.h header not found.])], [[#include <gtk/gtk.h>]])
            AC_DEFINE([HAVE_LIBGTK], [1], [use gtk to determine mime type])
        else
            AC_MSG_WARN("Could not find glib-2.0 gtk+-3.0 via pkg-config.  No GTK+ support.")
        fi
    else
        AC_MSG_WARN("Could not find pkg-config.  No GTK+ support.")
    fi
fi

PKG_CHECK_MODULES(XML, libxml-2.0 >= 2.4, AC_MSG_RESULT(OK), AC_MSG_ERROR([***** Libxml is required to build scp-dbus-service; Available at http://www.libxml.org/]))

PKG_CHECK_MODULES([GIO],[gio-2.0], AC_MSG_RESULT(OK), AC_MSG_ERROR([***** GIO-2.0 is required to build scp-dbus-service; Available at http:http://www.gtk.org/])) 

PKG_CHECK_MODULES([GIOUNIX],[gio-unix-2.0], AC_MSG_RESULT(OK), AC_MSG_ERROR([***** GIOUNIX-2.0 is required to build scp-dbus-service; Available at http://www.gtk.org/])) 

PKG_CHECK_MODULES([GLIB],[glib-2.0], AC_MSG_RESULT(OK), AC_MSG_ERROR([***** GLIB-2.0 is required to build scp-dbus-service; Available at http://www.gtk.org/])) 

PKG_CHECK_MODULES([GTK],[gtk+-3.0], AC_MSG_RESULT(OK), AC_MSG_ERROR([***** GTK+-3.0 is required to build scp-dbus-service; Available at http://www.gtk.org/]))

# Checks for header files. 
AC_CHECK_HEADERS([stdio.h stdlib.h string.h unistd.h fcntl.h])
AC_CHECK_HEADERS([stdbool.h math.h ctype.h])
AC_CHECK_HEADERS([pthread.h sys/types.h libgen.h locale.h])

# Checks for libraries.
AC_MSG_CHECKING(for cups)
AC_CHECK_HEADERS([cups/cups.h cups/ppd.h], AC_MSG_RESULT(OK), [AC_MSG_ERROR([cups header not found.])], [[#include <cups/cups.h> #include <cups/ppd.h>]])
AC_TRY_LINK([#include <cups/cups.h> #include <cups/ppd.h>], [http_encryption_t encryption;], AC_MSG_RESULT(OK), AC_MSG_ERROR([***** CUPS is required to build scp-dbus-service;]))

# Check for avahi
#PKG_CHECK_MODULES([libavahi],[avahi-client],[have_avahi=1],[have_avahi=0])
#echo "HAVE_AVAHI="$have_avahi
#if test x"$have_avahi" = x"1"; then
#    AC_CHECK_HEADERS([avahi-client/client.h avahi-client/lookup.h avahi-common/simple-watch.h\
#    avahi-common/malloc.h avahi-common/error.h],[],[have_avahi=0])
#fi
#AC_DEFINE_UNQUOTED([HAVE_AVAHI],[$have_avahi],[Install AVAHI!!!])

#if test x"$have_avahi" = x"1"; then
#    LIB_AVAHI="-lavahi-common -lavahi-client"
#    echo "$LIB_AVAHI"
#    AC_SUBST(LIB_AVAHI)
#fi

# Checks for typedefs, structures, and compiler characteristics. 
AC_TYPE_SIZE_T 
 
# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([getline strstr dup])

AC_CONFIG_FILES([Makefile
                 dbus/Makefile
                 SCPService.pc])

AC_OUTPUT