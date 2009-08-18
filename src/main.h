#ifndef MAIN_H
#define MAIN_H

#define _(STRING) gettext(STRING)

#define PROGRAM_NAME "LaTeXila"
#define PROGRAM_VERSION "0.0.1"

#define FONT "Monospace"

// each document opened is represented by a document_t structure
typedef struct
{
	gchar		*path;
	gboolean	saved;
	GtkWidget	*source_view;
	GtkSourceBuffer	*source_buffer;
	GtkWidget	*title;
} document_t;

typedef struct
{
	gchar *title;
	gchar *command;
	gchar *command_output;
} action_t;

typedef struct
{
	GList		*all_docs;
	document_t	*active_doc;
	GList		*all_actions;
	action_t	*active_action;
	GtkWindow	*main_window;
	GtkNotebook	*notebook;
	GtkListStore	*list_store;
	GtkTextView	*log;
	GtkAction	*undo;
	GtkAction	*redo;
} latexila_t;

// all the documents are accessible by the docs variable
extern latexila_t latexila;

// for the actions list in the log zone
enum
{
	COLUMN_ACTION_TITLE,
	N_COLUMNS
};

#endif /* MAIN_H */
