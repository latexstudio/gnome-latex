/*
 * This file is part of LaTeXila.
 *
 * Copyright © 2009, 2010 Sébastien Wilmet
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

#ifndef MAIN_H
#define MAIN_H

#include "config.h"

// if Native Language Support is enabled
#ifdef LATEXILA_NLS_ENABLED
#	define _(STRING) gettext(STRING)
#else
#	define _(STRING) (STRING)
#endif

#ifndef N_
#	define N_(STRING) (STRING)
#endif

// each document opened is represented by a document_t structure
typedef struct
{
	gchar			*path;
	gboolean		saved;
	GtkWidget		*source_view;
	GtkSourceBuffer	*source_buffer;
	GtkWidget		*title;
} document_t;

// the log zone
typedef struct
{
	GtkListStore		*list_store;
	GtkTreeView			*list_view;
	GtkTreeSelection	*list_selection;
	GtkTextView			*text_view;
	GtkTextBuffer		*text_buffer;
	GtkTextTagTable		*tag_table;
} action_log_t;

// symbols tables
typedef struct
{
	GtkWidget		*vbox;
	GtkListStore	*list_stores[7];
	GtkIconView		*icon_view;
} symbols_t;

// file browser
typedef struct
{
	GtkWidget		*vbox;
	GtkListStore	*list_store;
} file_browser_t;

// preferences, settings
typedef struct
{
	gboolean				show_line_numbers;
	gboolean				show_side_pane;
	gboolean				show_edit_toolbar;
	gint					window_width;
	gint					window_height;
	gboolean				window_maximised;
	gint					main_hpaned_pos;
	gint					vpaned_pos;
	gint					log_hpaned_pos;
	gchar					*font_str;
	PangoFontDescription 	*font_desc;
	gint					font_size;
	gchar					*command_view;
	gchar					*command_latex;
	gchar					*command_pdflatex;
	gchar					*command_dvipdf;
	gchar					*command_dvips;
	gchar					*command_web_browser;
	gchar					*command_bibtex;
	gchar					*command_makeindex;
	gboolean				compile_show_all_output;
	gboolean				compile_non_stop;
	gchar					*file_chooser_dir;
	gchar					*file_browser_dir;
	gboolean				file_browser_show_all_files;
	GPtrArray				*list_opened_docs;
	gboolean				reopen_files_on_startup;
	gboolean				delete_aux_files;
	gchar					*style_scheme_id;
	gint					tab_width;
	gboolean				spaces_instead_of_tabs;
	gboolean				highlight_current_line;
	gboolean				highlight_matching_brackets;
	gboolean				toolbars_horizontal;
	gint					side_pane_page;
} preferences_t;

typedef struct
{
	GtkAction *undo;
	GtkAction *redo;
	GtkAction *compile_latex;
	GtkAction *compile_pdflatex;
	GtkAction *dvi_to_pdf;
	GtkAction *dvi_to_ps;
	GtkAction *view_dvi;
	GtkAction *view_pdf;
	GtkAction *view_ps;
	GtkAction *bibtex;
	GtkAction *makeindex;
	GtkAction *stop_execution;
} actions_t;

typedef struct
{
	GtkWidget	*go_to_line;
	GtkWidget	*go_to_line_entry;
	GtkWidget	*find;
	GtkWidget	*find_entry;
	GtkWidget	*find_match_case;
	GtkWidget	*replace;
	GtkWidget	*replace_entry_search;
	GtkWidget	*replace_entry_replace;
	GtkWidget	*replace_match_case;
	GtkWidget	*replace_button;
} under_source_view_t;

typedef struct
{
	GList				*all_docs;
	document_t			*active_doc;
	action_log_t		action_log;
	symbols_t			symbols;
	file_browser_t		file_browser;
	preferences_t		prefs;
	actions_t			actions;
	under_source_view_t under_source_view;
	GtkWindow			*main_window;
	GtkNotebook			*notebook;
	GtkStatusbar		*statusbar;
	GtkStatusbar		*cursor_position;
	GtkPaned			*main_hpaned;
	GtkPaned			*vpaned;
	GtkPaned			*log_hpaned;
	GtkWidget			*edit_toolbar;
	GtkWidget			*side_pane;
} latexila_t;

// a lot of things are accessible by the "latexila" variable everywhere in the
// sources (if this file is included)
extern latexila_t latexila;

// for the actions list in the log zone
enum action
{
	COLUMN_ACTION_TITLE,
	COLUMN_ACTION_TEXTBUFFER,
	N_COLUMNS_ACTION
};

#endif /* MAIN_H */
