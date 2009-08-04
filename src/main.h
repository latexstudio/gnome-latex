#ifndef MAIN_H
#define MAIN_H

#define _(STRING) gettext(STRING)

#define PROGRAM_NAME "LaTeXila"
#define PROGRAM_VERSION "0.0.1"

// each document opened is represented by a document_t structure
typedef struct
{
	gchar *path;
	gboolean saved;
	GtkTextView *text_view;
} document_t;

typedef struct
{
	GList *all;
	document_t *active;
	GtkWindow *main_window;
	GtkNotebook *notebook;
} docs_t;

// all the documents are accessible by the docs variable
extern docs_t docs;

#endif /* MAIN_H */
