#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <gtk/gtk.h>

#include "print.h"

void
print_log (GtkTextView *log, gchar *title, gchar *command,
		gchar *command_output, gboolean error)
{
	GtkTextBuffer *log_buffer = gtk_text_view_get_buffer (log);
	gtk_text_buffer_set_text (log_buffer, "", -1);

	GtkTextIter end;

	// title (in bold)
	gtk_text_buffer_get_end_iter (log_buffer, &end);
	gtk_text_buffer_insert_with_tags_by_name (log_buffer, &end,
			title, -1, "bold", NULL);

	// command
	gtk_text_buffer_get_end_iter (log_buffer, &end);
	gchar *command2 = g_strdup_printf ("\n$ %s\n", command);
	gtk_text_buffer_insert (log_buffer, &end, command2, -1);
	g_free (command2);

	// command output
	gtk_text_buffer_get_end_iter (log_buffer, &end);
	if (error)
		gtk_text_buffer_insert_with_tags_by_name (log_buffer, &end,
				command_output, -1, "error", NULL);
	else
		gtk_text_buffer_insert (log_buffer, &end, command_output, -1);
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
