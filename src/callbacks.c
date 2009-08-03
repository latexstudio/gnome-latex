#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <libintl.h>
#include <gtk/gtk.h>

#include "main.h"
#include "callbacks.h"
#include "error.h"

static void open_file (const gchar *file_name, GtkTextView *text_view);
static void save_as_dialog (widgets_t *widgets);
static void file_save (void);

void
cb_open (GtkAction *action, widgets_t *widgets)
{
	GtkWidget *dialog = gtk_file_chooser_dialog_new (
			_("Open File"),
			GTK_WINDOW (widgets->window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL
	);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

		docs.active->path = g_strdup (filename);
		docs.active->saved = TRUE;

		//TODO open the file in a new tab
		open_file (filename, docs.active->text_view);
		
		print_info ("Open file: \"%s\"", filename);

		g_free (filename);
	}

	gtk_widget_destroy (dialog);
}

void
cb_save (GtkAction *action, widgets_t *widgets)
{
	if (docs.active)
	{
		if (! docs.active->saved)
		{
			if (docs.active->path == NULL)
				save_as_dialog (widgets);

			file_save();
		}
	}

	else
		print_warning ("no document opened");
}

void
cb_save_as (GtkAction *action, widgets_t *widgets)
{
	if (docs.active)
	{
		document_t doc_backup = *docs.active;

		docs.active->path = NULL;
		docs.active->saved = FALSE;
		cb_save (action, widgets);

		// if the user click on cancel
		if (! docs.active->saved)
			(*docs.active) = doc_backup;
	}

	else
		print_warning ("no document opened");
}

void
cb_about_dialog (GtkAction *action, widgets_t *widgets)
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
			GTK_WINDOW (widgets->window),
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

void
cb_text_changed (GtkWidget *widget, gpointer user_data)
{
	if (docs.active)
		docs.active->saved = FALSE;
}

static void
open_file (const gchar *file_name, GtkTextView *text_view)
{
	// the arguments must not be NULL
	g_return_if_fail (file_name && text_view);

	gchar *contents = NULL;

	if (g_file_get_contents (file_name, &contents, NULL, NULL))
	{
		GtkTextBuffer *text_buffer = gtk_text_view_get_buffer (text_view);
		gchar *utf8 = g_locale_to_utf8 (contents, -1, NULL, NULL, NULL);
		gtk_text_buffer_set_text (text_buffer, utf8, -1);
		g_free (contents);
		g_free (utf8);
	}

	else
		print_warning ("impossible to open the file \"%s\"", file_name);
}

static void
save_as_dialog (widgets_t *widgets)
{
	GtkWidget *dialog = gtk_file_chooser_dialog_new (
			_("Save File"),
			GTK_WINDOW (widgets->window),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL
	);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
		docs.active->path = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

	gtk_widget_destroy (dialog);
}

static void
file_save (void)
{
	if (docs.active->path != NULL)
	{
		print_info ("save current buffer to \"%s\"", docs.active->path);

		FILE* file = fopen (docs.active->path, "w");
		if (file != NULL)
		{
			GtkTextBuffer *text_buffer = gtk_text_view_get_buffer (docs.active->text_view);
			GtkTextIter start;
			GtkTextIter end;
			gtk_text_buffer_get_bounds (text_buffer, &start, &end);
			gchar *contents = gtk_text_buffer_get_text (text_buffer, &start, &end, FALSE);
			gchar *locale = g_locale_from_utf8 (contents, -1, NULL, NULL, NULL);
			
			// write the contents into the file
			fprintf (file, "%s\n", locale);

			fclose (file);
			g_free (contents);
			g_free (locale);

			docs.active->saved = TRUE;
		}

		// error with fopen
		else
		{
			print_warning ("impossible to save the file \"%s\"",
					docs.active->path);
		}
	}
}
