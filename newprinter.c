#include "newprinter.h"

void NewPrinter()
{

}

static void s_c_p(GtkWidget *widget, gpointer data)
{
    popen("system-config-printer","r");
}

static void activate(GtkApplication *app, char *user_data)
{
	  GtkWidget *window,
	            *grid,
	            *label,
	            *button;

	  window = gtk_application_window_new (app);
  	gtk_window_set_title (GTK_WINDOW (window), "Message");
  	gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  	grid = gtk_grid_new ();
  	gtk_container_add (GTK_CONTAINER (window), grid);

  	label = gtk_label_new(user_data);
  	gtk_grid_attach (GTK_GRID (grid), label, 0, 0, 2, 1);

    button = gtk_button_new_with_label ("system-config-printer");
    g_signal_connect_swapped (button, "clicked", G_CALLBACK (s_c_p), window);
    gtk_grid_attach (GTK_GRID (grid), button, 0, 1, 2, 1);

  	button = gtk_button_new_with_label ("Exit");
  	g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
  	gtk_grid_attach (GTK_GRID (grid), button, 0, 3, 2, 1);

  	gtk_widget_show_all (window);
}


bool init(char *dialog_mode,
    		  const char *device_uri,
    		  const char *name,
    		  FILE *ppd,
    		  const char *device_id,
    		  const char *host,
    		  http_encryption_t encryption,
    		  unsigned int xid)
{
  	if(!(strcmp(host,"\0")))
  		  host = cupsServer();

  	if(encryption == 0)
  		  encryption = cupsEncryption();

  	//Auth_Connection();

  	if(!(strcmp(dialog_mode, "printer_with_uri")))
  		  activate_NewPrinterFromDevice(device_uri, device_id);
  	else if(!(strcmp(dialog_mode, "ppd")))
  		  activate_ChangePPD(device_uri, device_id, name, ppd);
  	else if(!(strcmp(dialog_mode, "download_driver")))
  		  activate_DownloadDriverForDeviceID(device_id);

  	return true;
}

static int activate_NewPrinterFromDevice(const char *dev_uri, const char *devid)
{
  	GtkApplication *app;
  	int status;
    char buffer[1024];
  	snprintf(buffer, 1024, "\tWant to add New Printer from device\t\n\n\tDevice URI : %s\t\n\n\tDevice ID : %s\t\n",dev_uri, devid);
    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), (gchar *)buffer);
    status = g_application_run (G_APPLICATION (app), 0, NULL);
    g_object_unref (app);
    return status;
}

static int activate_DownloadDriverForDeviceID(const char *devid)
{
  	GtkApplication *app;
  	int status;
  	char buffer[1024];
  	snprintf(buffer, 1024, "\tWant to download Driver for the given Device ID\t\n\n\tDevice ID : %s\t\n", devid);
    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  	g_signal_connect (app, "activate", G_CALLBACK (activate), (gchar *)buffer);
  	status = g_application_run (G_APPLICATION (app), 0, NULL);
  	g_object_unref (app);
  	return status;
}

static int activate_ChangePPD(const char *dev_uri, const char *devid, const char *name, FILE *ppd)
{
    GtkApplication *app;
    int status;
    char buffer[1024];
    snprintf(buffer, 1024, "\tWant to Change PPD for the given\t\n\n\tDevice URI : %s\t\n\n\tDevice ID : %s\t\n\n\tPrinter/Queue Name : %s\t\n\n\t", dev_uri, devid, name);
    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), (gchar *)buffer);
    status = g_application_run (G_APPLICATION (app), 0, NULL);
    g_object_unref (app);
    return status;
}