#ifndef JOBAPPLET_H
#define JOBAPPLET_H

#include <glib.h>
#include <stdbool.h>
#include "killtimer.h"
#include "scp_interface.h"
#include "JobApplet.h"

#define JAinterface scpinterfaceJobApplet

extern gulong finished_id;
extern bool has_finished;

void CPJobApplet(GDBusConnection *connection, gchar *path);

gboolean Quit(JAinterface *interface,
              GDBusMethodInvocation *invocation,
              gpointer user_data);

void Finished();

void on_dialog_closed(JAinterface *interface);

#endif