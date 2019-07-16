#ifndef PPDIALOG_H
#define PPDIALOG_H

#include <glib.h>
#include <stdbool.h>
#include "killtimer.h"
#include "scp_interface.h"
#include "PPDialog.h"

#define PPDinterface scpinterfacePrinterPropertiesDialog

extern gulong ppd_finished;

void CPPrinterPropertiesDialog(GDBusConnection *connection,
                               gchar *path,
                               guint xid,
                               const gchar *name);

gboolean PrintTestPage(PPDinterface *interface,
                       GDBusMethodInvocation *invocation,
                       gpointer user_data);

gboolean on_dialog_closed(PPDinterface *interface,
                          GDBusMethodInvocation *invocation,
                          gpointer user_data);

#endif