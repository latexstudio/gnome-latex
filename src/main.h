#ifndef MAIN_H
#define MAIN_H

#define _(STRING) gettext(STRING)

#define PROGRAM_NAME "LaTeXila"
#define PROGRAM_VERSION "0.0.1"

#define FONT "Monospace"

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
	GList			*all_docs;
	document_t		*active_doc;
	GtkWindow		*main_window;
	GtkNotebook		*notebook;
	GtkListStore		*list_store;
	GtkTreeView		*list_view;
	GtkTreeSelection	*list_selection;
	GtkTextView		*log;
	GtkAction		*undo;
	GtkAction		*redo;
} latexila_t;

// all the documents are accessible by the docs variable
extern latexila_t latexila;

// for the actions list in the log zone
enum
{
	COLUMN_ACTION_TITLE,
	COLUMN_ACTION_COMMAND,
	COLUMN_ACTION_COMMAND_OUTPUT,
	N_COLUMNS_ACTION
};

#endif /* MAIN_H */
