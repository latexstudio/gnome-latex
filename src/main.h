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

// if GTK+ version is < 2.14.0
#if !GTK_CHECK_VERSION(2, 14, 0)
#	if !defined(gtk_dialog_get_action_area)
#		define gtk_dialog_get_action_area(dialog) GTK_DIALOG(dialog)->action_area
#	endif
#	if !defined(gtk_dialog_get_content_area)
#		define gtk_dialog_get_content_area(dialog) GTK_DIALOG(dialog)->vbox
#	endif
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

typedef struct
{
	GList				*all_docs;
	document_t			*active_doc;
	GtkWindow			*main_window;
	GtkNotebook			*notebook;
	GtkListStore		*list_store;
	GtkTreeView			*list_view;
	GtkTreeSelection	*list_selection;
	GtkTextView			*log;
	GtkStatusbar		*statusbar;
	GtkStatusbar		*cursor_position;
	GtkAction			*undo;
	GtkAction			*redo;
	GKeyFile			*key_file;
	gchar				*pref_file;
	PangoFontDescription *font_desc;
	gint				font_size;
	GtkWidget			*symbol_tables;
	GtkListStore		*symbol_stores[7];
	GtkIconView			*symbol_view;
} latexila_t;

// all the documents are accessible by the "latexila" variable
extern latexila_t latexila;

// for the actions list in the log zone
enum action
{
	COLUMN_ACTION_TITLE,
	COLUMN_ACTION_COMMAND,
	COLUMN_ACTION_COMMAND_OUTPUT,
	COLUMN_ACTION_ERROR,
	N_COLUMNS_ACTION
};

// for the symbol tables
struct symbol
{
	gchar *filename;
	gchar *latex_command;
	gchar *package_required;
};

// for the symbol lists
enum symbols
{
	COLUMN_SYMBOL_PIXBUF,
	COLUMN_SYMBOL_COMMAND,
	COLUMN_SYMBOL_TOOLTIP,
	N_COLUMNS_SYMBOL
};

enum category_symbols
{
	COLUMN_CAT_ICON,
	COLUMN_CAT_NAME,
	COLUMN_CAT_NUM,
	N_COLUMNS_CAT
};

#endif /* MAIN_H */
