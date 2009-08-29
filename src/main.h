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
