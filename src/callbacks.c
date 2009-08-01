#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <libintl.h>
#include <gtk/gtk.h>

#include "main.h"
#include "callbacks.h"
#include "error.h"

void
cb_open (void)
{
	GtkWidget *dialog = gtk_file_chooser_dialog_new (
			_("Open File"),
			NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL
	);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

		//TODO open the file in a new tab
		//open_file (filename);
		
		print_info ("Open file: \"%s\"", filename);
		g_free (filename);
	}

	gtk_widget_destroy (dialog);
}

void
cb_about_dialog (void)
{
	gchar *comments = _(PROGRAM_NAME " is a LaTeX development environment for the GNOME Desktop");
	gchar *copyright = "Copyright © 2009 Sébastien Wilmet";

	//TODO show the appropriate text for the GPL 3 licence
	gchar *licence = "GPL 3 or later";

	//TODO set the url hook
	gchar *website = "http://latexila.sourceforge.net/";

	const gchar * const authors[] =
	{
		"Sébastien Wilmet <sebastien.wilmet@gmail.com>",
		NULL
	};

	gtk_show_about_dialog (
			NULL,
			"program-name", PROGRAM_NAME,
			"authors", authors,
			"comments", comments,
			"copyright", copyright,
			"license", licence,
			"version", PROGRAM_VERSION,
			"title", _("About " PROGRAM_NAME),
			"translator-credits", _("translator-credits"),
			"website", website,
			NULL
	);
}

void
cb_quit (void)
{
	print_info ("Bye bye");
	gtk_main_quit ();
}
