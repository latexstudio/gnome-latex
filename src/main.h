#ifndef MAIN_H
#define MAIN_H

#define _(STRING) gettext(STRING)

#define PROGRAM_NAME "LaTeXila"
#define PROGRAM_VERSION "0.0.1"

// used for data callbacks
typedef struct
{
	GtkWidget *window;
} widgets_t;

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
} docs_t;

// all the documents are accessible by the docs variable
extern docs_t docs;

#endif /* MAIN_H */
