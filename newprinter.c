#include "newprinter.h"

void NewPrinter()
{

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
  	gtk_grid_attach (GTK_GRID (grid), label, 0, 0, 1, 1);

  	button = gtk_button_new_with_label ("Exit");
  	g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
  	gtk_grid_attach (GTK_GRID (grid), button, 0, 1, 2, 1);

  	gtk_widget_show_all (window);
}


bool init(char *dialog_mode,
		  char *device_uri,
		  char *name,
		  char *ppd,
		  char *device_id,
		  char *host,
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
	//else if(!(strcmp(dialog_mode, "ppd")))
		//initialisePPDMode();
	else if(!(strcmp(dialog_mode, "download_driver")))
		activate_DownloadDriverForDeviceID(device_id);

	return true;
}

static int activate_NewPrinterFromDevice(char *dev_uri, char *devid)
{
	GtkApplication *app;
	int status;
	const char buffer[1024];
	snprintf(buffer, 1024, "Device URI : %s\nDevice ID : %s.",dev_uri, devid);
  	app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  	g_signal_connect (app, "activate", G_CALLBACK (activate), (gchar *)buffer);
  	status = g_application_run (G_APPLICATION (app), 0, NULL);
  	g_object_unref (app);
  	return status;
}

static int activate_DownloadDriverForDeviceID(char *devid)
{
	GtkApplication *app;
	int status;
	char buffer[1024];
	snprintf(buffer, 1024, "Device ID : %s.", devid);
  	app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  	g_signal_connect (app, "activate", G_CALLBACK (activate), (gchar *)buffer);
  	status = g_application_run (G_APPLICATION (app), 0, NULL);
  	g_object_unref (app);
  	return status;
}