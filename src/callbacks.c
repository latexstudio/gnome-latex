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
#include <gtksourceview/gtksourcestylescheme.h>
#include <gtksourceview/gtksourcestyleschememanager.h>

#include "main.h"
#include "config.h"
#include "callbacks.h"
#include "print.h"
#include "external_commands.h"
#include "prefs.h"
#include "file_browser.h"

static void close_document (gint index);
static void save_as_dialog (void);
static void file_save (void);
static gboolean close_all (void);
static void set_title (void);
static void set_undo_redo_sensitivity (void);
static void update_cursor_position_statusbar (void);
static void scroll_to_cursor (void);
static void find (gboolean backward);
static gboolean find_next_match (const gchar *what, GtkSourceSearchFlags flags,
		gboolean backward, GtkTextIter *match_start, GtkTextIter *match_end);
static void free_latexila (void);
static void delete_auxiliaries_files (const gchar *filename);
static void set_entry_background (GtkWidget *entry, gboolean error);
static void insert_text_at_beginning_of_selected_lines (gchar *text);

static gboolean save_list_opened_docs = FALSE;

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

	if (latexila.prefs.file_chooser_dir != NULL)
		gtk_file_chooser_set_current_folder_uri (GTK_FILE_CHOOSER (dialog),
				latexila.prefs.file_chooser_dir);

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

	/* save the current folder */
	g_free (latexila.prefs.file_chooser_dir);
	latexila.prefs.file_chooser_dir = gtk_file_chooser_get_current_folder_uri (
			GTK_FILE_CHOOSER (dialog));

	gtk_widget_destroy (dialog);
}

void
cb_save (void)
{
	if (latexila.active_doc == NULL)
		return;

	if (! latexila.active_doc->saved || latexila.active_doc->path == NULL)
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
		*latexila.active_doc = doc_backup;
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
			latexila.active_doc = g_list_nth_data (latexila.all_docs,
					gtk_notebook_get_current_page (latexila.notebook));
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
	// remember the files opened at exit for reopening them on next startup
	// latexila.prefs.list_opened_docs is filled in close_document ()
	// (wich is called indirectly by calling close_all ())
	save_list_opened_docs = TRUE;

	if (close_all ())
	{
		save_preferences (&latexila.prefs);
		free_latexila ();
		gtk_main_quit ();
	}

	// free the list of opened documents
	else
	{
		g_ptr_array_free (latexila.prefs.list_opened_docs, TRUE);
		latexila.prefs.list_opened_docs = g_ptr_array_new ();
		save_list_opened_docs = FALSE;
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

		if (! gtk_text_buffer_get_modified (GTK_TEXT_BUFFER (latexila.active_doc->source_buffer)))
		{
			latexila.active_doc->saved = TRUE;
			set_title ();
		}
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

		if (! gtk_text_buffer_get_modified (GTK_TEXT_BUFFER (latexila.active_doc->source_buffer)))
		{
			latexila.active_doc->saved = TRUE;
			set_title ();
		}
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
cb_zoom_in (void)
{
	latexila.prefs.font_size += PANGO_SCALE;
	pango_font_description_set_size (latexila.prefs.font_desc,
			latexila.prefs.font_size);
	change_font_source_view ();
}

void
cb_zoom_out (void)
{
	latexila.prefs.font_size -= PANGO_SCALE;
	pango_font_description_set_size (latexila.prefs.font_desc,
			latexila.prefs.font_size);
	change_font_source_view ();
}

void
cb_zoom_reset (void)
{
	set_current_font_prefs (&latexila.prefs);
	change_font_source_view ();
}

void
cb_find (void)
{
	if (latexila.active_doc == NULL)
		return;

	// reset the background color
	set_entry_background (latexila.under_source_view.find_entry, FALSE);

	gtk_widget_show_all (latexila.under_source_view.find);
	gtk_widget_grab_focus (latexila.under_source_view.find_entry);
}

void
cb_close_find (GtkWidget *widget, gpointer user_data)
{
	gtk_widget_hide (latexila.under_source_view.find);

	guint context_id = gtk_statusbar_get_context_id (latexila.statusbar, "find");
	gtk_statusbar_pop (latexila.statusbar, context_id);
	
	if (latexila.active_doc == NULL)
		return;

	gtk_widget_grab_focus (latexila.active_doc->source_view);
}

void
cb_find_entry (GtkEntry *entry, gpointer user_data)
{
	find (FALSE);
}

void
cb_find_next (GtkWidget *widget, gpointer user_data)
{
	find (FALSE);
}

void
cb_find_previous (GtkWidget *widget, gpointer user_data)
{
	find (TRUE);
}

void
cb_close_replace (GtkWidget *widget, gpointer user_data)
{
	gtk_widget_hide (latexila.under_source_view.replace);

	guint context_id = gtk_statusbar_get_context_id (latexila.statusbar,
			"replace");
	gtk_statusbar_pop (latexila.statusbar, context_id);
	
	if (latexila.active_doc == NULL)
		return;

	gtk_widget_grab_focus (latexila.active_doc->source_view);
}

void
cb_replace_find (GtkWidget *widget, gpointer user_data)
{
	if (latexila.active_doc == NULL)
		return;

	guint context_id = gtk_statusbar_get_context_id (latexila.statusbar,
			"replace");

	const gchar *what = gtk_entry_get_text (
			GTK_ENTRY (latexila.under_source_view.replace_entry_search));

	gboolean tmp = gtk_toggle_button_get_active (
			GTK_TOGGLE_BUTTON (latexila.under_source_view.replace_match_case));
	GtkSourceSearchFlags flags = tmp ? 0 : GTK_SOURCE_SEARCH_CASE_INSENSITIVE;

	GtkTextIter match_start, match_end;

	gboolean match_found = find_next_match (what, flags, FALSE, &match_start,
			&match_end);
	if (! match_found)
	{
		gtk_statusbar_pop (latexila.statusbar, context_id);
		gtk_statusbar_push (latexila.statusbar, context_id,
				_("Phrase not found"));

		gtk_widget_set_sensitive (latexila.under_source_view.replace_button,
				FALSE);

		// red background
		set_entry_background (latexila.under_source_view.replace_entry_search,
				TRUE);
	}
	else
	{
		gtk_statusbar_pop (latexila.statusbar, context_id);

		gtk_widget_set_sensitive (latexila.under_source_view.replace_button,
				TRUE);

		// normal background
		set_entry_background (latexila.under_source_view.replace_entry_search,
				FALSE);
	}
}

void
cb_replace_replace (GtkWidget *widget, gpointer user_data)
{
	if (latexila.active_doc == NULL)
		return;

	GtkTextBuffer *buffer = GTK_TEXT_BUFFER (latexila.active_doc->source_buffer);

	const gchar *replacement = gtk_entry_get_text (
			GTK_ENTRY (latexila.under_source_view.replace_entry_replace));

	GtkTextIter match_start, match_end;
	gtk_text_buffer_get_selection_bounds (buffer, &match_start, &match_end);

	gtk_text_buffer_begin_user_action (buffer);
	gtk_text_buffer_delete (buffer, &match_start, &match_end);
	gtk_text_buffer_insert (buffer, &match_start, replacement, -1);
	gtk_text_buffer_end_user_action (buffer);

	cb_replace_find (NULL, NULL);
}

void
cb_replace_replace_all (GtkWidget *widget, gpointer user_data)
{
	if (latexila.active_doc == NULL)
		return;

	const gchar *what = gtk_entry_get_text (
			GTK_ENTRY (latexila.under_source_view.replace_entry_search));
	const gchar *replacement = gtk_entry_get_text (
			GTK_ENTRY (latexila.under_source_view.replace_entry_replace));

	gboolean tmp = gtk_toggle_button_get_active (
			GTK_TOGGLE_BUTTON (latexila.under_source_view.replace_match_case));
	GtkSourceSearchFlags flags = tmp ? 0 : GTK_SOURCE_SEARCH_CASE_INSENSITIVE;

	GtkTextBuffer *buffer = GTK_TEXT_BUFFER (latexila.active_doc->source_buffer);

	GtkTextIter match_start, match_end, iter;

	gtk_text_buffer_begin_user_action (buffer);

	// begin at the start of the buffer
	gtk_text_buffer_get_start_iter (buffer, &iter);
	gint i = 0;
	while (gtk_source_iter_forward_search (&iter, what, flags, &match_start,
				&match_end, NULL))
	{
		// make the replacement
		gtk_text_buffer_delete (buffer, &match_start, &match_end);
		gtk_text_buffer_insert (buffer, &match_start, replacement, -1);

		// iter points to the end of the inserted text
		iter = match_start;

		i++;
	}

	gtk_text_buffer_end_user_action (buffer);

	// normal background (overwritten if the phrase was not found)
	set_entry_background (latexila.under_source_view.replace_entry_search,
			FALSE);

	// print a message in the statusbar
	gchar *msg;
	if (i == 0)
	{
		msg = g_strdup (_("Phrase not found"));

		// red background
		set_entry_background (latexila.under_source_view.replace_entry_search,
				TRUE);
	}
	else if (i == 1)
		msg = g_strdup (_("Found and replaced one occurence"));
	else
		msg = g_strdup_printf (_("Found and replaced %d occurences"), i);

	guint context_id = gtk_statusbar_get_context_id (latexila.statusbar,
			"replace");
	gtk_statusbar_pop (latexila.statusbar, context_id);
	gtk_statusbar_push (latexila.statusbar, context_id, msg);
	g_free (msg);

	// the user must do a find before doing a replace
	gtk_widget_set_sensitive (latexila.under_source_view.replace_button,
			FALSE);
}

void
cb_replace (void)
{
	if (latexila.active_doc == NULL)
		return;

	// reset the background color
	set_entry_background (latexila.under_source_view.replace_entry_search,
			FALSE);

	// the user must do a find before doing a replace
	gtk_widget_set_sensitive (latexila.under_source_view.replace_button,
			FALSE);

	gtk_widget_show_all (latexila.under_source_view.replace);
	gtk_widget_grab_focus (latexila.under_source_view.replace_entry_search);
}

void
cb_go_to_line (void)
{
	if (latexila.active_doc == NULL)
		return;

	// reset the background color
	set_entry_background (latexila.under_source_view.go_to_line_entry, FALSE);

	gtk_widget_show_all (latexila.under_source_view.go_to_line);
	gtk_widget_grab_focus (latexila.under_source_view.go_to_line_entry);
}

void
cb_close_go_to_line (GtkWidget *widget, gpointer user_data)
{
	gtk_widget_hide (latexila.under_source_view.go_to_line);
	
	if (latexila.active_doc == NULL)
		return;

	gtk_widget_grab_focus (latexila.active_doc->source_view);
}

void
cb_go_to_line_entry (GtkEntry *entry, gpointer user_data)
{
	if (latexila.active_doc == NULL)
		return;

	const gchar *txt = gtk_entry_get_text (entry);
	gint num = strtol (txt, NULL, 10);
	GtkTextIter iter;
	GtkTextBuffer *buffer = GTK_TEXT_BUFFER (latexila.active_doc->source_buffer);
	gint nb_lines = gtk_text_buffer_get_line_count (buffer);
	
	if (nb_lines < num)
	{
		set_entry_background (GTK_WIDGET (entry), TRUE);
		return;
	}
	
	gtk_text_buffer_get_iter_at_line (buffer, &iter, --num);
	gtk_text_buffer_place_cursor (buffer, &iter);
	scroll_to_cursor ();
	cb_close_go_to_line (NULL, NULL);
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

	cb_save ();

	gchar *title = _("Compile (latex)");
	gchar *command[] = {
        g_strdup (latexila.prefs.command_latex),
        "-interaction=nonstopmode",
        g_strdup (latexila.active_doc->path),
        NULL
    };

    compile_document (title, command);

    g_free (command[0]);
    g_free (command[2]);
}

void
cb_pdflatex (void)
{
	if (latexila.active_doc == NULL)
		return;

	cb_save ();

	gchar *title = _("Compile (pdflatex)");
	gchar *command[] = {
        g_strdup (latexila.prefs.command_pdflatex),
        "-interaction=nonstopmode",
        g_strdup (latexila.active_doc->path),
        NULL
    };

    compile_document (title, command);

    g_free (command[0]);
    g_free (command[2]);
}

void
cb_view_dvi (void)
{
	if (latexila.active_doc == NULL)
		return;

	view_current_document (_("View DVI"), ".dvi");
}

void
cb_view_pdf (void)
{
	if (latexila.active_doc == NULL)
		return;

	view_current_document (_("View PDF"), ".pdf");
}

void
cb_view_ps (void)
{
	if (latexila.active_doc == NULL)
		return;

	view_current_document (_("View PS"), ".ps");
}

void
cb_dvi_to_pdf (void)
{
	if (latexila.active_doc == NULL)
		return;

	convert_document (_("DVI to PDF"), ".dvi", latexila.prefs.command_dvipdf);
}

void
cb_dvi_to_ps (void)
{
	if (latexila.active_doc == NULL)
		return;

	convert_document (_("DVI to PS"), ".dvi", latexila.prefs.command_dvips);
}

void
cb_bibtex (void)
{
	if (latexila.active_doc == NULL)
		return;

	run_bibtex ();
}

void
cb_makeindex (void)
{
	if (latexila.active_doc == NULL)
		return;

	run_makeindex ();
}

void
cb_tools_comment (void)
{
	if (latexila.active_doc == NULL)
		return;

	insert_text_at_beginning_of_selected_lines ("% ");
}

void
cb_tools_uncomment (void)
{
	if (latexila.active_doc == NULL)
		return;

	GtkTextIter start, end;
	GtkTextBuffer *buffer = GTK_TEXT_BUFFER (latexila.active_doc->source_buffer);
	gtk_text_buffer_get_selection_bounds (buffer, &start, &end);
	
	gint start_line = gtk_text_iter_get_line (&start);
	gint end_line = gtk_text_iter_get_line (&end);

	gint nb_lines = gtk_text_buffer_get_line_count (buffer);

	gtk_text_buffer_begin_user_action (buffer);

	for (gint i = start_line ; i <= end_line ; i++)
	{
		/* get the text of the line */
		gtk_text_buffer_get_iter_at_line (buffer, &start, i);

		// if last line
		if (i == nb_lines - 1)
			gtk_text_buffer_get_end_iter (buffer, &end);
		else
			gtk_text_buffer_get_iter_at_line (buffer, &end, i + 1);

		gchar *line = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

		/* find the first '%' character */
		gint j = 0;
		gint start_delete = -1;
		gint stop_delete = -1;
		while (line[j] != '\0')
		{
			if (line[j] == '%')
			{
				start_delete = j;
				stop_delete = j + 1;
				if (line[j + 1] == ' ')
					stop_delete++;
				break;
			}

			else if (line[j] != ' ' && line[j] != '\t')
				break;

			j++;
		}

		g_free (line);

		if (start_delete == -1)
			continue;

		gtk_text_buffer_get_iter_at_line_offset (buffer, &start, i, start_delete);
		gtk_text_buffer_get_iter_at_line_offset (buffer, &end, i, stop_delete);
		gtk_text_buffer_delete (buffer, &start, &end);
	}

	gtk_text_buffer_end_user_action (buffer);
}

void
cb_tools_indent (void)
{
	if (latexila.active_doc == NULL)
		return;

	if (latexila.prefs.spaces_instead_of_tabs)
	{
		gchar *spaces = g_strnfill (latexila.prefs.tab_width, ' ');
		insert_text_at_beginning_of_selected_lines (spaces);
		g_free (spaces);
	}
	else
		insert_text_at_beginning_of_selected_lines ("\t");
}

void
cb_tools_unindent (void)
{
	if (latexila.active_doc == NULL)
		return;

	GtkTextIter start, end;
	GtkTextBuffer *buffer = GTK_TEXT_BUFFER (latexila.active_doc->source_buffer);
	gtk_text_buffer_get_selection_bounds (buffer, &start, &end);
	
	gint start_line = gtk_text_iter_get_line (&start);
	gint end_line = gtk_text_iter_get_line (&end);

	gint nb_lines = gtk_text_buffer_get_line_count (buffer);

	gtk_text_buffer_begin_user_action (buffer);

	for (gint i = start_line ; i <= end_line ; i++)
	{
		/* get the text of the line */
		gtk_text_buffer_get_iter_at_line (buffer, &start, i);

		// if last line
		if (i == nb_lines - 1)
			gtk_text_buffer_get_end_iter (buffer, &end);
		else
			gtk_text_buffer_get_iter_at_line (buffer, &end, i + 1);

		gchar *line = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

		/* find the characters to delete */
		gint j = 0;
		gint start_delete = -1;
		gint stop_delete = -1;
		gint nb_spaces = 0;
		while (line[j] != '\0')
		{
			if (line[j] == ' ')
			{
				if (nb_spaces == 0)
					start_delete = j;
				nb_spaces++;
				if (nb_spaces % latexila.prefs.tab_width == 0)
					nb_spaces = 0;
			}
			else if (line[j] == '\t')
			{
				start_delete = j;
				nb_spaces = 0;
			}
			else
			{
				stop_delete = j;
				break;
			}

			j++;
		}

		g_free (line);

		if (start_delete == -1)
			continue;

		gtk_text_buffer_get_iter_at_line_offset (buffer, &start, i, start_delete);
		gtk_text_buffer_get_iter_at_line_offset (buffer, &end, i, stop_delete);
		gtk_text_buffer_delete (buffer, &start, &end);
	}

	gtk_text_buffer_end_user_action (buffer);
}

void
cb_documents_save_all (void)
{
	GList *current = latexila.all_docs;
	while (current != NULL)
	{
		document_t *current_doc = g_list_nth_data (current, 0);
		document_t *active_doc = latexila.active_doc;
		latexila.active_doc = current_doc;
		cb_save ();
		latexila.active_doc = active_doc;

		current = g_list_next (current);
	}
}

void
cb_documents_close_all (void)
{
	close_all ();
}

void
cb_documents_previous (void)
{
	if (latexila.active_doc == NULL)
		return;

	gint current_page = gtk_notebook_get_current_page (latexila.notebook);
	gint nb_pages = gtk_notebook_get_n_pages (latexila.notebook);

	// if first page, go to the last
	if (current_page == 0)
		gtk_notebook_set_current_page (latexila.notebook, nb_pages - 1);
	else
		gtk_notebook_set_current_page (latexila.notebook, current_page - 1);
}

void
cb_documents_next (void)
{
	if (latexila.active_doc == NULL)
		return;

	gint current_page = gtk_notebook_get_current_page (latexila.notebook);
	gint nb_pages = gtk_notebook_get_n_pages (latexila.notebook);

	// if last page, go to the first
	if (current_page == nb_pages - 1)
		gtk_notebook_set_current_page (latexila.notebook, 0);
	else
		gtk_notebook_set_current_page (latexila.notebook, current_page + 1);
}

void
cb_action_list_changed (GtkTreeSelection *selection, gpointer user_data)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	if (gtk_tree_selection_get_selected (selection, &model, &iter))
	{
		GtkTextBuffer *text_buffer;
		gtk_tree_model_get (model, &iter,
				COLUMN_ACTION_TEXTBUFFER, &text_buffer,
				-1);

		gtk_text_view_set_buffer (latexila.action_log.text_view, text_buffer);
	}
}

void
cb_help_latex_reference (void)
{
	view_in_web_browser (_("View LaTeX Reference"), DATA_DIR "/latexhelp.html");
}

void
cb_about_dialog (void)
{
	gchar *comments = _("LaTeXila is a LaTeX editor for the GNOME Desktop");
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

	gchar *website = "http://latexila.sourceforge.net/";

	const gchar *authors[] =
	{
		"Sébastien Wilmet <sebastien.wilmet@gmail.com>",
		NULL
	};

	const gchar *artists[] =
	{
		"Eric Forgeot <e.forgeot@laposte.net>",
		"Sébastien Wilmet <sebastien.wilmet@gmail.com>",
		"The Kile Team: http://kile.sourceforge.net/",
		"Gedit LaTeX Plugin: http://www.michaels-website.de/gedit-latex-plugin/",
		NULL
	};

	GdkPixbuf *logo = gdk_pixbuf_new_from_file (DATA_DIR "/images/logo.png",
			NULL);

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
			"logo", logo,
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
cb_page_reordered (GtkNotebook *notebook, GtkWidget *child, guint page_num,
		gpointer user_data)
{
	// keep the same order in the GList all_docs than the tabs
	latexila.all_docs = g_list_remove (latexila.all_docs, latexila.active_doc);
	latexila.all_docs = g_list_insert (latexila.all_docs, latexila.active_doc,
			page_num);
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
cb_show_side_pane (GtkToggleAction *toggle_action, gpointer user_data)
{
	if (latexila.side_pane == NULL)
		return;

	latexila.prefs.show_side_pane =
		gtk_toggle_action_get_active (toggle_action);

	if (latexila.prefs.show_side_pane)
		gtk_widget_show_all (latexila.side_pane);
	else
		gtk_widget_hide (latexila.side_pane);
}

void
cb_show_edit_toolbar (GtkToggleAction *toggle_action, gpointer user_data)
{
	if (latexila.edit_toolbar == NULL)
		return;

	latexila.prefs.show_edit_toolbar =
		gtk_toggle_action_get_active (toggle_action);

	if (latexila.prefs.show_edit_toolbar)
		gtk_widget_show_all (latexila.edit_toolbar);
	else
		gtk_widget_hide (latexila.edit_toolbar);
}

void
open_new_document_without_uri (const gchar *filename)
{
	GError *error = NULL;

	gchar *uri = g_filename_to_uri (filename, NULL, &error);
	if (error != NULL)
	{
		print_warning ("can not open the file \"%s\": %s", filename,
				error->message);
		g_error_free (error);
	}
	else
		open_new_document (filename, uri);

	g_free (uri);
}

void
open_new_document (const gchar *filename, const gchar *uri)
{
	/* check if the document is not already opened */
	GList *current = latexila.all_docs;
	gint n = 0;
	while (current != NULL)
	{
		document_t *current_doc = g_list_nth_data (current, 0);

		// if the filename is the same, just go to that tab on the notebook
		if (current_doc->path != NULL &&
				strcmp (filename, current_doc->path) == 0)
		{
			gtk_notebook_set_current_page (latexila.notebook, n);
			return;
		}

		current = g_list_next (current);
		n++;
	}

	/* get the contents */
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
	{
		print_warning ("impossible to open the file \"%s\"", filename);

		GtkWidget *dialog = gtk_message_dialog_new (
				latexila.main_window,
				GTK_DIALOG_MODAL,
				GTK_MESSAGE_WARNING,
				GTK_BUTTONS_CLOSE,
				_("Impossible to open the file \"%s\"."), filename);
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
	}
}

void
change_font_source_view (void)
{
	if (latexila.active_doc == NULL)
		return;

	// traverse the list
	// an other solution is to call g_list_foreach ()
	GList *current = latexila.all_docs;
	while (current != NULL)
	{
		document_t *doc = g_list_nth_data (current, 0);
		gtk_widget_modify_font (doc->source_view, latexila.prefs.font_desc);
		current = g_list_next (current);
	}
}

void
create_document_in_new_tab (const gchar *path, const gchar *text,
		const gchar *title)
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
	}

	// the default file is a LaTeX document
	else
	{
		GtkSourceLanguage *lang = gtk_source_language_manager_get_language (
				lm, "latex");
		gtk_source_buffer_set_language (new_doc->source_buffer, lang);
	}

	// set the font
	gtk_widget_modify_font (new_doc->source_view, latexila.prefs.font_desc);

	// enable text wrapping (between words only)
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (new_doc->source_view),
			GTK_WRAP_WORD);

	// show line numbers?
	gtk_source_view_set_show_line_numbers (
			GTK_SOURCE_VIEW (new_doc->source_view),
			latexila.prefs.show_line_numbers);

	// set the style scheme
	GtkSourceStyleSchemeManager *style_scheme_manager =
		gtk_source_style_scheme_manager_get_default ();
	GtkSourceStyleScheme *style_scheme =
		gtk_source_style_scheme_manager_get_scheme (style_scheme_manager,
				latexila.prefs.style_scheme_id);
	gtk_source_buffer_set_style_scheme (new_doc->source_buffer, style_scheme);

	// indentation
	gtk_source_view_set_auto_indent (GTK_SOURCE_VIEW (new_doc->source_view), TRUE);
	gtk_source_view_set_indent_width (GTK_SOURCE_VIEW (new_doc->source_view), -1);
	gtk_source_view_set_tab_width (GTK_SOURCE_VIEW (new_doc->source_view),
			latexila.prefs.tab_width);
	gtk_source_view_set_insert_spaces_instead_of_tabs (
			GTK_SOURCE_VIEW (new_doc->source_view),
			latexila.prefs.spaces_instead_of_tabs);

	// highlight
	gtk_source_view_set_highlight_current_line (
			GTK_SOURCE_VIEW (new_doc->source_view),
			latexila.prefs.highlight_current_line);
	gtk_source_buffer_set_highlight_matching_brackets (new_doc->source_buffer,
			latexila.prefs.highlight_matching_brackets);

	// put the text into the buffer
	gtk_source_buffer_begin_not_undoable_action (new_doc->source_buffer);
	gtk_text_buffer_set_text (GTK_TEXT_BUFFER (new_doc->source_buffer),
			text, -1);
	gtk_source_buffer_end_not_undoable_action (new_doc->source_buffer);

	// move the cursor at the start
	GtkTextIter start;
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (new_doc->source_buffer),
			&start);
	gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER (new_doc->source_buffer),
			&start);

	// when the text is modified
	g_signal_connect (G_OBJECT (new_doc->source_buffer), "changed",
			G_CALLBACK (cb_text_changed), NULL);

	// when the cursor is moved
	g_signal_connect (G_OBJECT (new_doc->source_buffer), "mark-set",
			G_CALLBACK (cb_cursor_moved), NULL);

	// the buffer is saved
	gtk_text_buffer_set_modified (GTK_TEXT_BUFFER (new_doc->source_buffer),
			FALSE);

	// with a scrollbar
	GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (sw), GTK_WIDGET (new_doc->source_view));
	gtk_widget_show_all (sw);

	// add the new document in a new tab
	GtkWidget *hbox = gtk_hbox_new (FALSE, 3);

	GtkWidget *label = gtk_label_new (title);
	new_doc->title = label;
	gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);

	GtkWidget *close_button = gtk_button_new ();

	// apply the style defined with gtk_rc_parse_string
	// the button will be smaller
	gtk_widget_set_name (close_button, "my-close-button");

	gtk_button_set_relief (GTK_BUTTON (close_button), GTK_RELIEF_NONE);
	GtkWidget *image = gtk_image_new_from_stock (GTK_STOCK_CLOSE,
			GTK_ICON_SIZE_MENU);
	gtk_container_add (GTK_CONTAINER (close_button), image);
	g_signal_connect (G_OBJECT (close_button), "clicked",
			G_CALLBACK (cb_close_tab), sw);

	gtk_box_pack_start (GTK_BOX (hbox), close_button, FALSE, FALSE, 0);
	gtk_widget_show_all (hbox);

	gint index = gtk_notebook_append_page (latexila.notebook, sw, hbox);
	gtk_notebook_set_tab_reorderable (latexila.notebook, sw, TRUE);
	gtk_notebook_set_current_page (latexila.notebook, index);
	
	set_undo_redo_sensitivity ();
	update_cursor_position_statusbar ();
}


/******************************************************************************
 * local functions
 *****************************************************************************/

static void
close_document (gint index)
{
	if (latexila.active_doc == NULL)
		return;

	/* if the document is not saved, ask the user for saving changes before
	 * closing */
	if (! latexila.active_doc->saved)
	{
		gchar *doc_name;
		if (latexila.active_doc->path == NULL)
			doc_name = g_strdup (_("New document"));
		else
			doc_name = g_path_get_basename (latexila.active_doc->path);

		GtkWidget *dialog = gtk_message_dialog_new (
				latexila.main_window,
				GTK_DIALOG_MODAL,
				GTK_MESSAGE_QUESTION,
				GTK_BUTTONS_NONE,
				_("Save changes to \"%s\" before closing?"),
				doc_name);
		g_free (doc_name);

		gtk_dialog_add_buttons (GTK_DIALOG (dialog),
				GTK_STOCK_YES, GTK_RESPONSE_YES,
				GTK_STOCK_NO, GTK_RESPONSE_NO,
				GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				NULL);

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
		if (save_list_opened_docs)
			g_ptr_array_add (latexila.prefs.list_opened_docs,
					(gpointer) g_strdup (latexila.active_doc->path));

		delete_auxiliaries_files (latexila.active_doc->path);

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
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL
	);

	if (latexila.prefs.file_chooser_dir != NULL)
		gtk_file_chooser_set_current_folder_uri (GTK_FILE_CHOOSER (dialog),
				latexila.prefs.file_chooser_dir);

	while (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		gchar *filename = gtk_file_chooser_get_filename (
				GTK_FILE_CHOOSER (dialog));
		
		/* if the file exists, ask the user whether the file can be replaced */
		if (g_file_test (filename, G_FILE_TEST_EXISTS))
		{
			GtkWidget *confirmation = gtk_message_dialog_new (
					latexila.main_window,
					GTK_DIALOG_MODAL,
					GTK_MESSAGE_QUESTION,
					GTK_BUTTONS_NONE,
					_("A file named \"%s\" already exists. Do you want to replace it?"),
					g_path_get_basename (filename));

			gtk_dialog_add_button (GTK_DIALOG (confirmation),
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);

			// button replace created by hand because we use the icon "save as"
			// with the text "replace"
			GtkWidget *button_replace = gtk_button_new_with_label (_("Replace"));
			GtkWidget *icon = gtk_image_new_from_stock (GTK_STOCK_SAVE_AS,
					GTK_ICON_SIZE_BUTTON);
			gtk_button_set_image (GTK_BUTTON (button_replace), icon);
			gtk_dialog_add_action_widget (GTK_DIALOG (confirmation),
					button_replace, GTK_RESPONSE_YES);
			gtk_widget_show (button_replace);

			gint response = gtk_dialog_run (GTK_DIALOG (confirmation));
			gtk_widget_destroy (confirmation);

			// return to the file chooser dialog so the user can choose an other
			// file
			if (response != GTK_RESPONSE_YES)
			{
				g_free (filename);
				continue;
			}
		}

		latexila.active_doc->path = filename;
		gchar *uri = gtk_file_chooser_get_uri (GTK_FILE_CHOOSER (dialog));

		GtkRecentManager *manager = gtk_recent_manager_get_default ();
		gtk_recent_manager_add_item (manager, uri);

		break;
	}

	/* save the current folder */
	g_free (latexila.prefs.file_chooser_dir);
	latexila.prefs.file_chooser_dir = gtk_file_chooser_get_current_folder_uri (
			GTK_FILE_CHOOSER (dialog));

	gtk_widget_destroy (dialog);
}

static void
file_save (void)
{
	if (latexila.active_doc->path == NULL)
		return;

	GError *error = NULL;
	GtkTextBuffer *text_buffer = GTK_TEXT_BUFFER (latexila.active_doc->source_buffer);
	GtkTextIter start, end;
	gtk_text_buffer_get_bounds (text_buffer, &start, &end);
	gchar *contents = gtk_text_buffer_get_text (text_buffer, &start, &end, FALSE);
	gchar *locale = g_locale_from_utf8 (contents, -1, NULL, NULL, &error);
	g_free (contents);

	if (error != NULL)
	{
		print_warning ("impossible to convert the contents: %s", error->message);
		g_error_free (error);
		return;
	}

	g_file_set_contents (latexila.active_doc->path, locale, -1, &error);

	if (error != NULL)
	{
		print_warning ("impossible to save the file: %s", error->message);
		g_error_free (error);
		return;
	}

	latexila.active_doc->saved = TRUE;
	gtk_text_buffer_set_modified (
			GTK_TEXT_BUFFER (latexila.active_doc->source_buffer), FALSE);
	g_free (locale);
}

static gboolean
close_all (void)
{
	while (latexila.active_doc != NULL)
	{
		gint tmp = gtk_notebook_get_n_pages (latexila.notebook);
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

	gtk_action_set_sensitive (latexila.actions.undo, can_undo);
	gtk_action_set_sensitive (latexila.actions.redo, can_redo);
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

static void
find (gboolean backward)
{
	if (latexila.active_doc == NULL)
		return;

	guint context_id = gtk_statusbar_get_context_id (latexila.statusbar, "find");

	const gchar *what = gtk_entry_get_text (GTK_ENTRY (latexila.under_source_view.find_entry));
	gboolean tmp = gtk_toggle_button_get_active (
			GTK_TOGGLE_BUTTON (latexila.under_source_view.find_match_case));
	GtkSourceSearchFlags flags = tmp ? 0 : GTK_SOURCE_SEARCH_CASE_INSENSITIVE;

	GtkTextIter match_start, match_end;
	if (! find_next_match (what, flags, backward, &match_start, &match_end))
	{
		// print a message in the statusbar
		gtk_statusbar_pop (latexila.statusbar, context_id);
		gtk_statusbar_push (latexila.statusbar, context_id,
				_("Phrase not found"));

		// red background
		set_entry_background (latexila.under_source_view.find_entry, TRUE);
	}

	else
	{
		// we can possibly print a message, anyway we must pop the last message
		gtk_statusbar_pop (latexila.statusbar, context_id);

		// normal background
		set_entry_background (latexila.under_source_view.find_entry, FALSE);
	}
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
free_latexila (void)
{
	g_free (latexila.prefs.font_str);
	g_free (latexila.prefs.command_view);
	g_free (latexila.prefs.command_latex);
	g_free (latexila.prefs.command_pdflatex);
	g_free (latexila.prefs.command_dvipdf);
	g_free (latexila.prefs.command_dvips);
	g_free (latexila.prefs.command_web_browser);
	g_free (latexila.prefs.command_bibtex);
	g_free (latexila.prefs.command_makeindex);
	g_free (latexila.prefs.file_chooser_dir);
	g_free (latexila.prefs.file_browser_dir);
	g_free (latexila.prefs.style_scheme_id);
	g_ptr_array_free (latexila.prefs.list_opened_docs, TRUE);

	for (int i = 0 ; i < 7 ; i++)
		g_object_unref (latexila.symbols.list_stores[i]);
}

static void
delete_auxiliaries_files (const gchar *filename)
{
	if (! g_str_has_suffix (filename, ".tex"))
		return;

	gchar *extensions[] = {".aux", ".bit", ".blg", ".lof", ".log", ".lot",
		".glo", ".glx", ".gxg", ".gxs", ".idx", ".ilg", ".ind", ".out", ".url",
		".svn", ".toc"};
	gint nb_extensions = G_N_ELEMENTS (extensions);

	gchar *filename_without_ext = g_strndup (filename, strlen (filename) - 4);

	for (int i = 0 ; i < nb_extensions ; i++)
	{
		gchar *tmp = g_strdup_printf ("%s%s", filename_without_ext,
				extensions[i]);
		g_remove (tmp);
		g_free (tmp);
	}

	g_free (filename_without_ext);
	cb_file_browser_refresh (NULL, NULL);
}

static void
set_entry_background (GtkWidget *entry, gboolean error)
{
	if (error)
	{
		GdkColor red, white;
		gdk_color_parse ("#FF6666", &red);
		gdk_color_parse ("white", &white);
		gtk_widget_modify_base (entry, GTK_STATE_NORMAL, &red);
		gtk_widget_modify_text (entry, GTK_STATE_NORMAL, &white);
	}

	else
	{
		gtk_widget_modify_base (entry, GTK_STATE_NORMAL, NULL);
		gtk_widget_modify_text (entry, GTK_STATE_NORMAL, NULL);
	}
}

static void
insert_text_at_beginning_of_selected_lines (gchar *text)
{
	if (latexila.active_doc == NULL)
		return;

	GtkTextIter start, end;
	GtkTextBuffer *buffer = GTK_TEXT_BUFFER (latexila.active_doc->source_buffer);
	gtk_text_buffer_get_selection_bounds (buffer, &start, &end);
	
	gint start_line = gtk_text_iter_get_line (&start);
	gint end_line = gtk_text_iter_get_line (&end);

	gtk_text_buffer_begin_user_action (buffer);
	for (gint i = start_line ; i <= end_line ; i++)
	{
		GtkTextIter iter;
		gtk_text_buffer_get_iter_at_line (buffer, &iter, i);
		gtk_text_buffer_insert (buffer, &iter, text, -1);
	}
	gtk_text_buffer_end_user_action (buffer);
}
