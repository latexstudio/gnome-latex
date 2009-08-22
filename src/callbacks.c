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
#include "print.h"

static void document_get_content (const gchar *filename, const gchar *uri);
static void create_document_in_new_tab (const gchar *path, const gchar *text,
		const gchar *title);
static void close_document (gint index);
static void save_as_dialog (void);
static void file_save (void);
static gboolean close_all (void);
static void set_title (void);
static void set_undo_redo_sensitivity (void);
static void run_compilation (gchar *title, gchar *command);
static void view_document (gchar *title, gchar *doc_extension);
static void convert_document (gchar *title, gchar *doc_extension,
		gchar *command);
static void add_action (gchar *title, gchar *command, gchar *command_output);

void
cb_new (void)
{
	char *default_text = "\\documentclass[a4paper,11pt]{article}\n"
		"\\begin{document}\n"
		"\\end{document}";

	create_document_in_new_tab (NULL, default_text, _("New document"));
}

void
cb_open (void)
{
	GtkWidget *dialog = gtk_file_chooser_dialog_new (
			_("Open File"),
			latexila.main_window,
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL
	);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		gchar *filename = gtk_file_chooser_get_filename (
				GTK_FILE_CHOOSER (dialog));
		gchar *uri = gtk_file_chooser_get_uri (
				GTK_FILE_CHOOSER (dialog));

		document_get_content (filename, uri);

		g_free (filename);
		g_free (uri);
	}
	gtk_widget_destroy (dialog);
}

void
cb_save (void)
{
	if (latexila.active_doc != NULL)
	{
		if (! latexila.active_doc->saved)
		{
			if (latexila.active_doc->path == NULL)
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
	if (latexila.active_doc != NULL)
	{
		document_t doc_backup = *latexila.active_doc;

		latexila.active_doc->path = NULL;
		latexila.active_doc->saved = FALSE;
		cb_save ();

		// if the user click on cancel
		if (! latexila.active_doc->saved)
		{
			(*latexila.active_doc) = doc_backup;
			set_title ();
		}
	}

	else
		print_warning ("no document opened");
}

void
cb_close (void)
{
	if (latexila.active_doc != NULL)
	{
		gint index = gtk_notebook_get_current_page (latexila.notebook);
		close_document (index);

		if (gtk_notebook_get_n_pages (latexila.notebook) > 0)
			latexila.active_doc = g_list_nth_data (latexila.all_docs,
					gtk_notebook_get_current_page (latexila.notebook));
		else
			latexila.active_doc = NULL;

		set_undo_redo_sensitivity ();
	}

	else
		print_warning ("no document opened");
}

void
cb_close_tab (GtkWidget *widget, GtkWidget *child)
{
	document_t *doc_backup = latexila.active_doc;
	gint page_to_close = gtk_notebook_page_num (latexila.notebook, child);
	latexila.active_doc = g_list_nth_data (latexila.all_docs, page_to_close);

	// the document to close is the current document
	if (latexila.active_doc == doc_backup)
	{
		close_document (page_to_close);

		if (gtk_notebook_get_n_pages (latexila.notebook) > 0)
			latexila.active_doc = g_list_nth_data (latexila.all_docs, gtk_notebook_get_current_page (latexila.notebook));
		else
			latexila.active_doc = NULL;

		set_undo_redo_sensitivity ();
	}

	// the document to close is not the current document
	else
	{
		close_document (page_to_close);
		latexila.active_doc = doc_backup;
	}
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
	if (gtk_source_buffer_can_undo (latexila.active_doc->source_buffer))
		gtk_source_buffer_undo (latexila.active_doc->source_buffer);

	set_undo_redo_sensitivity ();
}

void
cb_redo (void)
{
	if (gtk_source_buffer_can_redo (latexila.active_doc->source_buffer))
		gtk_source_buffer_redo (latexila.active_doc->source_buffer);

	set_undo_redo_sensitivity ();
}

gboolean
cb_delete_event (GtkWidget *widget, GdkEvent *event, gpointer user_data)
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
	guint nb_docs = g_list_length (latexila.all_docs);
	for (guint i = 0 ; i < nb_docs ; i++)
	{
		document_t *doc = g_list_nth_data (latexila.all_docs, i);
		gtk_source_view_set_show_line_numbers (
				GTK_SOURCE_VIEW (doc->source_view), show);
	}
}

void
cb_latex (void)
{
	gchar *title = _("Compile (latex)");
	gchar *command = g_strdup_printf ("latex -interaction=nonstopmode %s",
			latexila.active_doc->path);

	run_compilation (title, command);
	g_free (command);
}

void
cb_pdflatex (void)
{
	gchar *title = _("Compile (pdflatex)");
	gchar *command = g_strdup_printf ("pdflatex -interaction=nonstopmode %s",
			latexila.active_doc->path);

	run_compilation (title, command);
	g_free (command);
}

void
cb_view_dvi (void)
{
	view_document (_("View DVI"), ".dvi");
}

void
cb_view_pdf (void)
{
	view_document (_("View PDF"), ".pdf");
}

void
cb_view_ps (void)
{
	view_document (_("View PS"), ".ps");
}

void
cb_dvi_to_pdf (void)
{
	convert_document (_("DVI to PDF"), ".dvi", "dvipdf");
}

void
cb_dvi_to_ps (void)
{
	convert_document (_("DVI to PS"), ".dvi", "dvips");
}

void
cb_action_list_changed (GtkTreeSelection *selection, gpointer user_data)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	if (gtk_tree_selection_get_selected (selection, &model, &iter))
	{
		gchar *title, *command, *command_output;
		gtk_tree_model_get (model, &iter,
				COLUMN_ACTION_TITLE, &title,
				COLUMN_ACTION_COMMAND, &command,
				COLUMN_ACTION_COMMAND_OUTPUT, &command_output,
				-1);
		print_log (latexila.log, title, command, command_output);
	}
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
			latexila.main_window,
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
	if (latexila.active_doc != NULL)
	{
		latexila.active_doc->saved = FALSE;
		set_title ();
		set_undo_redo_sensitivity ();
	}
}

void
cb_page_change (GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data)
{
	latexila.active_doc = g_list_nth_data (latexila.all_docs, page_num);
	set_undo_redo_sensitivity ();
}

void
cb_recent_item_activated (GtkRecentAction *action, gpointer user_data)
{
	GtkRecentChooser *chooser = GTK_RECENT_CHOOSER (action);
	GtkRecentInfo *info = gtk_recent_chooser_get_current_item (chooser);
	const gchar *filename = gtk_recent_info_get_uri_display (info);
	const gchar *uri = gtk_recent_info_get_uri (info);

	document_get_content (filename, uri);
}

/*****************************
 * local functions
 *****************************/

static void
document_get_content (const gchar *filename, const gchar *uri)
{
	print_info ("Open file: \"%s\"", filename);

	gchar *contents = NULL;
	if (g_file_get_contents (filename, &contents, NULL, NULL))
	{
		// convert the text to UTF-8
		gchar *text_utf8 = g_locale_to_utf8 (contents, -1, NULL, NULL, NULL);

		create_document_in_new_tab (filename, text_utf8,
				g_path_get_basename (filename));

		GtkRecentManager *manager = gtk_recent_manager_get_default ();
		gtk_recent_manager_add_item (manager, uri);

		g_free (contents);
		g_free (text_utf8);
	}
	else
		print_warning ("impossible to open the file \"%s\"", filename);
}

static void
create_document_in_new_tab (const gchar *path, const gchar *text, const gchar *title)
{
	// create a new document_t structure
	// if path = NULL, this is a new document
	// else, the user opened a document
	document_t *new_doc = g_malloc (sizeof (document_t));
	new_doc->path = g_strdup (path);
	new_doc->saved = TRUE;
	new_doc->source_buffer = gtk_source_buffer_new (NULL);
	new_doc->source_view = gtk_source_view_new_with_buffer (new_doc->source_buffer);

	latexila.all_docs = g_list_append (latexila.all_docs, new_doc);
	latexila.active_doc = new_doc;

	GtkSourceLanguageManager *lm = gtk_source_language_manager_get_default ();

	// set the language for the syntaxic color
	if (path != NULL)
	{
		GtkSourceLanguage *lang = gtk_source_language_manager_guess_language (
				lm, path, NULL);
		if (lang != NULL)
			gtk_source_buffer_set_language (new_doc->source_buffer, lang);
		else
			print_info ("language of the file unknown ==> no syntaxic color");
	}

	// the default file is a LaTeX document
	else
	{
		GtkSourceLanguage *lang = gtk_source_language_manager_get_language (
				lm, "latex");
		gtk_source_buffer_set_language (new_doc->source_buffer, lang);
	}

	// set auto indentation
	gtk_source_view_set_auto_indent (GTK_SOURCE_VIEW (new_doc->source_view), TRUE);

	// set the font
	PangoFontDescription *font_desc = pango_font_description_from_string (FONT);
	gtk_widget_modify_font (new_doc->source_view, font_desc);

	// enable text wrapping (between words only)
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (new_doc->source_view),
			GTK_WRAP_WORD);

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
	// TODO set height
	GtkWidget *hbox = gtk_hbox_new (FALSE, 3);

	GtkWidget *label = gtk_label_new (title);
	new_doc->title = label;
	gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);

	GtkWidget *close_button = gtk_button_new ();

	// apply the style defined with gtk_rc_parse_string
	// the button will be smaller
	gtk_widget_set_name (close_button, "my-close-button");

	gtk_button_set_relief (GTK_BUTTON (close_button), GTK_RELIEF_NONE);
	GtkWidget *image = gtk_image_new_from_stock (GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU);
	gtk_container_add (GTK_CONTAINER (close_button), image);
	g_signal_connect (G_OBJECT (close_button), "clicked",
			G_CALLBACK (cb_close_tab), sw);

	gtk_box_pack_start (GTK_BOX (hbox), close_button, FALSE, FALSE, 0);
	gtk_widget_show_all (hbox);

	gint index = gtk_notebook_append_page (latexila.notebook, sw, hbox);
	gtk_notebook_set_current_page (latexila.notebook, index);
	
	set_undo_redo_sensitivity ();
}

static void
close_document (gint index)
{
	if (latexila.active_doc != NULL)
	{
		/* if the document is not saved, ask the user for saving changes before
		 * closing */
		if (! latexila.active_doc->saved)
		{
			GtkWidget *dialog = gtk_dialog_new_with_buttons (
					_("Close the document"),
					latexila.main_window,
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
		latexila.all_docs = g_list_remove (latexila.all_docs, latexila.active_doc);
		if (latexila.active_doc->path != NULL)
		{
			print_info ("close the file \"%s\"", latexila.active_doc->path);
			g_free (latexila.active_doc->path);
		}
		g_free (latexila.active_doc);

		gtk_notebook_remove_page (latexila.notebook, index);
	}

	else
		print_warning ("no document opened");
}

static void
save_as_dialog (void)
{
	GtkWidget *dialog = gtk_file_chooser_dialog_new (
			_("Save File"),
			latexila.main_window,
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL
	);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
		latexila.active_doc->path = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

	gtk_widget_destroy (dialog);
}

static void
file_save (void)
{
	if (latexila.active_doc->path != NULL)
	{
		print_info ("save current buffer to \"%s\"", latexila.active_doc->path);

		FILE* file = fopen (latexila.active_doc->path, "w");
		if (file != NULL)
		{
			GtkTextBuffer *text_buffer = GTK_TEXT_BUFFER (latexila.active_doc->source_buffer);
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

			latexila.active_doc->saved = TRUE;
		}

		// error with fopen
		else
		{
			print_warning ("impossible to save the file \"%s\"",
					latexila.active_doc->path);
		}
	}
}

static gboolean
close_all (void)
{
	while (latexila.active_doc != NULL)
	{
		int tmp = gtk_notebook_get_n_pages (latexila.notebook);
		gtk_notebook_set_current_page (latexila.notebook, 0);
		cb_close ();
		if (gtk_notebook_get_n_pages (latexila.notebook) >= tmp)
			return FALSE;
	}

	return TRUE;
}

static void
set_title (void)
{
	if (latexila.active_doc)
	{
		gchar *tmp;
		gchar *title;

		if (latexila.active_doc->path != NULL)
			tmp = g_path_get_basename (latexila.active_doc->path);
		else
			tmp = g_strdup (_("New document"));

		if (latexila.active_doc->saved)
			title = g_strdup (tmp);
		else
			title = g_strdup_printf ("*%s", tmp);

		gtk_label_set_text (GTK_LABEL (latexila.active_doc->title), title);

		g_free (tmp);
		g_free (title);
	}
}

static void
set_undo_redo_sensitivity (void)
{
	gboolean can_undo = FALSE;
	gboolean can_redo = FALSE;

	if (latexila.active_doc != NULL)
	{
		can_undo = gtk_source_buffer_can_undo (latexila.active_doc->source_buffer);
		can_redo = gtk_source_buffer_can_redo (latexila.active_doc->source_buffer);
	}

	gtk_action_set_sensitive (latexila.undo, can_undo);
	gtk_action_set_sensitive (latexila.redo, can_redo);
}

static void
run_compilation (gchar *title, gchar *command)
{
	if (latexila.active_doc != NULL)
	{
		gchar *command_output;

		// the current document is a *.tex file?
		gboolean tex_file = g_regex_match_simple ("\\.tex$", latexila.active_doc->path, 0, 0);
		if (! tex_file)
		{
			command_output = g_strdup_printf (_("compilation failed: %s is not a *.tex file"),
					g_path_get_basename (latexila.active_doc->path));

			add_action (title, command, command_output);
			g_free (command_output);
			return;
		}

		// run the command in the directory where the .tex is
		gchar *dir_backup = g_get_current_dir ();
		gchar *dir = g_path_get_dirname (latexila.active_doc->path);
		g_chdir (dir);

		print_info ("execution of the command: %s", command);
		
		GError *error = NULL;
		g_spawn_command_line_sync (command, &command_output, NULL, NULL, &error);
		g_chdir (dir_backup);
		
		// an error occured
		if (error != NULL)
		{
			command_output = g_strdup_printf (_("execution failed: %s"),
					error->message);
			g_error_free (error);
		}

		add_action (title, command, command_output);

		g_free (command_output);
		g_free (dir_backup);
		g_free (dir);
	}
}

static void
view_document (gchar *title, gchar *doc_extension)
{
	if (latexila.active_doc != NULL)
	{
		gchar *command, *command_output;

		GError *error = NULL;
		GRegex *regex = g_regex_new ("\\.tex$", 0, 0, NULL);

		// replace .tex by doc_extension (.pdf, .dvi, ...)
		gchar *doc_path = g_regex_replace_literal (regex, latexila.active_doc->path,
				-1, 0, doc_extension, 0, NULL);

		command = g_strdup_printf ("evince %s", doc_path);

		// the current document is a *.tex file?
		gboolean tex_file = g_regex_match (regex, latexila.active_doc->path, 0, NULL);
		if (! tex_file)
		{
			command_output = g_strdup_printf (_("failed: %s is not a *.tex file"),
					g_path_get_basename (latexila.active_doc->path));

			add_action (title, command, command_output);
			g_free (command);
			g_free (command_output);
			g_free (doc_path);
			g_regex_unref (regex);
			return;
		}

		// the document (PDF, DVI, ...) file exist?
		if (! g_file_test (doc_path, G_FILE_TEST_IS_REGULAR))
		{
			command_output = g_strdup_printf (
					_("%s does not exist. If this is not already made, compile the document with the right command."),
					g_path_get_basename (doc_path));

			add_action (title, command, command_output);
			g_free (command);
			g_free (command_output);
			g_free (doc_path);
			g_regex_unref (regex);
			return;
		}

		// run the command
		print_info ("execution of the command: %s", command);
		g_spawn_command_line_async (command, &error);

		if (error != NULL)
		{
			command_output = g_strdup_printf (_("execution failed: %s"),
					error->message);
			g_error_free (error);
		}
		else
			command_output = g_strdup ("OK");

		add_action (title, command, command_output);

		g_free (command);
		g_free (command_output);
		g_free (doc_path);
		g_regex_unref (regex);
	}
}

static void
convert_document (gchar *title, gchar *doc_extension, gchar *command)
{
	if (latexila.active_doc != NULL)
	{
		gchar *full_command, *command_output;

		GError *error = NULL;
		GRegex *regex = g_regex_new ("\\.tex$", 0, 0, NULL);

		// replace .tex by doc_extension (.pdf, .dvi, ...)
		gchar *doc_path = g_regex_replace_literal (regex,
				latexila.active_doc->path, -1, 0, doc_extension, 0, NULL);

		full_command = g_strdup_printf ("%s %s", command, doc_path);

		// the document to convert exist?
		if (! g_file_test (doc_path, G_FILE_TEST_IS_REGULAR))
		{
			command_output = g_strdup_printf (
					_("%s does not exist. If this is not already made, compile the document with the right command."),
					g_path_get_basename (doc_path));

			add_action (title, full_command, command_output);
			g_free (full_command);
			g_free (command_output);
			g_free (doc_path);
			g_regex_unref (regex);
			return;
		}

		// run the command in the directory where the .tex is
		print_info ("execution of the command: %s", full_command);

		gchar *dir_backup = g_get_current_dir ();
		gchar *dir = g_path_get_dirname (latexila.active_doc->path);
		g_chdir (dir);
		g_spawn_command_line_sync (full_command, &command_output, NULL, NULL, &error);
		g_chdir (dir_backup);

		
		// an error occured
		if (error != NULL)
		{
			command_output = g_strdup_printf (_("execution failed: %s"),
					error->message);
			g_error_free (error);
		}

		add_action (title, full_command, command_output);

		g_free (full_command);
		g_free (command_output);
		g_free (doc_path);
		g_regex_unref (regex);
	}
}

static void
add_action (gchar *title, gchar *command, gchar *command_output)
{
	static gint num = 1;
	gchar *title2 = g_strdup_printf ("%d. %s", num, title);

	// append an new entry to the action list
	GtkTreeIter iter;
	gtk_list_store_append (latexila.list_store, &iter);
	gtk_list_store_set (latexila.list_store, &iter,
			COLUMN_ACTION_TITLE, title2,
			COLUMN_ACTION_COMMAND, command,
			COLUMN_ACTION_COMMAND_OUTPUT, command_output,
			-1);

	// the new entry is selected
	// cb_action_list_changed () is called, so the details are showed
	gtk_tree_selection_select_iter (latexila.list_selection, &iter);

	// scroll to the end
	GtkTreePath *path = gtk_tree_model_get_path (
			GTK_TREE_MODEL (latexila.list_store), &iter);
	gtk_tree_view_scroll_to_cell (latexila.list_view, path, NULL, FALSE, 0, 0);

	num++;
	g_free (title2);
}


