#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <libintl.h>
#include <gtk/gtk.h>

#include "main.h"
#include "callbacks.h"
#include "error.h"

static void create_document_in_new_tab (gchar *path, gchar *text, GtkWidget *label);
static void save_as_dialog (void);
static void file_save (void);
static gboolean close_all (void);

void
cb_new (void)
{
	char *default_text = "\\documentclass[a4paper,11pt]{article}\n"
		"\\begin{document}\n"
		"\\end{document}";
	GtkWidget *label = gtk_label_new (_("New document"));

	create_document_in_new_tab (NULL, default_text, label);
}

void
cb_open (void)
{
	GtkWidget *dialog = gtk_file_chooser_dialog_new (
			_("Open File"),
			docs.main_window,
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL
	);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		gchar *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		print_info ("Open file: \"%s\"", filename);
		gchar *contents = NULL;
		if (g_file_get_contents (filename, &contents, NULL, NULL))
		{
			// convert the text to UTF-8
			gchar *text_utf8 = g_locale_to_utf8 (contents, -1, NULL, NULL, NULL);

			GtkWidget *label = gtk_label_new (g_path_get_basename (filename));
			create_document_in_new_tab (filename, text_utf8, label);
			g_free (contents);
			g_free (text_utf8);
		}
		else
			print_warning ("impossible to open the file \"%s\"", filename);

		g_free (filename);
	}
	gtk_widget_destroy (dialog);
}

void
cb_save (void)
{
	if (docs.active != NULL)
	{
		if (! docs.active->saved)
		{
			if (docs.active->path == NULL)
				save_as_dialog ();

			file_save();
		}
	}

	else
		print_warning ("no document opened");
}

void
cb_save_as (void)
{
	if (docs.active != NULL)
	{
		document_t doc_backup = *docs.active;

		docs.active->path = NULL;
		docs.active->saved = FALSE;
		cb_save ();

		// if the user click on cancel
		if (! docs.active->saved)
			(*docs.active) = doc_backup;
	}

	else
		print_warning ("no document opened");
}

void
cb_close (void)
{
	if (docs.active != NULL)
	{
		/* if the document is not saved, ask the user for saving changes before
		 * closing */
		if (! docs.active->saved)
		{
			GtkWidget *dialog = gtk_dialog_new_with_buttons (
					_("Close the document"),
					docs.main_window,
					GTK_DIALOG_MODAL,
					GTK_STOCK_YES, GTK_RESPONSE_YES,
					GTK_STOCK_NO, GTK_RESPONSE_NO,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					NULL
			);
			GtkWidget *label = gtk_label_new (_("Save changes to document before closing?"));
			gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), label, TRUE, FALSE, 10);
			gtk_widget_show_all (GTK_DIALOG (dialog)->vbox);

			switch (gtk_dialog_run (GTK_DIALOG (dialog)))
			{
				// save changes before closing
				case GTK_RESPONSE_YES:
					cb_save ();
					break;

				// close the document without saving changes
				case GTK_RESPONSE_NO:
					break;
				
				// do nothing, the document is not closed
				case GTK_RESPONSE_CANCEL:
					gtk_widget_destroy (dialog);
					return;
			}

			gtk_widget_destroy (dialog);
		}

		/* close the tab */
		docs.all = g_list_remove (docs.all, docs.active);
		g_free (docs.active->path);
		g_free (docs.active);
		gtk_notebook_remove_page (docs.notebook, gtk_notebook_get_current_page (docs.notebook));
		if (gtk_notebook_get_n_pages (docs.notebook) > 0)
			docs.active = g_list_nth_data (docs.all, gtk_notebook_get_current_page (docs.notebook));
		else
			docs.active = NULL;
	}

	else
		print_warning ("no document opened");
}

void
cb_quit (void)
{
	if (close_all ())
	{
		print_info ("Bye bye");
		gtk_main_quit ();
	}
}

gboolean
cb_delete_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	cb_quit ();

	// the destroy signal is not emitted
	return TRUE;
}

void
cb_about_dialog (void)
{
	gchar *comments = _(PROGRAM_NAME " is a LaTeX development environment for the GNOME Desktop");
	gchar *copyright = "Copyright © 2009 Sébastien Wilmet";

	//TODO show the appropriate text for the GPL 3 licence
	gchar *licence = "GNU General Public Licence version 3 or later";

	//TODO set the url hook
	gchar *website = "http://latexila.sourceforge.net/";

	const gchar * const authors[] =
	{
		"Sébastien Wilmet <sebastien.wilmet@gmail.com>",
		NULL
	};

	gtk_show_about_dialog (
			docs.main_window,
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
cb_text_changed (GtkWidget *widget, gpointer user_data)
{
	if (docs.active != NULL)
		docs.active->saved = FALSE;
}

void
cb_page_change (GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data)
{
	docs.active = g_list_nth_data (docs.all, page_num);
	print_info ("switch-page: %d (%s)", page_num, docs.active->path);
}

/*****************************
 * local functions
 *****************************/

static void
create_document_in_new_tab (gchar *path, gchar *text, GtkWidget *label)
{
	// create a new document_t structure
	// if path = NULL, this is a new document
	// else, the user opened a document
	document_t *new_doc = g_malloc (sizeof (document_t));
	new_doc->path = path;
	new_doc->saved = (path == NULL) ? FALSE : TRUE;
	new_doc->text_view = GTK_TEXT_VIEW (gtk_text_view_new ());

	docs.all = g_list_append (docs.all, new_doc);
	docs.active = new_doc;

	// put the text into the buffer
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer (new_doc->text_view);
	gtk_text_buffer_set_text (text_buffer, text, -1);

	// when the text is modified
	g_signal_connect (G_OBJECT (text_buffer), "changed",
			G_CALLBACK (cb_text_changed), NULL);

	// with a scrollbar
	GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (sw), GTK_WIDGET (new_doc->text_view));
	gtk_widget_show_all (sw);

	// add the new document in a new tab
	gint index = gtk_notebook_append_page (docs.notebook, sw, label);
	gtk_notebook_set_current_page (docs.notebook, index);
}

static void
save_as_dialog (void)
{
	GtkWidget *dialog = gtk_file_chooser_dialog_new (
			_("Save File"),
			docs.main_window,
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

static gboolean
close_all (void)
{
	while (docs.active != NULL)
	{
		int tmp = gtk_notebook_get_n_pages (docs.notebook);
		gtk_notebook_set_current_page (docs.notebook, 0);
		cb_close ();
		if (gtk_notebook_get_n_pages (docs.notebook) >= tmp)
			return FALSE;
	}

	return TRUE;
}
