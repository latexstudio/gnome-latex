/*
 * This file is part of LaTeXila.
 *
 * Copyright © 2009, 2010 Sébastien Wilmet
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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <gtk/gtk.h>

#include "print.h"

void
print_log (GtkTextBuffer *log_buffer, gchar *title, gchar *command,
		gchar *command_output, gboolean error)
{
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
print_log_add (GtkTextView *log, const gchar *text, gboolean error)
{
	GtkTextBuffer *log_buffer = gtk_text_view_get_buffer (log);
	GtkTextIter end;

	// insert the text to the end
	gtk_text_buffer_get_end_iter (log_buffer, &end);
	if (error)
		gtk_text_buffer_insert_with_tags_by_name (log_buffer, &end, text, -1,
				"error", NULL);
	else
		gtk_text_buffer_insert (log_buffer, &end, text, -1);

	// scroll to the end
	gtk_text_buffer_get_end_iter (log_buffer, &end);
	gtk_text_view_scroll_to_iter (log, &end, 0.0, FALSE, 0.0, 1.0);
}

void
print_info (const char *format, ...)
{
	va_list va;
	va_start (va, format);
	vprintf (format, va);
	printf ("\n");
}

void
print_warning (const char *format, ...)
{
	va_list va;
	va_start (va, format);
	fprintf (stderr, "Warning: ");
	vfprintf (stderr, format, va);
	fprintf (stderr, "\n");
}

void
print_error (const char *format, ...)
{
	va_list va;
	va_start (va, format);
	fprintf (stderr, "Error: ");
	vfprintf (stderr, format, va);
	fprintf (stderr, "\n");
	exit (EXIT_FAILURE);
}
