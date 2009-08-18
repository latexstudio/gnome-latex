#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>

#include "main.h"

void
print_log (action_t *action)
{
	/* append an entry to the action list */
	GtkTreeIter iter;
	gtk_list_store_append (latexila.list_store, &iter);
	gtk_list_store_set (latexila.list_store, &iter, COLUMN_ACTION_TITLE,
			action->title, -1);

	/* show the details */
	GtkTextBuffer *log_buffer = gtk_text_view_get_buffer (latexila.log);
	gtk_text_buffer_set_text (log_buffer, "", -1);

	GtkTextIter end;

	// title (in bold)
	gtk_text_buffer_get_end_iter (log_buffer, &end);
	gtk_text_buffer_insert_with_tags_by_name (log_buffer, &end,
			action->title, -1, "bold", NULL);

	// command
	gtk_text_buffer_get_end_iter (log_buffer, &end);
	gchar *command = g_strdup_printf ("\n$ %s\n", action->command);
	gtk_text_buffer_insert (log_buffer, &end, command, -1);
	g_free (command);

	// command output
	gtk_text_buffer_get_end_iter (log_buffer, &end);
	gtk_text_buffer_insert (log_buffer, &end, action->command_output, -1);
}

void
print_info (char *format, ...)
{
	va_list va;
	va_start (va, format);
	vprintf (format, va);
	printf ("\n");
}

void
print_warning (char *format, ...)
{
	va_list va;
	va_start (va, format);
	fprintf (stderr, "Warning: ");
	vfprintf (stderr, format, va);
	fprintf (stderr, "\n");
}

void
print_error (char *format, ...)
{
	va_list va;
	va_start (va, format);
	fprintf (stderr, "Error: ");
	vfprintf (stderr, format, va);
	fprintf (stderr, "\n");
	exit (EXIT_FAILURE);
}
