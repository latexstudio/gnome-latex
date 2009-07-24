#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <gtk/gtk.h>

int
main (int argc, char *argv[])
{
	GtkWidget *window;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(window), "destroy",
			G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(window), "delete_event",
			G_CALLBACK(gtk_main_quit), NULL);
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	gtk_widget_show(window);

	gtk_main();

	return EXIT_SUCCESS;
}
