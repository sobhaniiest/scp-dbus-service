#ifndef PPDIALOG_H
#define PPDIALOG_H

#include <glib.h>
#include <gio/gio.h>
#include <gtk/gtk.h> 
#include <stdbool.h>
#include <cups/cups.h>
#include "killtimer.h"
#include "newprinter.h"
#include "scp_interface.h"
#include "PPDialog.h"

#define PPDinterface scpinterfacePrinterPropertiesDialog

extern gulong ppd_finished;

typedef struct _PPDialog_printer
{
    bool status;
    const char *name;
}PPDialog_printer;

void CPPrinterPropertiesDialog(GDBusConnection *connection,
                               gchar *path,
                               guint xid,
                               const gchar *name,
                               GHashTable *uri);

gboolean PrintTestPage(PPDinterface *interface,
                       GDBusMethodInvocation *invocation,
                       PPDialog_printer *data);

gboolean on_dialog_closed(PPDinterface *interface,
                          GDBusMethodInvocation *invocation,
                          gpointer user_data);

#endif