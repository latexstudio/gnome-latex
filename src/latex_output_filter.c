/*
 * This file is part of LaTeXila.
 *
 * Copyright © 2010 Sébastien Wilmet
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
#include <string.h>
#include <ctype.h> // for isspace()
#include <gtk/gtk.h>

#include "main.h"
#include "print.h"
#include "latex_output_filter.h"
#include "log.h"

static gboolean detect_badbox (const gchar *line);
static gboolean detect_badbox_line (const gchar *badbox,
		gboolean current_line_is_empty);

static gboolean detect_warning (const gchar *line);
static gboolean detect_warning_line (const gchar *warning,
		gboolean current_line_is_empty);

static gboolean detect_error (const gchar *line);
static gboolean detect_other (const gchar *line);

static void update_stack_file (const gchar *line);
static void update_stack_file_heuristic (const gchar *line);

static gboolean file_exists (const gchar *filename);
static gchar * get_path_if_file_exists (const gchar *filename);
static gchar * get_current_filename (void);
static void push_file_on_stack (gchar *filename, gboolean reliable);
static void pop_file_from_stack (void);
static gboolean top_file_on_stack_reliable (void);
static void print_msg (void);

// the current message
static message_t msg;

// if a message is splitted, we enter in a different status, so we fetch the end
// of the message
static enum filter_status status = START;

// if a message is splitted, the lines are concatenated in this buffer
static gchar line_buf[BUFFER_SIZE] = "";
static gint nb_lines = 0;

// the stack containing the files that TeX is processing
static GSList *stack_file = NULL;

// the directory where the document is compiled
static const gchar *path = NULL;

// some regex, initialised in latex_output_filter_init() and freed by
// latex_output_filter_free()
static GRegex *reg_badbox = NULL;
static GRegex *reg_badbox_lines = NULL;
static GRegex *reg_badbox_line = NULL;
static GRegex *reg_badbox_output = NULL;

static GRegex *reg_warning = NULL;
static GRegex *reg_warning_no_file = NULL;
static GRegex *reg_warning_line = NULL;
static GRegex *reg_warning_international_line = NULL;

static GRegex *reg_latex_error = NULL;
static GRegex *reg_pdflatex_error = NULL;
static GRegex *reg_tex_error = NULL;
static GRegex *reg_error_line = NULL;

static GRegex *reg_file_pop = NULL;

// for statistics
static gint nb_badboxes = 0;
static gint nb_warnings = 0;
static gint nb_errors = 0;

void
latex_output_filter (const gchar *line)
{
	if (line == NULL)
		return;

	switch (status)
	{
		case START:
			if (strlen (line) == 0)
				return;

			if (! (detect_badbox (line) || detect_warning (line)
						|| detect_error (line) || detect_other (line)))
				update_stack_file (line);
			break;

		case BADBOX:
			detect_badbox (line);
			break;

		case WARNING:
			detect_warning (line);
			break;

		case ERROR:
		case ERROR_SEARCH_LINE:
			detect_error (line);
			break;

		case FILENAME:
		case FILENAME_HEURISTIC:
			update_stack_file (line);

		default:
			status = START;
			break;
	}
}

void
latex_output_filter_init (void)
{
	reg_badbox = g_regex_new ("^(Over|Under)full \\\\[hv]box", 0, 0, NULL);
	reg_badbox_lines = g_regex_new ("(.*) at lines ([0-9]+)--([0-9]+)", 0, 0, NULL);
	reg_badbox_line = g_regex_new ("(.*) at line ([0-9]+)", 0, 0, NULL);
	reg_badbox_output = g_regex_new ("(.*)has occurred while \\output is active", 0, 0, NULL);

	reg_warning = g_regex_new ("^(((! )?(La|pdf)TeX)|Package|Class) .*Warning.*:(.*)",
			G_REGEX_CASELESS, 0, NULL);
	reg_warning_no_file = g_regex_new ("(No file .*)", 0, 0, NULL);
	reg_warning_line = g_regex_new ("(.*) on input line ([0-9]+)\\.$", 0, 0, NULL);
	reg_warning_international_line = g_regex_new ("(.*)([0-9]+)\\.$", 0, 0, NULL);

	reg_latex_error = g_regex_new ("^! LaTeX Error: (.*)$", 0, 0, NULL);
	reg_pdflatex_error = g_regex_new ("^Error: pdflatex (.*)$", 0, 0, NULL);
	reg_tex_error = g_regex_new ("^! (.*)\\.$", 0, 0, NULL);
	reg_error_line = g_regex_new ("^l\\.([0-9]+)(.*)", 0, 0, NULL);

	reg_file_pop = g_regex_new ("(\\) )?:<-$", 0, 0, NULL);

	msg.line = NO_LINE;
}

void
latex_output_filter_free (void)
{
	g_regex_unref (reg_badbox);
	g_regex_unref (reg_badbox_lines);
	g_regex_unref (reg_badbox_line);
	g_regex_unref (reg_badbox_output);
	g_regex_unref (reg_warning);
	g_regex_unref (reg_warning_no_file);
	g_regex_unref (reg_warning_line);
	g_regex_unref (reg_warning_international_line);
	g_regex_unref (reg_latex_error);
	g_regex_unref (reg_pdflatex_error);
	g_regex_unref (reg_tex_error);
	g_regex_unref (reg_error_line);
	g_regex_unref (reg_file_pop);
	g_free ((gpointer) path);
}

void
latex_output_filter_set_path (const gchar *dir)
{
	path = g_strdup (dir);
}

void
latex_output_filter_print_stats (void)
{
	gchar *str = g_strdup_printf ("%d %s, %d %s, %d %s",
			nb_errors, nb_errors > 1 ? "errors" : "error",
			nb_warnings, nb_warnings > 1 ? "warnings" : "warning",
			nb_badboxes, nb_badboxes > 1 ? "badboxes" : "badbox");

	print_output_info (str);
	g_free (str);

	// it's finish, we reset the counters
	nb_badboxes = 0;
	nb_warnings = 0;
	nb_errors = 0;

	// empty the stack file
	gint nb_files_in_stack = g_slist_length (stack_file);
	if (nb_files_in_stack > 0)
		print_warning ("the file stack was not empty!");
	for (int i = 0 ; i < nb_files_in_stack ; i++)
	{
		print_info ("%s", get_current_filename ());
		pop_file_from_stack ();
	}
}

static gboolean
detect_badbox (const gchar *line)
{
	gboolean current_line_is_empty;

	switch (status)
	{
		case START:
			if (g_regex_match (reg_badbox, line, 0, NULL))
			{
				msg.message_type = MESSAGE_TYPE_BADBOX;
				nb_badboxes++;

				if (detect_badbox_line (line, FALSE))
					print_msg ();
				else
				{
					g_strlcpy (line_buf, line, BUFFER_SIZE);
					nb_lines++;
				}

				return TRUE;
			}
			return FALSE;
			break;
		
		case BADBOX:
			g_strlcat (line_buf, line, BUFFER_SIZE);
			nb_lines++;
			current_line_is_empty = strlen (line) == 0;
			if (detect_badbox_line (line_buf, current_line_is_empty))
			{
				print_msg ();
				nb_lines = 0;
			}

			// the return value is not important here
			return TRUE;
			break;

		default:
			break;
	}

	return FALSE;
}

static gboolean
detect_badbox_line (const gchar *badbox, gboolean current_line_is_empty)
{
	if (g_regex_match (reg_badbox_lines, badbox, 0, NULL))
	{
		status = START;
		gchar **strings = g_regex_split (reg_badbox_lines, badbox, 0);
		msg.message = g_strdup (strings[1]);
		long n1 = strtol (strings[2], NULL, 10);
		long n2 = strtol (strings[3], NULL, 10);
		msg.line = n1 < n2 ? n1 : n2;
		g_strfreev (strings);
		return TRUE;
	}

	else if (g_regex_match (reg_badbox_line, badbox, 0, NULL))
	{
		status = START;
		gchar **strings = g_regex_split (reg_badbox_line, badbox, 0);
		msg.message = g_strdup (strings[1]);
		msg.line = strtol (strings[2], NULL, 10);
		g_strfreev (strings);
		return TRUE;
	}

	else if (g_regex_match (reg_badbox_output, badbox, 0, NULL))
	{
		status = START;
		gchar **strings = g_regex_split (reg_badbox_output, badbox, 0);
		msg.message = g_strdup (strings[1]);
		msg.line = NO_LINE;
		g_strfreev (strings);
		return TRUE;
	}

	else if (nb_lines > 4 || current_line_is_empty)
	{
		status = START;
		msg.message = g_strdup (badbox);
		msg.line = NO_LINE;
		return TRUE;
	}

	status = BADBOX;
	return FALSE;
}

static gboolean
detect_warning (const gchar *line)
{
	gboolean current_line_is_empty;
	gchar **strings;

	switch (status)
	{
		case START:
			if (g_regex_match (reg_warning, line, 0, NULL))
			{
				nb_warnings++;
				msg.message_type = MESSAGE_TYPE_WARNING;

				strings = g_regex_split (reg_warning, line, 0);

				if (detect_warning_line (strings[5], FALSE))
					print_msg ();
				else
				{
					g_strlcpy (line_buf, strings[5], BUFFER_SIZE);
					nb_lines++;
				}

				g_strfreev (strings);
				return TRUE;
			}

			else if (g_regex_match (reg_warning_no_file, line, 0, NULL))
			{
				nb_warnings++;
				msg.message_type = MESSAGE_TYPE_WARNING;
				strings = g_regex_split (reg_warning_no_file, line, 0);
				msg.message = g_strdup (strings[1]);
				g_strfreev (strings);
				msg.line = NO_LINE;
				print_msg ();
				return TRUE;
			}

			return FALSE;
			break;

		case WARNING:
			g_strlcat (line_buf, line, BUFFER_SIZE);
			nb_lines++;
			current_line_is_empty = strlen (line) == 0;
			if (detect_warning_line (line_buf, current_line_is_empty))
			{
				print_msg ();
				nb_lines = 0;
			}

			// the return value is not important here
			return TRUE;
			break;

		default:
			break;
	}

	return FALSE;
}

static gboolean
detect_warning_line (const gchar *warning, gboolean current_line_is_empty)
{
	if (g_regex_match (reg_warning_line, warning, 0, NULL))
	{
		status = START;
		gchar **strings = g_regex_split (reg_warning_line, warning, 0);
		msg.message = g_strdup (strings[1]);
		msg.line = strtol (strings[2], NULL, 10);
		g_strfreev (strings);
		return TRUE;
	}
	
	else if (g_regex_match (reg_warning_international_line, warning, 0, NULL))
	{
		status = START;
		gchar **strings = g_regex_split (reg_warning_international_line, warning, 0);
		msg.message = g_strdup (strings[1]);
		msg.line = strtol (strings[2], NULL, 10);
		g_strfreev (strings);
		return TRUE;
	}

	else if (warning[strlen (warning) - 1] == '.')
	{
		status = START;
		msg.message = g_strdup (warning);
		msg.line = NO_LINE;
		return TRUE;
	}

	else if (nb_lines > 5 || current_line_is_empty)
	{
		status = START;
		msg.message = g_strdup (warning);
		msg.line = NO_LINE;
		return TRUE;
	}

	status = WARNING;
	return FALSE;
}

static gboolean
detect_error (const gchar *line)
{
	gboolean found = FALSE;
	gchar *tmp;
	gchar **strings;

	switch (status)
	{
		case START:
			if (g_regex_match (reg_latex_error, line, 0, NULL))
			{
				found = TRUE;
				strings = g_regex_split (reg_latex_error, line, 0);
				tmp = g_strdup (strings[1]);
				g_strfreev (strings);
			}

			else if (g_regex_match (reg_pdflatex_error, line, 0, NULL))
			{
				found = TRUE;
				strings = g_regex_split (reg_pdflatex_error, line, 0);
				tmp = g_strdup (strings[1]);
				g_strfreev (strings);
			}

			else if (g_regex_match (reg_tex_error, line, 0, NULL))
			{
				found = TRUE;
				strings = g_regex_split (reg_tex_error, line, 0);
				tmp = g_strdup (strings[1]);
				g_strfreev (strings);
			}

			if (found)
			{
				nb_errors++;
				nb_lines++;
				msg.message_type = MESSAGE_TYPE_ERROR;

				// the message is complete
				if (line[strlen (line) - 1] == '.')
				{
					msg.message = tmp;
					status = ERROR_SEARCH_LINE;
				}
				// the message is splitted
				else
				{
					g_strlcpy (line_buf, tmp, BUFFER_SIZE);
					g_free (tmp);
					status = ERROR;
				}
				return TRUE;
			}

			return FALSE;
			break;

		case ERROR:
			g_strlcat (line_buf, line, BUFFER_SIZE);
			nb_lines++;

			if (line[strlen (line) - 1] == '.')
			{
				msg.message = g_strdup (line_buf);
				status = ERROR_SEARCH_LINE;
			}
			else if (nb_lines > 4)
			{
				msg.message = g_strdup (line_buf);
				msg.line = NO_LINE;
				print_msg ();
				nb_lines = 0;
				status = START;
			}
			
			// the return value is not important here
			return TRUE;
			break;

		case ERROR_SEARCH_LINE:
			nb_lines++;
			if (g_regex_match (reg_error_line, line, 0, NULL))
			{
				strings = g_regex_split (reg_error_line, line, 0);
				msg.line = strtol (strings[1], NULL, 10);
				g_strfreev (strings);
				print_msg ();
				nb_lines = 0;
				status = START;
				return TRUE;
			}
			else if (nb_lines > 11)
			{
				msg.line = NO_LINE;
				print_msg ();
				nb_lines = 0;
				status = START;
				return TRUE;
			}
			break;

		default:
			break;
	}

	return FALSE;
}

static gboolean
detect_other (const gchar *line)
{
	if (strstr (line, "Output written on"))
	{
		msg.message = g_strdup (line);
		msg.line = NO_LINE;
		msg.message_type = MESSAGE_TYPE_OTHER;
		print_msg ();
		return TRUE;
	}

	return FALSE;
}

// There are basically two ways to detect the current file TeX is processing:
//      1) Use \Input (srctex or srcltx package) and \include exclusively. This will
//      cause (La)TeX to print the line ":<+ filename"  in the log file when opening a file, 
//      ":<-" when closing a file. Filenames pushed on the stack in this mode are marked
//      as reliable.
//
//      2) Since people will probably also use the \input command, we also have to be
//      to detect the old-fashioned way. TeX prints '(filename' when opening a file and a ')'
//      when closing one. It is impossible to detect this with 100% certainty (TeX prints many messages
//      and even text (a context) from the TeX source file, there could be unbalanced parentheses), 
//      so we use a heuristic algorithm. In heuristic mode a ')' will only be considered as a signal that 
//      TeX is closing a file if the top of the stack is not marked as "reliable". 
//      
// The method used here is almost the same as in Kile.
static void
update_stack_file (const gchar *line)
{
	static gchar filename[BUFFER_SIZE] = "";
	gchar *tmp;

	switch (status)
	{
		case START:
		case FILENAME_HEURISTIC:
			// TeX is opening a file
			if (g_str_has_prefix (line, ":<+ "))
			{
				g_strlcpy (filename, line + 4, BUFFER_SIZE);
				g_strstrip (filename);
				status = FILENAME;
			}
			// TeX closed a file
			else if (g_regex_match (reg_file_pop, line, 0, NULL)
					|| g_str_has_prefix (line, ":<-"))
				pop_file_from_stack ();
			// fallback to the heuristic detection of filenames
			else
				update_stack_file_heuristic (line);
			break;

		case FILENAME:
			// The partial filename was followed by '(', this means that TeX is
			// signalling it is opening the file. We are sure the filename is
			// complete now. Don't call update_stack_file_heuristic()
			// since we don't want the filename on the stack twice.
			if (line[0] == '(' || g_str_has_prefix (line, "\\openout"))
			{
				push_file_on_stack (filename, TRUE);
				status = START;
			}

			// The partial filename was followed by a TeX error, meaning the
			// file doesn't exist. Don't push it on the stack, instead try to
			// detect the error.
			else if (line[0] == '!')
			{
				status = START;
				detect_error (line);
			}
			else if (g_str_has_prefix (line, "No file"))
			{
				status = START;
				detect_warning (line);
			}

			// the filename is not complete
			else
			{
				tmp = g_strdup (line);
				g_strstrip (tmp);
				g_strlcat (filename, tmp, BUFFER_SIZE);
				g_free (tmp);
			}
			break;

		default:
			break;
	}
}

static void
update_stack_file_heuristic (const gchar *line)
{
	static gchar filename[BUFFER_SIZE] = "";
	gboolean expect_filename = status == FILENAME_HEURISTIC;
	gint index = 0;
	int length = strlen (line);

	// handle special case
	if (expect_filename && length > 0 && line[0] == ')')
	{
		push_file_on_stack (filename, FALSE);
		expect_filename = FALSE;
		status = START;
	}

	// scan for parentheses and grab filenames
	for (int i = 0 ; i < length ; i++)
	{
		/*
		We're expecting a filename. If a filename really ends at this position,
		one of the following must be true:
			1) Next character is a space, indicating the end of a filename
			  (yes, there can't spaces in the path, this is a TeX limitation).
			2) We're at the end of the line, the filename is probably
			   continued on the next line.
			3) The file was closed already, signalled by the ')'.
		*/

		gboolean is_last_char = i + 1 == length;
		gboolean next_is_terminator =
			is_last_char ? FALSE : (isspace (line[i+1]) || line[i+1] == ')');

		if (expect_filename && (is_last_char || next_is_terminator))
		{
			g_strlcat (filename, line + index, strlen (filename) + i - index + 2);

			if (strlen (filename) == 0)
				continue;

			// by default, an output line is 79 characters max
			if ((is_last_char && i < 78) || next_is_terminator || file_exists (filename))
			{
				push_file_on_stack (filename, FALSE);
				expect_filename = FALSE;
				status = START;
			}

			// Guess the filename is continued on the next line, only if the
			// current filename does not exist
			else if (is_last_char)
			{
				if (file_exists (filename))
				{
					push_file_on_stack (filename, FALSE);
					expect_filename = FALSE;
					status = START;
				}
				else
					status = FILENAME_HEURISTIC;
			}

			// filename not detected
			else
			{
				status = START;
				filename[0] = '\0';
				expect_filename = FALSE;
			}
		}

		// TeX is opening a file
		else if (line[i] == '(')
		{
			status = START;
			filename[0] = '\0';
			// we need to extract the filename
			expect_filename = TRUE;
			// this is where the filename is supposed to start
			index = i + 1;
		}

		// TeX is closing a file
		// If this filename was pushed on the stack by the reliable ":<+-"
		// method, don't pop, a ":<-" will follow. This helps in preventing
		// unbalanced ')' from popping filenames from the stack too soon.
		else if (line[i] == ')' && g_slist_length (stack_file) > 0
				&& ! top_file_on_stack_reliable ())
			pop_file_from_stack ();
	}
}

static gboolean
file_exists (const gchar *filename)
{
	gchar *full_path = get_path_if_file_exists (filename);
	if (full_path != NULL)
	{
		g_free (full_path);
		return TRUE;
	}
	return FALSE;
}

// return NULL if the filename does not exist
// return the path of the filename if it exists (must be freed)
static gchar *
get_path_if_file_exists (const gchar *filename)
{
	if (g_path_is_absolute (filename))
	{
		if (g_file_test (filename, G_FILE_TEST_IS_REGULAR))
			return g_strdup (filename);
		else
			return NULL;
	}

	gchar *full_path;
	if (g_str_has_prefix (filename, "./"))
		full_path = g_build_filename (path, filename + 2, NULL);
	else
		full_path = g_build_filename (path, filename, NULL);

	if (g_file_test (full_path, G_FILE_TEST_IS_REGULAR))
		return full_path;

	// try to add various extensions on the filename to see if the file exists
	gchar *extensions[] = {".tex", ".ltx", ".latex", ".dtx", ".ins"};
	guint nb_ext = G_N_ELEMENTS (extensions);
	for (guint i = 0 ; i < nb_ext ; i++)
	{
		gchar *tmp = g_strdup_printf ("%s%s", full_path, extensions[i]);
		if (g_file_test (tmp, G_FILE_TEST_IS_REGULAR))
		{
			g_free (full_path);
			return tmp;
		}
		g_free (tmp);
	}

	g_free (full_path);
	return NULL;
}

static gchar *
get_current_filename (void)
{
	if (g_slist_length (stack_file) > 0)
	{
		file_in_stack_t *file = stack_file->data;
		return file->filename;
	}

	return NULL;
}

static void
push_file_on_stack (gchar *filename, gboolean reliable)
{
	//print_info ("***\tpush\t\"%s\" (%s)", filename, reliable ? "reliable" : "not reliable");
	file_in_stack_t *file = g_malloc (sizeof (file_in_stack_t));

	gchar *path = get_path_if_file_exists (filename);
	if (path != NULL)
		file->filename = path;
	else
		file->filename = g_strdup (filename);

	file->reliable = reliable;
	stack_file = g_slist_prepend (stack_file, file);
}

static void
pop_file_from_stack (void)
{
	file_in_stack_t *file = stack_file->data;
	//print_info ("***\tpop\t\"%s\" (%s)", file->filename, file->reliable ? "reliable" : "not reliable");
	stack_file = g_slist_remove (stack_file, file);
	g_free (file->filename);
	g_free (file);
}

static gboolean
top_file_on_stack_reliable (void)
{
	// stack_file must contain at least one file
	g_assert (stack_file != NULL);

	file_in_stack_t *file = stack_file->data;
	return file->reliable;
}

static void
print_msg (void)
{
	gchar *filename = get_current_filename ();
	print_output_message (filename, msg.line, msg.message, msg.message_type);
	
	msg.line = NO_LINE;
	msg.message_type = MESSAGE_TYPE_OTHER;
	g_free (msg.message);
}
