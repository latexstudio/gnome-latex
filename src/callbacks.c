#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <libintl.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagemanager.h>

#include "main.h"
#include "callbacks.h"
#include "error.h"

static void create_document_in_new_tab (gchar *path, gchar *text, GtkWidget *label);
static void save_as_dialog (void);
static void file_save (void);
static gboolean close_all (void);
static void set_title (void);
static void set_undo_redo_sensitivity (void);
static void run_compilation (gchar *command);
static void view_document (gchar *doc_type, gchar *doc_extension);

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

			file_save ();
			set_title ();
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
		if (docs.active->path != NULL)
		{
			print_info ("close the file \"%s\"", docs.active->path);
			g_free (docs.active->path);
		}
		g_free (docs.active);
		gtk_notebook_remove_page (docs.notebook, gtk_notebook_get_current_page (docs.notebook));
		if (gtk_notebook_get_n_pages (docs.notebook) > 0)
			docs.active = g_list_nth_data (docs.all, gtk_notebook_get_current_page (docs.notebook));
		else
			docs.active = NULL;

		set_undo_redo_sensitivity ();
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

void
cb_undo (void)
{
	if (gtk_source_buffer_can_undo (docs.active->source_buffer))
		gtk_source_buffer_undo (docs.active->source_buffer);

	set_undo_redo_sensitivity ();
}

void
cb_redo (void)
{
	if (gtk_source_buffer_can_redo (docs.active->source_buffer))
		gtk_source_buffer_redo (docs.active->source_buffer);

	set_undo_redo_sensitivity ();
}

gboolean
cb_delete_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	cb_quit ();

	// the destroy signal is not emitted
	return TRUE;
}

void
cb_line_numbers (GtkToggleAction *action, gpointer user_data)
{
	gboolean show = gtk_toggle_action_get_active (action);

	//TODO optimisation?
	guint nb_docs = g_list_length (docs.all);
	for (guint i = 0 ; i < nb_docs ; i++)
	{
		document_t *doc = g_list_nth_data (docs.all, i);
		gtk_source_view_set_show_line_numbers (
				GTK_SOURCE_VIEW (doc->source_view), show);
	}
}

void
cb_latex (void)
{
	run_compilation ("latex -interaction=nonstopmode");
}

void
cb_pdflatex (void)
{
	run_compilation ("pdflatex -interaction=nonstopmode");
}

void
cb_view_dvi (void)
{
	view_document ("DVI", ".dvi");
}

void
cb_view_pdf (void)
{
	view_document ("PDF", ".pdf");
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
	{
		docs.active->saved = FALSE;
		set_title ();
		set_undo_redo_sensitivity ();
	}
}

void
cb_page_change (GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data)
{
	docs.active = g_list_nth_data (docs.all, page_num);
	set_undo_redo_sensitivity ();
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
	new_doc->path = g_strdup (path);
	//new_doc->saved = (path == NULL) ? FALSE : TRUE;
	new_doc->saved = TRUE;
	new_doc->source_buffer = gtk_source_buffer_new (NULL);
	new_doc->source_view = gtk_source_view_new_with_buffer (new_doc->source_buffer);

	docs.all = g_list_append (docs.all, new_doc);
	docs.active = new_doc;

	// set the language for the syntaxic color
	if (path != NULL)
	{
		GtkSourceLanguage *lang = gtk_source_language_manager_guess_language (
				docs.lm, path, NULL);
		if (lang != NULL)
			gtk_source_buffer_set_language (new_doc->source_buffer, lang);
		else
			print_info ("language of the file unknown ==> no syntaxic color");
	}

	// the default file is a LaTeX document
	else
	{
		GtkSourceLanguage *lang = gtk_source_language_manager_get_language (
				docs.lm, "latex");
		gtk_source_buffer_set_language (new_doc->source_buffer, lang);
	}

	// set auto indentation
	gtk_source_view_set_auto_indent (GTK_SOURCE_VIEW (new_doc->source_view), TRUE);

	// set the font
	PangoFontDescription *font_desc = pango_font_description_from_string (FONT);
	gtk_widget_modify_font (new_doc->source_view, font_desc);

	// put the text into the buffer
	gtk_source_buffer_begin_not_undoable_action (new_doc->source_buffer);
	gtk_text_buffer_set_text (GTK_TEXT_BUFFER (new_doc->source_buffer), text, -1);
	gtk_source_buffer_end_not_undoable_action (new_doc->source_buffer);

	// when the text is modified
	g_signal_connect (G_OBJECT (new_doc->source_buffer), "changed",
			G_CALLBACK (cb_text_changed), NULL);

	// with a scrollbar
	GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (sw), GTK_WIDGET (new_doc->source_view));
	gtk_widget_show_all (sw);

	// add the new document in a new tab
	gint index = gtk_notebook_append_page (docs.notebook, sw, label);
	gtk_notebook_set_current_page (docs.notebook, index);

	set_undo_redo_sensitivity ();
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
			GtkTextBuffer *text_buffer = GTK_TEXT_BUFFER (docs.active->source_buffer);
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

static void
set_title (void)
{
	if (docs.active)
	{
		gchar *tmp;
		gchar *title;

		if (docs.active->path != NULL)
			tmp = g_path_get_basename (docs.active->path);
		else
			tmp = g_strdup (_("New document"));

		if (docs.active->saved)
			title = g_strdup (tmp);
		else
			title = g_strdup_printf ("*%s", tmp);

		g_free (tmp);

		gint index = gtk_notebook_get_current_page (docs.notebook);
		GtkWidget *child = gtk_notebook_get_nth_page (docs.notebook, index);
		GtkLabel *label = GTK_LABEL (gtk_notebook_get_tab_label (docs.notebook, child));
		gtk_label_set_text (label, title);

		g_free (title);
	}
}

static void
set_undo_redo_sensitivity (void)
{
	gboolean can_undo = FALSE;
	gboolean can_redo = FALSE;

	if (docs.active != NULL)
	{
		can_undo = gtk_source_buffer_can_undo (docs.active->source_buffer);
		can_redo = gtk_source_buffer_can_redo (docs.active->source_buffer);
	}

	gtk_action_set_sensitive (docs.undo, can_undo);
	gtk_action_set_sensitive (docs.redo, can_redo);
}

static void
run_compilation (gchar *command)
{
	if (docs.active != NULL)
	{
		GtkTextBuffer *log_buffer = gtk_text_view_get_buffer (docs.log);

		// the current document is a *.tex file?
		gboolean tex_file = g_regex_match_simple ("\\.tex$", docs.active->path, 0, 0);
		if (! tex_file)
		{
			gchar *text = g_strdup_printf (_("compilation failed: %s is not a *.tex file"),
					g_path_get_basename (docs.active->path));

			gtk_text_buffer_set_text (log_buffer, text, -1);
			g_free (text);
			return;
		}

		// run the command in the directory where the .tex is
		gchar *dir_backup = g_get_current_dir ();
		gchar *dir = g_path_get_dirname (docs.active->path);
		g_chdir (dir);
		gchar *full_command = g_strdup_printf ("%s %s", command, docs.active->path);
		gchar *output;
		GError *error = NULL;

		print_info ("execution of the command: %s", full_command);
		
		g_spawn_command_line_sync (full_command, &output, NULL, NULL, &error);
		g_chdir (dir_backup);
		
		// an error occured
		if (error != NULL)
		{
			print_warning ("execution failed: %s", error->message);
			g_error_free (error);
		}

		// show the message of the command
		else
			gtk_text_buffer_set_text (log_buffer, output, -1);

		g_free (dir_backup);
		g_free (dir);
		g_free (full_command);
		g_free (output);
	}
}

static void
view_document (gchar *doc_type, gchar *doc_extension)
{
	if (docs.active != NULL)
	{
		GtkTextBuffer *log_buffer = gtk_text_view_get_buffer (docs.log);

		GError *error = NULL;
		GRegex *regex = g_regex_new ("\\.tex$", 0, 0, NULL);

		// the current document is a *.tex file?
		gboolean tex_file = g_regex_match (regex, docs.active->path, 0, NULL);
		if (! tex_file)
		{
			gchar *text = g_strdup_printf (_("view %s failed: %s is not a *.tex file"),
					doc_type, g_path_get_basename (docs.active->path));

			gtk_text_buffer_set_text (log_buffer, text, -1);
			g_free (text);
			g_regex_unref (regex);
			return;
		}

		// replace .tex by .pdf
		gchar *doc_path = g_regex_replace_literal (regex, docs.active->path,
				-1, 0, doc_extension, 0, NULL);

		// the document (PDF, DVI, ...) file exist?
		if (! g_file_test (doc_path, G_FILE_TEST_IS_REGULAR))
		{
			gchar *text = g_strdup_printf (
					_("%s does not exist. If this is not already made, compile the document with the right command."),
					g_path_get_basename (doc_path));
			
			gtk_text_buffer_set_text (log_buffer, text, -1);
			g_free (text);
			g_free (doc_path);
			g_regex_unref (regex);
			return;
		}

		// run the command
		gchar *command = g_strdup_printf ("evince %s", doc_path);
		print_info ("execution of the command: %s", command);
		g_spawn_command_line_async (command, &error);

		if (error != NULL)
		{
			print_warning ("execution failed: %s", error->message);
			g_error_free (error);
		}

		g_regex_unref (regex);
		g_free (doc_path);
		g_free (command);
	}
}

