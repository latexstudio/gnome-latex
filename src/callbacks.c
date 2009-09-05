/*
 * This file is part of LaTeXila.
 *
 * Copyright © 2009 Sébastien Wilmet
 *
 * LaTeXila is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LaTeXila is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LaTeXila.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <libintl.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourceiter.h>

#include "main.h"
#include "config.h"
#include "callbacks.h"
#include "print.h"

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
static void add_action (gchar *title, gchar *command, gchar *command_output,
		gboolean error);
static void update_cursor_position_statusbar (void);
static void scroll_to_cursor (void);
static gboolean find_next_match (const gchar *what, GtkSourceSearchFlags flags,
		gboolean backward, GtkTextIter *match_start, GtkTextIter *match_end);
static void change_font_source_view (void);
static void create_preferences (void);

static GtkWidget *pref_dialog = NULL;
static gboolean pref_changed = FALSE;

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

		open_new_document (filename, uri);

		g_free (filename);
		g_free (uri);
	}
	gtk_widget_destroy (dialog);
}

void
cb_save (void)
{
	if (latexila.active_doc == NULL)
		return;

	if (! latexila.active_doc->saved)
	{
		if (latexila.active_doc->path == NULL)
			save_as_dialog ();

		file_save ();
		set_title ();
	}
}

void
cb_save_as (void)
{
	if (latexila.active_doc == NULL)
		return;

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

void
cb_close (void)
{
	if (latexila.active_doc == NULL)
		return;

	gint index = gtk_notebook_get_current_page (latexila.notebook);
	close_document (index);

	if (gtk_notebook_get_n_pages (latexila.notebook) > 0)
		latexila.active_doc = g_list_nth_data (latexila.all_docs,
				gtk_notebook_get_current_page (latexila.notebook));
	else
		latexila.active_doc = NULL;

	set_undo_redo_sensitivity ();
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
		update_cursor_position_statusbar ();
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
	if (latexila.active_doc == NULL)
		return;

	if (gtk_source_buffer_can_undo (latexila.active_doc->source_buffer))
	{
		gtk_source_buffer_undo (latexila.active_doc->source_buffer);
		scroll_to_cursor ();
	}

	set_undo_redo_sensitivity ();
}

void
cb_redo (void)
{
	if (latexila.active_doc == NULL)
		return;

	if (gtk_source_buffer_can_redo (latexila.active_doc->source_buffer))
	{
		gtk_source_buffer_redo (latexila.active_doc->source_buffer);
		scroll_to_cursor ();
	}

	set_undo_redo_sensitivity ();
}

void
cb_cut (void)
{
	if (latexila.active_doc == NULL)
		return;

	GtkClipboard *clipboard = gtk_widget_get_clipboard (
			latexila.active_doc->source_view, GDK_SELECTION_CLIPBOARD);
	gtk_text_buffer_cut_clipboard (
			GTK_TEXT_BUFFER (latexila.active_doc->source_buffer),
			clipboard, TRUE);
}

void
cb_copy (void)
{
	if (latexila.active_doc == NULL)
		return;

	GtkClipboard *clipboard = gtk_widget_get_clipboard (
			latexila.active_doc->source_view, GDK_SELECTION_CLIPBOARD);
	gtk_text_buffer_copy_clipboard (
			GTK_TEXT_BUFFER (latexila.active_doc->source_buffer), clipboard);
}

void
cb_paste (void)
{
	if (latexila.active_doc == NULL)
		return;

	GtkClipboard *clipboard = gtk_widget_get_clipboard (
			latexila.active_doc->source_view, GDK_SELECTION_CLIPBOARD);
	gtk_text_buffer_paste_clipboard (
			GTK_TEXT_BUFFER (latexila.active_doc->source_buffer),
			clipboard, NULL, TRUE);
}

void
cb_delete (void)
{
	if (latexila.active_doc == NULL)
		return;

	GtkTextBuffer *buffer = GTK_TEXT_BUFFER (latexila.active_doc->source_buffer);
	gtk_text_buffer_delete_selection (buffer, TRUE, TRUE);
}

void
cb_select_all (void)
{
	if (latexila.active_doc == NULL)
		return;

	GtkTextBuffer *buffer = GTK_TEXT_BUFFER (latexila.active_doc->source_buffer);
	GtkTextIter start, end;
	gtk_text_buffer_get_start_iter (buffer, &start);
	gtk_text_buffer_get_end_iter (buffer, &end);
	gtk_text_buffer_select_range (buffer, &start, &end);
}

void
cb_preferences (void)
{
	if (pref_dialog == NULL)
		create_preferences ();
	gtk_window_present (GTK_WINDOW (pref_dialog));
}

void
cb_zoom_in (void)
{
	latexila.font_size += PANGO_SCALE;
	pango_font_description_set_size (latexila.font_desc, latexila.font_size);
	change_font_source_view ();
}

void
cb_zoom_out (void)
{
	latexila.font_size -= PANGO_SCALE;
	pango_font_description_set_size (latexila.font_desc, latexila.font_size);
	change_font_source_view ();
}

void
cb_zoom_reset (void)
{
	gchar *font_string = g_key_file_get_string (latexila.key_file,
            PROGRAM_NAME, "font", NULL);
    latexila.font_desc = pango_font_description_from_string (font_string);
    latexila.font_size = pango_font_description_get_size (latexila.font_desc);
	change_font_source_view ();
}

void
cb_find (void)
{
	if (latexila.active_doc == NULL)
		return;

	GtkWidget *dialog = gtk_dialog_new_with_buttons (
			_("Find"),
			latexila.main_window,
			GTK_DIALOG_MODAL,
			GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
			GTK_STOCK_FIND, GTK_RESPONSE_OK,
			NULL);

	GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

	GtkWidget *hbox = gtk_hbox_new (FALSE, 10);
	GtkWidget *label = gtk_label_new (_("Search for:"));
	GtkWidget *entry = gtk_entry_new ();
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), entry, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (content_area), hbox, TRUE, TRUE, 5);

	GtkWidget *case_sensitive = gtk_check_button_new_with_label (
			_("Case sensitive"));
	gtk_box_pack_start (GTK_BOX (content_area), case_sensitive, TRUE, TRUE, 5);

	GtkWidget *backward_search = gtk_check_button_new_with_label (
			_("Search backwards"));
	gtk_box_pack_start (GTK_BOX (content_area), backward_search, TRUE, TRUE, 5);

	gtk_widget_show_all (content_area);

	guint context_id = gtk_statusbar_get_context_id (latexila.statusbar,
			"searching");

	while (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
	{
		const gchar *what = gtk_entry_get_text (GTK_ENTRY (entry));
		gboolean tmp = gtk_toggle_button_get_active (
				GTK_TOGGLE_BUTTON (case_sensitive));
		GtkSourceSearchFlags flags = tmp ? 0 : GTK_SOURCE_SEARCH_CASE_INSENSITIVE;

		gboolean backward = gtk_toggle_button_get_active (
				GTK_TOGGLE_BUTTON (backward_search));

		GtkTextIter match_start, match_end;
		if (! find_next_match (what, flags, backward, &match_start, &match_end))
		{
			// print a message in the statusbar
			gtk_statusbar_pop (latexila.statusbar, context_id);
			gtk_statusbar_push (latexila.statusbar, context_id,
					_("Phrase not found"));
		}
	}

	gtk_statusbar_pop (latexila.statusbar, context_id);

	gtk_widget_destroy (dialog);
}

void
cb_replace (void)
{
	if (latexila.active_doc == NULL)
		return;

	GtkWidget *dialog = gtk_dialog_new ();
	gtk_window_set_title (GTK_WINDOW (dialog), _("Replace"));
	gtk_dialog_add_buttons (GTK_DIALOG (dialog),
			GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
			_("Replace All"), GTK_RESPONSE_APPLY,
			NULL);

	// button replace
	// we must set the sensitivity of this button so we create it by hand
	GtkWidget *button_replace = gtk_button_new_with_label (_("Replace"));
	GtkWidget *icon = gtk_image_new_from_stock (GTK_STOCK_FIND_AND_REPLACE,
			GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image (GTK_BUTTON (button_replace), icon);
	gtk_dialog_add_action_widget (GTK_DIALOG (dialog), button_replace,
			GTK_RESPONSE_YES);

	gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_FIND, GTK_RESPONSE_OK);


	GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
	GtkWidget *table = gtk_table_new (2, 2, FALSE);

	GtkWidget *label = gtk_label_new (_("Search for:"));
	// align the label to the left
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	GtkWidget *entry_search = gtk_entry_new ();
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 0, 1);
	gtk_table_attach_defaults (GTK_TABLE (table), entry_search, 1, 2, 0, 1);

	label = gtk_label_new (_("Replace with:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	GtkWidget *entry_replace = gtk_entry_new ();
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 1, 2);
	gtk_table_attach_defaults (GTK_TABLE (table), entry_replace, 1, 2, 1, 2);

	gtk_box_pack_start (GTK_BOX (content_area), table, TRUE, TRUE, 5);

	GtkWidget *case_sensitive = gtk_check_button_new_with_label (
			_("Case sensitive"));
	gtk_box_pack_start (GTK_BOX (content_area), case_sensitive, TRUE, TRUE, 5);

	gtk_widget_show_all (content_area);

	GtkTextBuffer *buffer = GTK_TEXT_BUFFER (latexila.active_doc->source_buffer);

	/* backward desactivated because there is a little problem with the position
	 * of the insert mark after making a replacement... The insert mark should
	 * be at the beginning of the replacement and not at the end.
	 */
	gboolean backward = FALSE;

	guint context_id = gtk_statusbar_get_context_id (latexila.statusbar,
			"searching");

	gboolean stop = FALSE;
	gboolean match_found = FALSE;
	GtkTextIter match_start, match_end, iter;
	while (! stop)
	{
		gtk_widget_set_sensitive (button_replace, match_found);
		gint result = gtk_dialog_run (GTK_DIALOG (dialog));

		const gchar *what = gtk_entry_get_text (GTK_ENTRY (entry_search));
		const gchar *replacement = gtk_entry_get_text (GTK_ENTRY (entry_replace));

		gboolean tmp = gtk_toggle_button_get_active (
				GTK_TOGGLE_BUTTON (case_sensitive));
		GtkSourceSearchFlags flags = tmp ? 0 : GTK_SOURCE_SEARCH_CASE_INSENSITIVE;

		switch (result)
		{
			case GTK_RESPONSE_CLOSE:
				stop = TRUE;
				break;

			/* replace all */
			case GTK_RESPONSE_APPLY:
				gtk_text_buffer_begin_user_action (buffer);

				// begin at the start of the buffer
				gtk_text_buffer_get_start_iter (buffer, &iter);
				gint i = 0;
				while (gtk_source_iter_forward_search (&iter, what, flags,
							&match_start, &match_end, NULL))
				{
					// make the replacement
					gtk_text_buffer_delete (buffer, &match_start, &match_end);
					gtk_text_buffer_insert (buffer, &match_start, replacement, -1);

					// iter points to the end of the inserted text
					iter = match_start;

					i++;
				}

				gtk_text_buffer_end_user_action (buffer);

				// print a message in the statusbar
				gchar *msg;
				if (i == 0)
					msg = g_strdup (_("Phrase not found"));
				else if (i == 1)
					msg = g_strdup (_("Found and replaced one occurence"));
				else
					msg = g_strdup_printf (_("Found and replaced %d occurences"), i);

				gtk_statusbar_pop (latexila.statusbar, context_id);
				gtk_statusbar_push (latexila.statusbar, context_id, msg);
				g_free (msg);

				break;

			/* replace */
			case GTK_RESPONSE_YES:
				// the user must find the first match
				if (! match_found)
					break;

				gtk_text_buffer_begin_user_action (buffer);
				gtk_text_buffer_delete (buffer, &match_start, &match_end);
				gtk_text_buffer_insert (buffer, &match_start, replacement, -1);
				gtk_text_buffer_end_user_action (buffer);

				match_found = find_next_match (what, flags, backward,
						&match_start, &match_end);
				break;

			/* just find the next match */
			case GTK_RESPONSE_OK:
				match_found = find_next_match (what, flags, backward,
						&match_start, &match_end);
				if (! match_found)
				{
					gtk_statusbar_pop (latexila.statusbar, context_id);
					gtk_statusbar_push (latexila.statusbar, context_id,
							_("Phrase not found"));
				}
				break;

			default:
				stop = TRUE;
				break;
		}
	}

	gtk_statusbar_pop (latexila.statusbar, context_id);
	gtk_widget_destroy (dialog);
}

gboolean
cb_delete_event (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	cb_quit ();

	// the destroy signal is not emitted
	return TRUE;
}

void
cb_latex (void)
{
	if (latexila.active_doc == NULL)
		return;

	gchar *title = _("Compile (latex)");
	gchar *command = g_strdup_printf ("latex -interaction=nonstopmode %s",
			latexila.active_doc->path);

	run_compilation (title, command);
	g_free (command);
}

void
cb_pdflatex (void)
{
	if (latexila.active_doc == NULL)
		return;

	gchar *title = _("Compile (pdflatex)");
	gchar *command = g_strdup_printf ("pdflatex -interaction=nonstopmode %s",
			latexila.active_doc->path);

	run_compilation (title, command);
	g_free (command);
}

void
cb_view_dvi (void)
{
	if (latexila.active_doc == NULL)
		return;

	view_document (_("View DVI"), ".dvi");
}

void
cb_view_pdf (void)
{
	if (latexila.active_doc == NULL)
		return;

	view_document (_("View PDF"), ".pdf");
}

void
cb_view_ps (void)
{
	if (latexila.active_doc == NULL)
		return;

	view_document (_("View PS"), ".ps");
}

void
cb_dvi_to_pdf (void)
{
	if (latexila.active_doc == NULL)
		return;

	convert_document (_("DVI to PDF"), ".dvi", "dvipdf");
}

void
cb_dvi_to_ps (void)
{
	if (latexila.active_doc == NULL)
		return;

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
		gboolean error;
		gtk_tree_model_get (model, &iter,
				COLUMN_ACTION_TITLE, &title,
				COLUMN_ACTION_COMMAND, &command,
				COLUMN_ACTION_COMMAND_OUTPUT, &command_output,
				COLUMN_ACTION_ERROR, &error,
				-1);
		print_log (latexila.log, title, command, command_output, error);
	}
}

void
cb_about_dialog (void)
{
	gchar *comments = _("LaTeXila is a LaTeX development environment for the GNOME Desktop");
	gchar *copyright = "Copyright © 2009 Sébastien Wilmet";

	gchar *licence = "LaTeXila is free software: you can redistribute it and/or modify\n"
			"it under the terms of the GNU General Public License as published by\n"
			"the Free Software Foundation, either version 3 of the License, or\n"
			"(at your option) any later version.\n\n"

			"LaTeXila is distributed in the hope that it will be useful,\n"
			"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
			"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
			"GNU General Public License for more details.\n\n"

			"You should have received a copy of the GNU General Public License\n"
			"along with LaTeXila.  If not, see <http://www.gnu.org/licenses/>.\n";

	//TODO set the url hook
	gchar *website = "http://latexila.sourceforge.net/";

	const gchar *authors[] =
	{
		"Sébastien Wilmet <sebastien.wilmet@gmail.com>",
		NULL
	};

	const gchar *artists[] =
	{
		"The Kile Team: http://kile.sourceforge.net/",
		NULL
	};

	gtk_show_about_dialog (
			latexila.main_window,
			"program-name", PROGRAM_NAME,
			"authors", authors,
			"artists", artists,
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
	if (latexila.active_doc == NULL)
		return;

	latexila.active_doc->saved = FALSE;
	set_title ();
	set_undo_redo_sensitivity ();

	update_cursor_position_statusbar ();
}

void
cb_cursor_moved (GtkTextBuffer *text_buffer, GtkTextIter *location,
		GtkTextMark *mark, gpointer user_data)
{
	if (mark != gtk_text_buffer_get_insert (text_buffer))
		return;

	update_cursor_position_statusbar ();
}

void
cb_page_change (GtkNotebook *notebook, GtkNotebookPage *page, guint page_num,
		gpointer user_data)
{
	latexila.active_doc = g_list_nth_data (latexila.all_docs, page_num);
	set_undo_redo_sensitivity ();
	update_cursor_position_statusbar ();
}

void
cb_recent_item_activated (GtkRecentAction *action, gpointer user_data)
{
	GtkRecentChooser *chooser = GTK_RECENT_CHOOSER (action);
	GtkRecentInfo *info = gtk_recent_chooser_get_current_item (chooser);
	const gchar *filename = gtk_recent_info_get_uri_display (info);
	const gchar *uri = gtk_recent_info_get_uri (info);

	open_new_document (filename, uri);
}

void
cb_pref_dialog_close (GtkDialog *dialog, gint response_id, gpointer user_data)
{
	gtk_widget_hide (GTK_WIDGET (dialog));

	if (pref_changed)
		save_preferences ();
}

void
cb_line_numbers (GtkToggleButton *toggle_button, gpointer user_data)
{
	gboolean show_line_numbers = gtk_toggle_button_get_active (toggle_button);
	g_key_file_set_boolean (latexila.key_file, PROGRAM_NAME,
			"show_line_numbers", show_line_numbers);

	//TODO optimisation?
	guint nb_docs = g_list_length (latexila.all_docs);
	for (guint i = 0 ; i < nb_docs ; i++)
	{
		document_t *doc = g_list_nth_data (latexila.all_docs, i);
		gtk_source_view_set_show_line_numbers (
				GTK_SOURCE_VIEW (doc->source_view), show_line_numbers);
	}

	pref_changed = TRUE;
}

void
cb_command_view (GtkButton *button, gpointer user_data)
{
	GtkEntry *entry = (GtkEntry *) user_data;
	const gchar *new_command_view = gtk_entry_get_text (entry);
	g_key_file_set_string (latexila.key_file, PROGRAM_NAME,
			"command_view", new_command_view);
	pref_changed = TRUE;
}

void
cb_font_set (GtkFontButton *font_button, gpointer user_data)
{
	const gchar *font_string = gtk_font_button_get_font_name (font_button);
	g_key_file_set_string (latexila.key_file, PROGRAM_NAME,
			"font", font_string);
	latexila.font_desc = pango_font_description_from_string (font_string);
	latexila.font_size = pango_font_description_get_size (latexila.font_desc);
	change_font_source_view ();
	pref_changed = TRUE;
}

void
cb_symbol_selected (GtkIconView *icon_view, gpointer user_data)
{
	if (latexila.active_doc == NULL)
	{
		gtk_icon_view_unselect_all (icon_view);
		return;
	}

	GList *selected_items = gtk_icon_view_get_selected_items (icon_view);
	// unselect the symbol, so the user can insert several times the same symbol
	gtk_icon_view_unselect_all (icon_view);
	GtkTreePath *path = g_list_nth_data (selected_items, 0);
	GtkTreeModel *model = gtk_icon_view_get_model (icon_view);
	GtkTreeIter iter;

	if (path != NULL && gtk_tree_model_get_iter (model, &iter, path))
	{
		gchar *latex_command;
		gtk_tree_model_get (model, &iter,
				COLUMN_SYMBOL_COMMAND, &latex_command,
				-1);

		// insert the symbol in the current document
		GtkTextBuffer *buffer =
			GTK_TEXT_BUFFER (latexila.active_doc->source_buffer);
		gtk_text_buffer_begin_user_action (buffer);
		gtk_text_buffer_insert_at_cursor (
				GTK_TEXT_BUFFER (latexila.active_doc->source_buffer),
				latex_command, -1);
		gtk_text_buffer_end_user_action (buffer);

		g_free (latex_command);
	}

	// free the GList
	g_list_foreach (selected_items, (GFunc) gtk_tree_path_free, NULL);
	g_list_free (selected_items);
}

void
open_new_document (const gchar *filename, const gchar *uri)
{
	print_info ("open file: \"%s\"", filename);

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

void
save_preferences (void)
{
	gchar *key_file_data = g_key_file_to_data (latexila.key_file, NULL, NULL);
	FILE* file = fopen (latexila.pref_file, "w");
	if (file != NULL)
	{
		fprintf (file, "%s", key_file_data);
		fclose (file);
	}
	else
	{
		print_warning ("impossible to save user preferences to \"%s\"",
				latexila.pref_file);
	}
	g_free (key_file_data);
}

/******************************************************************************
 * local functions
 *****************************************************************************/

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
	gtk_widget_modify_font (new_doc->source_view, latexila.font_desc);

	// enable text wrapping (between words only)
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (new_doc->source_view),
			GTK_WRAP_WORD);

	// show line numbers?
	gboolean show_line_numbers = g_key_file_get_boolean (latexila.key_file,
			PROGRAM_NAME, "show_line_numbers", NULL);
	gtk_source_view_set_show_line_numbers (
			GTK_SOURCE_VIEW (new_doc->source_view), show_line_numbers);

	// put the text into the buffer
	gtk_source_buffer_begin_not_undoable_action (new_doc->source_buffer);
	gtk_text_buffer_set_text (GTK_TEXT_BUFFER (new_doc->source_buffer), text, -1);
	gtk_source_buffer_end_not_undoable_action (new_doc->source_buffer);

	// move the cursor at the start
	GtkTextIter start;
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (new_doc->source_buffer), &start);
	gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER (new_doc->source_buffer), &start);

	// when the text is modified
	g_signal_connect (G_OBJECT (new_doc->source_buffer), "changed",
			G_CALLBACK (cb_text_changed), NULL);

	// when the cursor is moved
	g_signal_connect (G_OBJECT (new_doc->source_buffer), "mark-set",
			G_CALLBACK (cb_cursor_moved), NULL);

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
	update_cursor_position_statusbar ();
}

static void
close_document (gint index)
{
	if (latexila.active_doc == NULL)
		return;

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
		//gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), label, TRUE, FALSE, 10);
		//gtk_widget_show_all (GTK_DIALOG (dialog)->vbox);
		GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
		gtk_box_pack_start (GTK_BOX (content_area), label, TRUE, FALSE, 10);
		gtk_widget_show_all (content_area);

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
	if (latexila.active_doc->path == NULL)
		return;

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
	if (latexila.active_doc == NULL)
		return;

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
	if (latexila.active_doc == NULL)
		return;

	gchar *command_output;
	gboolean is_error = TRUE;

	/* the current document is a *.tex file? */
	gboolean tex_file = g_str_has_suffix (latexila.active_doc->path, ".tex");
	if (! tex_file)
	{
		command_output = g_strdup_printf (_("compilation failed: %s is not a *.tex file"),
				g_path_get_basename (latexila.active_doc->path));

		add_action (title, command, command_output, is_error);
		g_free (command_output);
		return;
	}

	/* print a message in the statusbar */
	guint context_id = gtk_statusbar_get_context_id (latexila.statusbar,
			"running-action");
	gtk_statusbar_push (latexila.statusbar, context_id,
			_("Compilation in progress. Please wait..."));

	// without that, the message in the statusbar does not appear
	while (gtk_events_pending ())
		gtk_main_iteration ();

	/* run the command in the directory where the .tex is */
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
	else
		is_error = FALSE;

	add_action (title, command, command_output, is_error);

	gtk_statusbar_pop (latexila.statusbar, context_id);

	g_free (command_output);
	g_free (dir_backup);
	g_free (dir);
}

static void
view_document (gchar *title, gchar *doc_extension)
{
	if (latexila.active_doc == NULL)
		return;

	gchar *command, *command_output;
	gboolean is_error = TRUE;

	GError *error = NULL;
	GRegex *regex = g_regex_new ("\\.tex$", 0, 0, NULL);

	/* replace .tex by doc_extension (.pdf, .dvi, ...) */
	gchar *doc_path = g_regex_replace_literal (regex, latexila.active_doc->path,
			-1, 0, doc_extension, 0, NULL);

	gchar *command_view = g_key_file_get_string (latexila.key_file, PROGRAM_NAME,
			"command_view", NULL);
	command = g_strdup_printf ("%s %s", command_view, doc_path);

	/* the current document is a *.tex file? */
	gboolean tex_file = g_regex_match (regex, latexila.active_doc->path, 0, NULL);
	if (! tex_file)
	{
		command_output = g_strdup_printf (_("failed: %s is not a *.tex file"),
				g_path_get_basename (latexila.active_doc->path));

		add_action (title, command, command_output, is_error);
		g_free (command);
		g_free (command_output);
		g_free (doc_path);
		g_regex_unref (regex);
		return;
	}

	/* the document (PDF, DVI, ...) file exist? */
	if (! g_file_test (doc_path, G_FILE_TEST_IS_REGULAR))
	{
		command_output = g_strdup_printf (
				_("%s does not exist. If this is not already made, compile the document with the right command."),
				g_path_get_basename (doc_path));

		add_action (title, command, command_output, is_error);
		g_free (command);
		g_free (command_output);
		g_free (doc_path);
		g_regex_unref (regex);
		return;
	}

	/* run the command */
	print_info ("execution of the command: %s", command);
	g_spawn_command_line_async (command, &error);

	if (error != NULL)
	{
		command_output = g_strdup_printf (_("execution failed: %s"),
				error->message);
		g_error_free (error);
	}
	else
	{
		command_output = g_strdup (_("Viewing in progress. Please wait..."));
		is_error = FALSE;
	}

	add_action (title, command, command_output, is_error);

	g_free (command);
	g_free (command_output);
	g_free (doc_path);
	g_regex_unref (regex);
}

static void
convert_document (gchar *title, gchar *doc_extension, gchar *command)
{
	if (latexila.active_doc == NULL)
		return;

	gchar *full_command, *command_output;
	gboolean is_error = TRUE;

	GError *error = NULL;
	GRegex *regex = g_regex_new ("\\.tex$", 0, 0, NULL);

	/* replace .tex by doc_extension (.pdf, .dvi, ...) */
	gchar *doc_path = g_regex_replace_literal (regex,
			latexila.active_doc->path, -1, 0, doc_extension, 0, NULL);

	full_command = g_strdup_printf ("%s %s", command, doc_path);

	/* the document to convert exist? */
	if (! g_file_test (doc_path, G_FILE_TEST_IS_REGULAR))
	{
		command_output = g_strdup_printf (
				_("%s does not exist. If this is not already made, compile the document with the right command."),
				g_path_get_basename (doc_path));

		add_action (title, full_command, command_output, is_error);
		g_free (full_command);
		g_free (command_output);
		g_free (doc_path);
		g_regex_unref (regex);
		return;
	}

	/* print a message in the statusbar */
	guint context_id = gtk_statusbar_get_context_id (latexila.statusbar,
			"running-action");
	gtk_statusbar_push (latexila.statusbar, context_id,
			_("Converting in progress. Please wait..."));

	// without that, the message in the statusbar does not appear
	while (gtk_events_pending ())
		gtk_main_iteration ();

	/* run the command in the directory where the .tex is */
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
	else
		is_error = FALSE;

	add_action (title, full_command, command_output, is_error);

	gtk_statusbar_pop (latexila.statusbar, context_id);

	g_free (full_command);
	g_free (command_output);
	g_free (doc_path);
	g_regex_unref (regex);
}

static void
add_action (gchar *title, gchar *command, gchar *command_output, gboolean error)
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
			COLUMN_ACTION_ERROR, error,
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

static void
update_cursor_position_statusbar (void)
{
	if (latexila.active_doc == NULL)
		return;

	/* get the cursor location */
	GtkTextIter location;
	GtkTextBuffer *buffer = GTK_TEXT_BUFFER (latexila.active_doc->source_buffer);
	gtk_text_buffer_get_iter_at_mark (buffer, &location,
			gtk_text_buffer_get_insert (buffer));

	/* get row */
	gint row = gtk_text_iter_get_line (&location) + 1;

	/* get column */
	gint tabwidth = gtk_source_view_get_tab_width (
			GTK_SOURCE_VIEW (latexila.active_doc->source_view));
	GtkTextIter start = location;
	gtk_text_iter_set_line_offset (&start, 0);
	gint col = 1;

	while (!gtk_text_iter_equal (&start, &location))
	{
		// take into acount the tabulation width
		if (gtk_text_iter_get_char (&start) == '\t')
		{
			col += (tabwidth - (col % tabwidth));
		}
		else
			col++;

		gtk_text_iter_forward_char (&start);
	}

	/* print the cursor position in the statusbar */
	gtk_statusbar_pop (latexila.cursor_position, 0);
	gchar *text = g_strdup_printf ("Ln %d, Col %d", row, col);
	gtk_statusbar_push (latexila.cursor_position, 0, text);
	g_free (text);
}

static void
scroll_to_cursor (void)
{
	if (latexila.active_doc == NULL)
		return;

	gtk_text_view_scroll_to_mark (
			GTK_TEXT_VIEW (latexila.active_doc->source_view),
			gtk_text_buffer_get_insert (
				GTK_TEXT_BUFFER (latexila.active_doc->source_buffer)),
			0.25, FALSE, 0, 0);
}

static gboolean
find_next_match (const gchar *what, GtkSourceSearchFlags flags,
		gboolean backward, GtkTextIter *match_start, GtkTextIter *match_end)
{
	if (latexila.active_doc == NULL)
		return FALSE;

	GtkTextBuffer *buffer = GTK_TEXT_BUFFER (latexila.active_doc->source_buffer);

	GtkTextIter iter;

	gtk_text_buffer_get_iter_at_mark (buffer, &iter,
			gtk_text_buffer_get_insert (buffer));

	if (backward)
	{
		// if a match is found
		if (gtk_source_iter_backward_search (&iter, what, flags, match_start,
					match_end, NULL))
		{
			gtk_text_buffer_select_range (buffer, match_start, match_end);
			scroll_to_cursor ();
			return TRUE;
		}

		else
		{
			GtkTextIter insert = iter;
			gtk_text_buffer_get_end_iter (buffer, &iter);
			if (gtk_source_iter_backward_search (&iter, what, flags,
						match_start, match_end, &insert))
			{
				gtk_text_buffer_select_range (buffer, match_start, match_end);
				scroll_to_cursor ();
				return TRUE;
			}
			
			// match not found
			else
				return FALSE;
		}
	}

	else
	{
		// if a match is found
		if (gtk_source_iter_forward_search (&iter, what, flags, match_start,
					match_end, NULL))
		{
			// the insert mark is set to match_end, so for the next search it
			// begins after the match, otherwise it would find the same match
			gtk_text_buffer_select_range (buffer, match_end, match_start);
			scroll_to_cursor ();
			return TRUE;
		}

		else
		{
			GtkTextIter insert = iter;
			gtk_text_buffer_get_start_iter (buffer, &iter);
			if (gtk_source_iter_forward_search (&iter, what, flags,
						match_start, match_end, &insert))
			{
				gtk_text_buffer_select_range (buffer, match_end, match_start);
				scroll_to_cursor ();
				return TRUE;
			}
			
			// match not found
			else
				return FALSE;
		}
	}
}

static void
change_font_source_view (void)
{
	guint nb_docs = g_list_length (latexila.all_docs);
	for (guint i = 0 ; i < nb_docs ; i++)
	{
		document_t *doc = g_list_nth_data (latexila.all_docs, i);
		gtk_widget_modify_font (doc->source_view, latexila.font_desc);
	}
}

static void
create_preferences (void)
{
	pref_dialog = gtk_dialog_new_with_buttons (_("Preferences"),
			latexila.main_window, 0,
			GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE, NULL);
	gtk_dialog_set_has_separator (GTK_DIALOG (pref_dialog), FALSE);

	g_signal_connect (G_OBJECT (pref_dialog), "response",
			G_CALLBACK (cb_pref_dialog_close), NULL);
	g_signal_connect (G_OBJECT (pref_dialog), "destroy",
			G_CALLBACK (gtk_widget_destroyed), &pref_dialog);

	GtkWidget *content_area = gtk_dialog_get_content_area (
			GTK_DIALOG (pref_dialog));
	gtk_box_set_spacing (GTK_BOX (content_area), 3);

	/* show line numbers */
	GtkWidget *line_numbers = gtk_check_button_new_with_label (
			_("Display line numbers"));
	gboolean tmp = g_key_file_get_boolean (latexila.key_file, PROGRAM_NAME,
			"show_line_numbers", NULL);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (line_numbers), tmp);
	g_signal_connect (G_OBJECT (line_numbers), "toggled",
			G_CALLBACK (cb_line_numbers), NULL);
	gtk_box_pack_start (GTK_BOX (content_area), line_numbers, FALSE, FALSE, 0);

	/* font */
	GtkWidget *hbox = gtk_hbox_new (FALSE, 5);
	GtkWidget *label = gtk_label_new (_("Font:"));
	gchar *current_font = g_key_file_get_string (latexila.key_file, PROGRAM_NAME,
			"font", NULL);
	GtkWidget *font_button = gtk_font_button_new_with_font (current_font);
	g_signal_connect (G_OBJECT (font_button), "font-set",
			G_CALLBACK (cb_font_set), NULL);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), font_button, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (content_area), hbox, FALSE, FALSE, 0);

	/* command view entry */
	hbox = gtk_hbox_new (FALSE, 5);
	label = gtk_label_new (_("Program for viewing documents:"));
	GtkWidget *command_view_entry = gtk_entry_new ();
	gchar *txt = g_key_file_get_string (latexila.key_file, PROGRAM_NAME,
			"command_view", NULL);
	gtk_entry_set_text (GTK_ENTRY (command_view_entry), txt);
	GtkWidget *button = gtk_button_new_with_label (_("OK"));
	g_signal_connect (G_OBJECT (button), "clicked",
			G_CALLBACK (cb_command_view), GTK_ENTRY (command_view_entry));
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), command_view_entry, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (content_area), hbox, FALSE, FALSE, 0);


	gtk_widget_show_all (content_area);
}
