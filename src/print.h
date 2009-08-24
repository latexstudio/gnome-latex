#ifndef PRINT_H
#define PRINT_H

void print_log (GtkTextView *log, gchar *title, gchar *command,
		gchar *command_output, gboolean error);

void print_info (char *, ...);
void print_warning (char *, ...);
void print_error (char *, ...);

#endif /* PRINT_H */
