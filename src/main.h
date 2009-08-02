#ifndef MAIN_H
#define MAIN_H

#define _(STRING) gettext(STRING)

#define PROGRAM_NAME "LaTeXila"
#define PROGRAM_VERSION "0.0.1"

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

typedef struct
{
	GtkWidget *window;
} widgets_t;

extern docs_t docs;

#endif /* MAIN_H */
