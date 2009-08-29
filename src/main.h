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

#ifdef LATEXILA_NLS_ENABLED
#define _(STRING) gettext(STRING)
#else
#define _(STRING) STRING
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
	gint				font_size;
	gboolean			show_line_numbers;
} latexila_t;

// all the documents are accessible by the docs variable
extern latexila_t latexila;

// for the actions list in the log zone
enum
{
	COLUMN_ACTION_TITLE,
	COLUMN_ACTION_COMMAND,
	COLUMN_ACTION_COMMAND_OUTPUT,
	COLUMN_ACTION_ERROR,
	N_COLUMNS_ACTION
};

#endif /* MAIN_H */
