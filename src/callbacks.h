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

#ifndef CALLBACKS_H
#define CALLBACKS_H

void cb_new (void);
void cb_open (void);
void cb_save (void);
void cb_save_as (void);
void cb_close (void);
void cb_close_tab (GtkWidget *widget, GtkWidget *child);
void cb_quit (void);
void cb_undo (void);
void cb_redo (void);
void cb_cut (void);
void cb_copy (void);
void cb_paste (void);
void cb_delete (void);
void cb_select_all (void);
void cb_preferences (void);
void cb_zoom_in (void);
void cb_zoom_out (void);
void cb_zoom_reset (void);
void cb_find (void);
void cb_replace (void);
void cb_latex (void);
void cb_pdflatex (void);
void cb_view_dvi (void);
void cb_view_pdf (void);
void cb_view_ps (void);
void cb_dvi_to_pdf (void);
void cb_dvi_to_ps (void);
void cb_action_list_changed (GtkTreeSelection *selection,
		gpointer user_data);
void cb_about_dialog (void);
void cb_text_changed (GtkWidget *widget, gpointer user_data);
void cb_cursor_moved (GtkTextBuffer *text_buffer, GtkTextIter *location,
		GtkTextMark *mark, gpointer user_data);
void cb_page_change (GtkNotebook *notebook, GtkNotebookPage *page,
		guint page_num, gpointer user_data);
gboolean cb_delete_event (GtkWidget *widget, GdkEvent *event,
		gpointer user_data);
void cb_recent_item_activated (GtkRecentAction *action, gpointer user_data);
void cb_pref_dialog_close (GtkDialog *dialog, gint response_id,
		gpointer user_data);
void cb_line_numbers (GtkToggleButton *toggle_button, gpointer user_data);
void cb_command_view (GtkButton *button, gpointer user_data);
void cb_font_set (GtkFontButton *font_button, gpointer user_data);
void cb_category_symbols_selected (GtkIconView *icon_view, gpointer user_data);
void cb_symbol_selected (GtkIconView *icon_view, gpointer user_data);
void cb_show_symbol_tables (GtkToggleAction *toggle_action, gpointer user_data);

void open_new_document (const gchar *filename, const gchar *uri);
void save_preferences (void);

#endif /* CALLBACKS_H */
