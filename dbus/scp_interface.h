/*
 * Generated by gdbus-codegen 2.56.4. DO NOT EDIT.
 *
 * The license of this code is the same as for the D-Bus interface description
 * it was derived from.
 */

#ifndef __SCP_INTERFACE_H__
#define __SCP_INTERFACE_H__

#include <gio/gio.h>

G_BEGIN_DECLS


/* ------------------------------------------------------------------------ */
/* Declarations for org.fedoraproject.Config.Printing */

#define SCP_INTERFACE_TYPE_ (scp_interface__get_type ())
#define SCP_INTERFACE_(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), SCP_INTERFACE_TYPE_, scpinterface))
#define SCP_INTERFACE_IS_(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), SCP_INTERFACE_TYPE_))
#define SCP_INTERFACE__GET_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE ((o), SCP_INTERFACE_TYPE_, scpinterfaceIface))

struct _scpinterface;
typedef struct _scpinterface scpinterface;
typedef struct _scpinterfaceIface scpinterfaceIface;

struct _scpinterfaceIface
{
  GTypeInterface parent_iface;


  gboolean (*handle_get_best_drivers) (
    scpinterface *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_device_id,
    const gchar *arg_device_make_and_model,
    const gchar *arg_device_uri);

  gboolean (*handle_group_physical_devices) (
    scpinterface *object,
    GDBusMethodInvocation *invocation,
    GVariant *arg_devices);

  gboolean (*handle_job_applet) (
    scpinterface *object,
    GDBusMethodInvocation *invocation);

  gboolean (*handle_missing_executables) (
    scpinterface *object,
    GDBusMethodInvocation *invocation,
    const gchar *arg_ppd_filename);

  gboolean (*handle_new_printer_dialog) (
    scpinterface *object,
    GDBusMethodInvocation *invocation);

  gboolean (*handle_printer_properties_dialog) (
    scpinterface *object,
    GDBusMethodInvocation *invocation,
    guint arg_xid,
    const gchar *arg_name);

  void (*error) (
    scpinterface *object);

  void (*ready) (
    scpinterface *object);

};

GType scp_interface__get_type (void) G_GNUC_CONST;

GDBusInterfaceInfo *scp_interface__interface_info (void);
guint scp_interface__override_properties (GObjectClass *klass, guint property_id_begin);


/* D-Bus method call completion functions: */
void scp_interface__complete_new_printer_dialog (
    scpinterface *object,
    GDBusMethodInvocation *invocation,
    const gchar *path);

void scp_interface__complete_printer_properties_dialog (
    scpinterface *object,
    GDBusMethodInvocation *invocation,
    const gchar *path);

void scp_interface__complete_job_applet (
    scpinterface *object,
    GDBusMethodInvocation *invocation,
    const gchar *path);

void scp_interface__complete_get_best_drivers (
    scpinterface *object,
    GDBusMethodInvocation *invocation,
    GVariant *drivers);

void scp_interface__complete_missing_executables (
    scpinterface *object,
    GDBusMethodInvocation *invocation,
    const gchar *const *missing_executables);

void scp_interface__complete_group_physical_devices (
    scpinterface *object,
    GDBusMethodInvocation *invocation,
    GVariant *grouped_devices);



/* D-Bus signal emissions functions: */
void scp_interface__emit_ready (
    scpinterface *object);

void scp_interface__emit_error (
    scpinterface *object);



/* D-Bus method calls: */
void scp_interface__call_new_printer_dialog (
    scpinterface *proxy,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean scp_interface__call_new_printer_dialog_finish (
    scpinterface *proxy,
    gchar **out_path,
    GAsyncResult *res,
    GError **error);

gboolean scp_interface__call_new_printer_dialog_sync (
    scpinterface *proxy,
    gchar **out_path,
    GCancellable *cancellable,
    GError **error);

void scp_interface__call_printer_properties_dialog (
    scpinterface *proxy,
    guint arg_xid,
    const gchar *arg_name,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean scp_interface__call_printer_properties_dialog_finish (
    scpinterface *proxy,
    gchar **out_path,
    GAsyncResult *res,
    GError **error);

gboolean scp_interface__call_printer_properties_dialog_sync (
    scpinterface *proxy,
    guint arg_xid,
    const gchar *arg_name,
    gchar **out_path,
    GCancellable *cancellable,
    GError **error);

void scp_interface__call_job_applet (
    scpinterface *proxy,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean scp_interface__call_job_applet_finish (
    scpinterface *proxy,
    gchar **out_path,
    GAsyncResult *res,
    GError **error);

gboolean scp_interface__call_job_applet_sync (
    scpinterface *proxy,
    gchar **out_path,
    GCancellable *cancellable,
    GError **error);

void scp_interface__call_get_best_drivers (
    scpinterface *proxy,
    const gchar *arg_device_id,
    const gchar *arg_device_make_and_model,
    const gchar *arg_device_uri,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean scp_interface__call_get_best_drivers_finish (
    scpinterface *proxy,
    GVariant **out_drivers,
    GAsyncResult *res,
    GError **error);

gboolean scp_interface__call_get_best_drivers_sync (
    scpinterface *proxy,
    const gchar *arg_device_id,
    const gchar *arg_device_make_and_model,
    const gchar *arg_device_uri,
    GVariant **out_drivers,
    GCancellable *cancellable,
    GError **error);

void scp_interface__call_missing_executables (
    scpinterface *proxy,
    const gchar *arg_ppd_filename,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean scp_interface__call_missing_executables_finish (
    scpinterface *proxy,
    gchar ***out_missing_executables,
    GAsyncResult *res,
    GError **error);

gboolean scp_interface__call_missing_executables_sync (
    scpinterface *proxy,
    const gchar *arg_ppd_filename,
    gchar ***out_missing_executables,
    GCancellable *cancellable,
    GError **error);

void scp_interface__call_group_physical_devices (
    scpinterface *proxy,
    GVariant *arg_devices,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean scp_interface__call_group_physical_devices_finish (
    scpinterface *proxy,
    GVariant **out_grouped_devices,
    GAsyncResult *res,
    GError **error);

gboolean scp_interface__call_group_physical_devices_sync (
    scpinterface *proxy,
    GVariant *arg_devices,
    GVariant **out_grouped_devices,
    GCancellable *cancellable,
    GError **error);



/* ---- */

#define SCP_INTERFACE_TYPE__PROXY (scp_interface__proxy_get_type ())
#define SCP_INTERFACE__PROXY(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), SCP_INTERFACE_TYPE__PROXY, scpinterfaceProxy))
#define SCP_INTERFACE__PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), SCP_INTERFACE_TYPE__PROXY, scpinterfaceProxyClass))
#define SCP_INTERFACE__PROXY_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), SCP_INTERFACE_TYPE__PROXY, scpinterfaceProxyClass))
#define SCP_INTERFACE_IS__PROXY(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), SCP_INTERFACE_TYPE__PROXY))
#define SCP_INTERFACE_IS__PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SCP_INTERFACE_TYPE__PROXY))

typedef struct _scpinterfaceProxy scpinterfaceProxy;
typedef struct _scpinterfaceProxyClass scpinterfaceProxyClass;
typedef struct _scpinterfaceProxyPrivate scpinterfaceProxyPrivate;

struct _scpinterfaceProxy
{
  /*< private >*/
  GDBusProxy parent_instance;
  scpinterfaceProxyPrivate *priv;
};

struct _scpinterfaceProxyClass
{
  GDBusProxyClass parent_class;
};

GType scp_interface__proxy_get_type (void) G_GNUC_CONST;

#if GLIB_CHECK_VERSION(2, 44, 0)
G_DEFINE_AUTOPTR_CLEANUP_FUNC (scpinterfaceProxy, g_object_unref)
#endif

void scp_interface__proxy_new (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
scpinterface *scp_interface__proxy_new_finish (
    GAsyncResult        *res,
    GError             **error);
scpinterface *scp_interface__proxy_new_sync (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);

void scp_interface__proxy_new_for_bus (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
scpinterface *scp_interface__proxy_new_for_bus_finish (
    GAsyncResult        *res,
    GError             **error);
scpinterface *scp_interface__proxy_new_for_bus_sync (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);


/* ---- */

#define SCP_INTERFACE_TYPE__SKELETON (scp_interface__skeleton_get_type ())
#define SCP_INTERFACE__SKELETON(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), SCP_INTERFACE_TYPE__SKELETON, scpinterfaceSkeleton))
#define SCP_INTERFACE__SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), SCP_INTERFACE_TYPE__SKELETON, scpinterfaceSkeletonClass))
#define SCP_INTERFACE__SKELETON_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), SCP_INTERFACE_TYPE__SKELETON, scpinterfaceSkeletonClass))
#define SCP_INTERFACE_IS__SKELETON(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), SCP_INTERFACE_TYPE__SKELETON))
#define SCP_INTERFACE_IS__SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SCP_INTERFACE_TYPE__SKELETON))

typedef struct _scpinterfaceSkeleton scpinterfaceSkeleton;
typedef struct _scpinterfaceSkeletonClass scpinterfaceSkeletonClass;
typedef struct _scpinterfaceSkeletonPrivate scpinterfaceSkeletonPrivate;

struct _scpinterfaceSkeleton
{
  /*< private >*/
  GDBusInterfaceSkeleton parent_instance;
  scpinterfaceSkeletonPrivate *priv;
};

struct _scpinterfaceSkeletonClass
{
  GDBusInterfaceSkeletonClass parent_class;
};

GType scp_interface__skeleton_get_type (void) G_GNUC_CONST;

#if GLIB_CHECK_VERSION(2, 44, 0)
G_DEFINE_AUTOPTR_CLEANUP_FUNC (scpinterfaceSkeleton, g_object_unref)
#endif

scpinterface *scp_interface__skeleton_new (void);


/* ------------------------------------------------------------------------ */
/* Declarations for org.fedoraproject.Config.Printing.NewPrinterDialog */

#define SCP_INTERFACE_TYPE_NEW_PRINTER_DIALOG (scp_interface_new_printer_dialog_get_type ())
#define SCP_INTERFACE_NEW_PRINTER_DIALOG(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), SCP_INTERFACE_TYPE_NEW_PRINTER_DIALOG, scpinterfaceNewPrinterDialog))
#define SCP_INTERFACE_IS_NEW_PRINTER_DIALOG(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), SCP_INTERFACE_TYPE_NEW_PRINTER_DIALOG))
#define SCP_INTERFACE_NEW_PRINTER_DIALOG_GET_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE ((o), SCP_INTERFACE_TYPE_NEW_PRINTER_DIALOG, scpinterfaceNewPrinterDialogIface))

struct _scpinterfaceNewPrinterDialog;
typedef struct _scpinterfaceNewPrinterDialog scpinterfaceNewPrinterDialog;
typedef struct _scpinterfaceNewPrinterDialogIface scpinterfaceNewPrinterDialogIface;

struct _scpinterfaceNewPrinterDialogIface
{
  GTypeInterface parent_iface;


  gboolean (*handle_change_ppd) (
    scpinterfaceNewPrinterDialog *object,
    GDBusMethodInvocation *invocation,
    guint arg_xid,
    const gchar *arg_name,
    const gchar *arg_device_id);

  gboolean (*handle_download_driver_for_device_id) (
    scpinterfaceNewPrinterDialog *object,
    GDBusMethodInvocation *invocation,
    guint arg_xid,
    const gchar *arg_device_id);

  gboolean (*handle_new_printer_from_device) (
    scpinterfaceNewPrinterDialog *object,
    GDBusMethodInvocation *invocation,
    guint arg_xid,
    const gchar *arg_device_uri,
    const gchar *arg_device_id);

  void (*dialog_canceled) (
    scpinterfaceNewPrinterDialog *object);

  void (*driver_download_checked) (
    scpinterfaceNewPrinterDialog *object,
    const gchar *arg_name);

  void (*printer_added) (
    scpinterfaceNewPrinterDialog *object,
    const gchar *arg_name);

  void (*printer_modified) (
    scpinterfaceNewPrinterDialog *object,
    const gchar *arg_name,
    gboolean arg_ppd_has_changed);

};

GType scp_interface_new_printer_dialog_get_type (void) G_GNUC_CONST;

GDBusInterfaceInfo *scp_interface_new_printer_dialog_interface_info (void);
guint scp_interface_new_printer_dialog_override_properties (GObjectClass *klass, guint property_id_begin);


/* D-Bus method call completion functions: */
void scp_interface_new_printer_dialog_complete_new_printer_from_device (
    scpinterfaceNewPrinterDialog *object,
    GDBusMethodInvocation *invocation);

void scp_interface_new_printer_dialog_complete_download_driver_for_device_id (
    scpinterfaceNewPrinterDialog *object,
    GDBusMethodInvocation *invocation);

void scp_interface_new_printer_dialog_complete_change_ppd (
    scpinterfaceNewPrinterDialog *object,
    GDBusMethodInvocation *invocation);



/* D-Bus signal emissions functions: */
void scp_interface_new_printer_dialog_emit_dialog_canceled (
    scpinterfaceNewPrinterDialog *object);

void scp_interface_new_printer_dialog_emit_printer_added (
    scpinterfaceNewPrinterDialog *object,
    const gchar *arg_name);

void scp_interface_new_printer_dialog_emit_printer_modified (
    scpinterfaceNewPrinterDialog *object,
    const gchar *arg_name,
    gboolean arg_ppd_has_changed);

void scp_interface_new_printer_dialog_emit_driver_download_checked (
    scpinterfaceNewPrinterDialog *object,
    const gchar *arg_name);



/* D-Bus method calls: */
void scp_interface_new_printer_dialog_call_new_printer_from_device (
    scpinterfaceNewPrinterDialog *proxy,
    guint arg_xid,
    const gchar *arg_device_uri,
    const gchar *arg_device_id,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean scp_interface_new_printer_dialog_call_new_printer_from_device_finish (
    scpinterfaceNewPrinterDialog *proxy,
    GAsyncResult *res,
    GError **error);

gboolean scp_interface_new_printer_dialog_call_new_printer_from_device_sync (
    scpinterfaceNewPrinterDialog *proxy,
    guint arg_xid,
    const gchar *arg_device_uri,
    const gchar *arg_device_id,
    GCancellable *cancellable,
    GError **error);

void scp_interface_new_printer_dialog_call_download_driver_for_device_id (
    scpinterfaceNewPrinterDialog *proxy,
    guint arg_xid,
    const gchar *arg_device_id,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean scp_interface_new_printer_dialog_call_download_driver_for_device_id_finish (
    scpinterfaceNewPrinterDialog *proxy,
    GAsyncResult *res,
    GError **error);

gboolean scp_interface_new_printer_dialog_call_download_driver_for_device_id_sync (
    scpinterfaceNewPrinterDialog *proxy,
    guint arg_xid,
    const gchar *arg_device_id,
    GCancellable *cancellable,
    GError **error);

void scp_interface_new_printer_dialog_call_change_ppd (
    scpinterfaceNewPrinterDialog *proxy,
    guint arg_xid,
    const gchar *arg_name,
    const gchar *arg_device_id,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean scp_interface_new_printer_dialog_call_change_ppd_finish (
    scpinterfaceNewPrinterDialog *proxy,
    GAsyncResult *res,
    GError **error);

gboolean scp_interface_new_printer_dialog_call_change_ppd_sync (
    scpinterfaceNewPrinterDialog *proxy,
    guint arg_xid,
    const gchar *arg_name,
    const gchar *arg_device_id,
    GCancellable *cancellable,
    GError **error);



/* ---- */

#define SCP_INTERFACE_TYPE_NEW_PRINTER_DIALOG_PROXY (scp_interface_new_printer_dialog_proxy_get_type ())
#define SCP_INTERFACE_NEW_PRINTER_DIALOG_PROXY(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), SCP_INTERFACE_TYPE_NEW_PRINTER_DIALOG_PROXY, scpinterfaceNewPrinterDialogProxy))
#define SCP_INTERFACE_NEW_PRINTER_DIALOG_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), SCP_INTERFACE_TYPE_NEW_PRINTER_DIALOG_PROXY, scpinterfaceNewPrinterDialogProxyClass))
#define SCP_INTERFACE_NEW_PRINTER_DIALOG_PROXY_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), SCP_INTERFACE_TYPE_NEW_PRINTER_DIALOG_PROXY, scpinterfaceNewPrinterDialogProxyClass))
#define SCP_INTERFACE_IS_NEW_PRINTER_DIALOG_PROXY(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), SCP_INTERFACE_TYPE_NEW_PRINTER_DIALOG_PROXY))
#define SCP_INTERFACE_IS_NEW_PRINTER_DIALOG_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SCP_INTERFACE_TYPE_NEW_PRINTER_DIALOG_PROXY))

typedef struct _scpinterfaceNewPrinterDialogProxy scpinterfaceNewPrinterDialogProxy;
typedef struct _scpinterfaceNewPrinterDialogProxyClass scpinterfaceNewPrinterDialogProxyClass;
typedef struct _scpinterfaceNewPrinterDialogProxyPrivate scpinterfaceNewPrinterDialogProxyPrivate;

struct _scpinterfaceNewPrinterDialogProxy
{
  /*< private >*/
  GDBusProxy parent_instance;
  scpinterfaceNewPrinterDialogProxyPrivate *priv;
};

struct _scpinterfaceNewPrinterDialogProxyClass
{
  GDBusProxyClass parent_class;
};

GType scp_interface_new_printer_dialog_proxy_get_type (void) G_GNUC_CONST;

#if GLIB_CHECK_VERSION(2, 44, 0)
G_DEFINE_AUTOPTR_CLEANUP_FUNC (scpinterfaceNewPrinterDialogProxy, g_object_unref)
#endif

void scp_interface_new_printer_dialog_proxy_new (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
scpinterfaceNewPrinterDialog *scp_interface_new_printer_dialog_proxy_new_finish (
    GAsyncResult        *res,
    GError             **error);
scpinterfaceNewPrinterDialog *scp_interface_new_printer_dialog_proxy_new_sync (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);

void scp_interface_new_printer_dialog_proxy_new_for_bus (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
scpinterfaceNewPrinterDialog *scp_interface_new_printer_dialog_proxy_new_for_bus_finish (
    GAsyncResult        *res,
    GError             **error);
scpinterfaceNewPrinterDialog *scp_interface_new_printer_dialog_proxy_new_for_bus_sync (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);


/* ---- */

#define SCP_INTERFACE_TYPE_NEW_PRINTER_DIALOG_SKELETON (scp_interface_new_printer_dialog_skeleton_get_type ())
#define SCP_INTERFACE_NEW_PRINTER_DIALOG_SKELETON(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), SCP_INTERFACE_TYPE_NEW_PRINTER_DIALOG_SKELETON, scpinterfaceNewPrinterDialogSkeleton))
#define SCP_INTERFACE_NEW_PRINTER_DIALOG_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), SCP_INTERFACE_TYPE_NEW_PRINTER_DIALOG_SKELETON, scpinterfaceNewPrinterDialogSkeletonClass))
#define SCP_INTERFACE_NEW_PRINTER_DIALOG_SKELETON_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), SCP_INTERFACE_TYPE_NEW_PRINTER_DIALOG_SKELETON, scpinterfaceNewPrinterDialogSkeletonClass))
#define SCP_INTERFACE_IS_NEW_PRINTER_DIALOG_SKELETON(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), SCP_INTERFACE_TYPE_NEW_PRINTER_DIALOG_SKELETON))
#define SCP_INTERFACE_IS_NEW_PRINTER_DIALOG_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SCP_INTERFACE_TYPE_NEW_PRINTER_DIALOG_SKELETON))

typedef struct _scpinterfaceNewPrinterDialogSkeleton scpinterfaceNewPrinterDialogSkeleton;
typedef struct _scpinterfaceNewPrinterDialogSkeletonClass scpinterfaceNewPrinterDialogSkeletonClass;
typedef struct _scpinterfaceNewPrinterDialogSkeletonPrivate scpinterfaceNewPrinterDialogSkeletonPrivate;

struct _scpinterfaceNewPrinterDialogSkeleton
{
  /*< private >*/
  GDBusInterfaceSkeleton parent_instance;
  scpinterfaceNewPrinterDialogSkeletonPrivate *priv;
};

struct _scpinterfaceNewPrinterDialogSkeletonClass
{
  GDBusInterfaceSkeletonClass parent_class;
};

GType scp_interface_new_printer_dialog_skeleton_get_type (void) G_GNUC_CONST;

#if GLIB_CHECK_VERSION(2, 44, 0)
G_DEFINE_AUTOPTR_CLEANUP_FUNC (scpinterfaceNewPrinterDialogSkeleton, g_object_unref)
#endif

scpinterfaceNewPrinterDialog *scp_interface_new_printer_dialog_skeleton_new (void);


/* ------------------------------------------------------------------------ */
/* Declarations for org.fedoraproject.Config.Printing.PrinterPropertiesDialog */

#define SCP_INTERFACE_TYPE_PRINTER_PROPERTIES_DIALOG (scp_interface_printer_properties_dialog_get_type ())
#define SCP_INTERFACE_PRINTER_PROPERTIES_DIALOG(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), SCP_INTERFACE_TYPE_PRINTER_PROPERTIES_DIALOG, scpinterfacePrinterPropertiesDialog))
#define SCP_INTERFACE_IS_PRINTER_PROPERTIES_DIALOG(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), SCP_INTERFACE_TYPE_PRINTER_PROPERTIES_DIALOG))
#define SCP_INTERFACE_PRINTER_PROPERTIES_DIALOG_GET_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE ((o), SCP_INTERFACE_TYPE_PRINTER_PROPERTIES_DIALOG, scpinterfacePrinterPropertiesDialogIface))

struct _scpinterfacePrinterPropertiesDialog;
typedef struct _scpinterfacePrinterPropertiesDialog scpinterfacePrinterPropertiesDialog;
typedef struct _scpinterfacePrinterPropertiesDialogIface scpinterfacePrinterPropertiesDialogIface;

struct _scpinterfacePrinterPropertiesDialogIface
{
  GTypeInterface parent_iface;


  gboolean (*handle_print_test_page) (
    scpinterfacePrinterPropertiesDialog *object,
    GDBusMethodInvocation *invocation);

  void (*finished) (
    scpinterfacePrinterPropertiesDialog *object);

};

GType scp_interface_printer_properties_dialog_get_type (void) G_GNUC_CONST;

GDBusInterfaceInfo *scp_interface_printer_properties_dialog_interface_info (void);
guint scp_interface_printer_properties_dialog_override_properties (GObjectClass *klass, guint property_id_begin);


/* D-Bus method call completion functions: */
void scp_interface_printer_properties_dialog_complete_print_test_page (
    scpinterfacePrinterPropertiesDialog *object,
    GDBusMethodInvocation *invocation);



/* D-Bus signal emissions functions: */
void scp_interface_printer_properties_dialog_emit_finished (
    scpinterfacePrinterPropertiesDialog *object);



/* D-Bus method calls: */
void scp_interface_printer_properties_dialog_call_print_test_page (
    scpinterfacePrinterPropertiesDialog *proxy,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean scp_interface_printer_properties_dialog_call_print_test_page_finish (
    scpinterfacePrinterPropertiesDialog *proxy,
    GAsyncResult *res,
    GError **error);

gboolean scp_interface_printer_properties_dialog_call_print_test_page_sync (
    scpinterfacePrinterPropertiesDialog *proxy,
    GCancellable *cancellable,
    GError **error);



/* ---- */

#define SCP_INTERFACE_TYPE_PRINTER_PROPERTIES_DIALOG_PROXY (scp_interface_printer_properties_dialog_proxy_get_type ())
#define SCP_INTERFACE_PRINTER_PROPERTIES_DIALOG_PROXY(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), SCP_INTERFACE_TYPE_PRINTER_PROPERTIES_DIALOG_PROXY, scpinterfacePrinterPropertiesDialogProxy))
#define SCP_INTERFACE_PRINTER_PROPERTIES_DIALOG_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), SCP_INTERFACE_TYPE_PRINTER_PROPERTIES_DIALOG_PROXY, scpinterfacePrinterPropertiesDialogProxyClass))
#define SCP_INTERFACE_PRINTER_PROPERTIES_DIALOG_PROXY_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), SCP_INTERFACE_TYPE_PRINTER_PROPERTIES_DIALOG_PROXY, scpinterfacePrinterPropertiesDialogProxyClass))
#define SCP_INTERFACE_IS_PRINTER_PROPERTIES_DIALOG_PROXY(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), SCP_INTERFACE_TYPE_PRINTER_PROPERTIES_DIALOG_PROXY))
#define SCP_INTERFACE_IS_PRINTER_PROPERTIES_DIALOG_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SCP_INTERFACE_TYPE_PRINTER_PROPERTIES_DIALOG_PROXY))

typedef struct _scpinterfacePrinterPropertiesDialogProxy scpinterfacePrinterPropertiesDialogProxy;
typedef struct _scpinterfacePrinterPropertiesDialogProxyClass scpinterfacePrinterPropertiesDialogProxyClass;
typedef struct _scpinterfacePrinterPropertiesDialogProxyPrivate scpinterfacePrinterPropertiesDialogProxyPrivate;

struct _scpinterfacePrinterPropertiesDialogProxy
{
  /*< private >*/
  GDBusProxy parent_instance;
  scpinterfacePrinterPropertiesDialogProxyPrivate *priv;
};

struct _scpinterfacePrinterPropertiesDialogProxyClass
{
  GDBusProxyClass parent_class;
};

GType scp_interface_printer_properties_dialog_proxy_get_type (void) G_GNUC_CONST;

#if GLIB_CHECK_VERSION(2, 44, 0)
G_DEFINE_AUTOPTR_CLEANUP_FUNC (scpinterfacePrinterPropertiesDialogProxy, g_object_unref)
#endif

void scp_interface_printer_properties_dialog_proxy_new (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
scpinterfacePrinterPropertiesDialog *scp_interface_printer_properties_dialog_proxy_new_finish (
    GAsyncResult        *res,
    GError             **error);
scpinterfacePrinterPropertiesDialog *scp_interface_printer_properties_dialog_proxy_new_sync (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);

void scp_interface_printer_properties_dialog_proxy_new_for_bus (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
scpinterfacePrinterPropertiesDialog *scp_interface_printer_properties_dialog_proxy_new_for_bus_finish (
    GAsyncResult        *res,
    GError             **error);
scpinterfacePrinterPropertiesDialog *scp_interface_printer_properties_dialog_proxy_new_for_bus_sync (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);


/* ---- */

#define SCP_INTERFACE_TYPE_PRINTER_PROPERTIES_DIALOG_SKELETON (scp_interface_printer_properties_dialog_skeleton_get_type ())
#define SCP_INTERFACE_PRINTER_PROPERTIES_DIALOG_SKELETON(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), SCP_INTERFACE_TYPE_PRINTER_PROPERTIES_DIALOG_SKELETON, scpinterfacePrinterPropertiesDialogSkeleton))
#define SCP_INTERFACE_PRINTER_PROPERTIES_DIALOG_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), SCP_INTERFACE_TYPE_PRINTER_PROPERTIES_DIALOG_SKELETON, scpinterfacePrinterPropertiesDialogSkeletonClass))
#define SCP_INTERFACE_PRINTER_PROPERTIES_DIALOG_SKELETON_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), SCP_INTERFACE_TYPE_PRINTER_PROPERTIES_DIALOG_SKELETON, scpinterfacePrinterPropertiesDialogSkeletonClass))
#define SCP_INTERFACE_IS_PRINTER_PROPERTIES_DIALOG_SKELETON(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), SCP_INTERFACE_TYPE_PRINTER_PROPERTIES_DIALOG_SKELETON))
#define SCP_INTERFACE_IS_PRINTER_PROPERTIES_DIALOG_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SCP_INTERFACE_TYPE_PRINTER_PROPERTIES_DIALOG_SKELETON))

typedef struct _scpinterfacePrinterPropertiesDialogSkeleton scpinterfacePrinterPropertiesDialogSkeleton;
typedef struct _scpinterfacePrinterPropertiesDialogSkeletonClass scpinterfacePrinterPropertiesDialogSkeletonClass;
typedef struct _scpinterfacePrinterPropertiesDialogSkeletonPrivate scpinterfacePrinterPropertiesDialogSkeletonPrivate;

struct _scpinterfacePrinterPropertiesDialogSkeleton
{
  /*< private >*/
  GDBusInterfaceSkeleton parent_instance;
  scpinterfacePrinterPropertiesDialogSkeletonPrivate *priv;
};

struct _scpinterfacePrinterPropertiesDialogSkeletonClass
{
  GDBusInterfaceSkeletonClass parent_class;
};

GType scp_interface_printer_properties_dialog_skeleton_get_type (void) G_GNUC_CONST;

#if GLIB_CHECK_VERSION(2, 44, 0)
G_DEFINE_AUTOPTR_CLEANUP_FUNC (scpinterfacePrinterPropertiesDialogSkeleton, g_object_unref)
#endif

scpinterfacePrinterPropertiesDialog *scp_interface_printer_properties_dialog_skeleton_new (void);


/* ------------------------------------------------------------------------ */
/* Declarations for org.fedoraproject.Config.Printing.JobApplet */

#define SCP_INTERFACE_TYPE_JOB_APPLET (scp_interface_job_applet_get_type ())
#define SCP_INTERFACE_JOB_APPLET(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), SCP_INTERFACE_TYPE_JOB_APPLET, scpinterfaceJobApplet))
#define SCP_INTERFACE_IS_JOB_APPLET(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), SCP_INTERFACE_TYPE_JOB_APPLET))
#define SCP_INTERFACE_JOB_APPLET_GET_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE ((o), SCP_INTERFACE_TYPE_JOB_APPLET, scpinterfaceJobAppletIface))

struct _scpinterfaceJobApplet;
typedef struct _scpinterfaceJobApplet scpinterfaceJobApplet;
typedef struct _scpinterfaceJobAppletIface scpinterfaceJobAppletIface;

struct _scpinterfaceJobAppletIface
{
  GTypeInterface parent_iface;


  gboolean (*handle_quit) (
    scpinterfaceJobApplet *object,
    GDBusMethodInvocation *invocation);

  void (*finished) (
    scpinterfaceJobApplet *object);

};

GType scp_interface_job_applet_get_type (void) G_GNUC_CONST;

GDBusInterfaceInfo *scp_interface_job_applet_interface_info (void);
guint scp_interface_job_applet_override_properties (GObjectClass *klass, guint property_id_begin);


/* D-Bus method call completion functions: */
void scp_interface_job_applet_complete_quit (
    scpinterfaceJobApplet *object,
    GDBusMethodInvocation *invocation);



/* D-Bus signal emissions functions: */
void scp_interface_job_applet_emit_finished (
    scpinterfaceJobApplet *object);



/* D-Bus method calls: */
void scp_interface_job_applet_call_quit (
    scpinterfaceJobApplet *proxy,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean scp_interface_job_applet_call_quit_finish (
    scpinterfaceJobApplet *proxy,
    GAsyncResult *res,
    GError **error);

gboolean scp_interface_job_applet_call_quit_sync (
    scpinterfaceJobApplet *proxy,
    GCancellable *cancellable,
    GError **error);



/* ---- */

#define SCP_INTERFACE_TYPE_JOB_APPLET_PROXY (scp_interface_job_applet_proxy_get_type ())
#define SCP_INTERFACE_JOB_APPLET_PROXY(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), SCP_INTERFACE_TYPE_JOB_APPLET_PROXY, scpinterfaceJobAppletProxy))
#define SCP_INTERFACE_JOB_APPLET_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), SCP_INTERFACE_TYPE_JOB_APPLET_PROXY, scpinterfaceJobAppletProxyClass))
#define SCP_INTERFACE_JOB_APPLET_PROXY_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), SCP_INTERFACE_TYPE_JOB_APPLET_PROXY, scpinterfaceJobAppletProxyClass))
#define SCP_INTERFACE_IS_JOB_APPLET_PROXY(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), SCP_INTERFACE_TYPE_JOB_APPLET_PROXY))
#define SCP_INTERFACE_IS_JOB_APPLET_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SCP_INTERFACE_TYPE_JOB_APPLET_PROXY))

typedef struct _scpinterfaceJobAppletProxy scpinterfaceJobAppletProxy;
typedef struct _scpinterfaceJobAppletProxyClass scpinterfaceJobAppletProxyClass;
typedef struct _scpinterfaceJobAppletProxyPrivate scpinterfaceJobAppletProxyPrivate;

struct _scpinterfaceJobAppletProxy
{
  /*< private >*/
  GDBusProxy parent_instance;
  scpinterfaceJobAppletProxyPrivate *priv;
};

struct _scpinterfaceJobAppletProxyClass
{
  GDBusProxyClass parent_class;
};

GType scp_interface_job_applet_proxy_get_type (void) G_GNUC_CONST;

#if GLIB_CHECK_VERSION(2, 44, 0)
G_DEFINE_AUTOPTR_CLEANUP_FUNC (scpinterfaceJobAppletProxy, g_object_unref)
#endif

void scp_interface_job_applet_proxy_new (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
scpinterfaceJobApplet *scp_interface_job_applet_proxy_new_finish (
    GAsyncResult        *res,
    GError             **error);
scpinterfaceJobApplet *scp_interface_job_applet_proxy_new_sync (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);

void scp_interface_job_applet_proxy_new_for_bus (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
scpinterfaceJobApplet *scp_interface_job_applet_proxy_new_for_bus_finish (
    GAsyncResult        *res,
    GError             **error);
scpinterfaceJobApplet *scp_interface_job_applet_proxy_new_for_bus_sync (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);


/* ---- */

#define SCP_INTERFACE_TYPE_JOB_APPLET_SKELETON (scp_interface_job_applet_skeleton_get_type ())
#define SCP_INTERFACE_JOB_APPLET_SKELETON(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), SCP_INTERFACE_TYPE_JOB_APPLET_SKELETON, scpinterfaceJobAppletSkeleton))
#define SCP_INTERFACE_JOB_APPLET_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), SCP_INTERFACE_TYPE_JOB_APPLET_SKELETON, scpinterfaceJobAppletSkeletonClass))
#define SCP_INTERFACE_JOB_APPLET_SKELETON_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), SCP_INTERFACE_TYPE_JOB_APPLET_SKELETON, scpinterfaceJobAppletSkeletonClass))
#define SCP_INTERFACE_IS_JOB_APPLET_SKELETON(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), SCP_INTERFACE_TYPE_JOB_APPLET_SKELETON))
#define SCP_INTERFACE_IS_JOB_APPLET_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SCP_INTERFACE_TYPE_JOB_APPLET_SKELETON))

typedef struct _scpinterfaceJobAppletSkeleton scpinterfaceJobAppletSkeleton;
typedef struct _scpinterfaceJobAppletSkeletonClass scpinterfaceJobAppletSkeletonClass;
typedef struct _scpinterfaceJobAppletSkeletonPrivate scpinterfaceJobAppletSkeletonPrivate;

struct _scpinterfaceJobAppletSkeleton
{
  /*< private >*/
  GDBusInterfaceSkeleton parent_instance;
  scpinterfaceJobAppletSkeletonPrivate *priv;
};

struct _scpinterfaceJobAppletSkeletonClass
{
  GDBusInterfaceSkeletonClass parent_class;
};

GType scp_interface_job_applet_skeleton_get_type (void) G_GNUC_CONST;

#if GLIB_CHECK_VERSION(2, 44, 0)
G_DEFINE_AUTOPTR_CLEANUP_FUNC (scpinterfaceJobAppletSkeleton, g_object_unref)
#endif

scpinterfaceJobApplet *scp_interface_job_applet_skeleton_new (void);


G_END_DECLS

#endif /* __SCP_INTERFACE_H__ */
