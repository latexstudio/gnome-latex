#ifndef MAIN_H
#define MAIN_H

#define _(STRING) gettext(STRING)

#define PROGRAM_NAME "LaTeXila"
#define PROGRAM_VERSION "0.0.1"

#define FONT "Monospace"

// each document opened is represented by a document_t structure
typedef struct
{
	gchar *path;
	gboolean saved;
	GtkWidget *source_view;
	GtkSourceBuffer *source_buffer;
} document_t;

typedef struct
{
	GList *all;
	document_t *active;
	GtkWindow *main_window;
	GtkNotebook *notebook;
	GtkTextView *log;
	GtkSourceLanguageManager *lm;
	GtkAction *undo;
	GtkAction *redo;
} docs_t;

// all the documents are accessible by the docs variable
extern docs_t docs;

#endif /* MAIN_H */
