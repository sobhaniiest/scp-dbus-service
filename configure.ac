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

PKG_CHECK_MODULES(XML, libxml-2.0 >= 2.4)
PKG_CHECK_MODULES([GIO],[gio-2.0]) 
PKG_CHECK_MODULES([GIOUNIX],[gio-unix-2.0]) 
PKG_CHECK_MODULES([GLIB],[glib-2.0]) 
PKG_CHECK_MODULES([GTK],[gtk+-3.0])

# Checks for libraries.
AC_CHECK_LIB([cups])

# Checks for header files. 
AC_CHECK_HEADERS([stdio.h stdlib.h string.h unistd.h fcntl.h])
AC_CHECK_HEADERS([cups/cups.h stdbool.h cups/ppd.h ctype.h])
AC_CHECK_HEADERS([pthread.h sys/types.h libgen.h locale.h])
AC_CHECK_HEADERS([libxml/parser.h libxml/xmlmemory.h regex.h])
AC_CHECK_HEADERS([gio/gio.h glib.h math.h gtk/gtk.h])

# Check for avahi
#PKG_CHECK_MODULES([libavahi],[avahi-client],[have_avahi=1],[have_avahi=0])
# echo "HAVE_AVAHI="$have_avahi
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
                 dbus/Makefile])

AC_OUTPUT