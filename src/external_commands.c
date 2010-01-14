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

#define _POSIX_C_SOURCE 1
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h> // for dup2
#include <sys/types.h>
#include <signal.h> // for kill
#include <locale.h>
#include <libintl.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gtksourceview/gtksourceview.h>

#include "main.h"
#include "config.h"
#include "print.h"
#include "utils.h"
#include "external_commands.h"
#include "file_browser.h"
#include "latex_output_filter.h"

static void add_action (const gchar *title, const gchar *command);
static void set_action_sensitivity (gboolean sensitive);
static gchar * get_command_line (gchar **command);
static void start_command_without_output (gchar **command, gchar *message);
static void start_command_with_output (gchar **command);
static void cb_spawn_setup (gpointer data);
static gboolean cb_watch_output_command (GIOChannel *channel,
		GIOCondition condition, gpointer user_data);
//static void output_filter_line (const gchar *line);
static void cb_child_watch (GPid pid, gint status, gpointer user_data);
static void finish_execute (void);
static void run_command_on_other_extension (gchar *title, gchar *message,
		gchar *command, gchar *extension);
static gboolean is_current_doc_tex_file (void);

static gboolean show_all_output = TRUE;
static GPid child_pid;
static gint child_pid_exit_code = 0;
static gboolean exit_code_set = FALSE;
static enum output output_status = OUTPUT_GO_FETCHING;

void
compile_document (gchar *title, gchar **command)
{
	if (latexila.active_doc == NULL)
		return;

	gchar *command_line = get_command_line (command);
	add_action (title, command_line);
	g_free (command_line);

	if (! is_current_doc_tex_file ())
		return;

	/* print a message in the statusbar */
	guint context_id = gtk_statusbar_get_context_id (latexila.statusbar,
			"running-action");
	gtk_statusbar_push (latexila.statusbar, context_id,
			_("Compilation in progress. Please wait..."));

	// without that, the message in the statusbar does not appear
	flush_queue ();

	show_all_output = latexila.prefs.compile_show_all_output;
	start_command_with_output (command);
}

void
view_current_document (gchar *title, gchar *doc_extension)
{
	if (latexila.active_doc == NULL)
		return;

	/* replace .tex by doc_extension (.pdf, .dvi, ...) */
	GRegex *regex = g_regex_new ("\\.tex$", 0, 0, NULL);
	gchar *doc_path = g_regex_replace_literal (regex, latexila.active_doc->path,
			-1, 0, doc_extension, 0, NULL);
	g_regex_unref (regex);

	gchar *command_line = g_strdup_printf ("%s %s", latexila.prefs.command_view,
			doc_path);
	add_action (title, command_line);
	g_free (command_line);

	if (! is_current_doc_tex_file ())
	{
		g_free (doc_path);
		return;
	}

	/* the document (PDF, DVI, ...) file exist? */
	if (! g_file_test (doc_path, G_FILE_TEST_IS_REGULAR))
	{
		gchar *command_output = g_strdup_printf (
				_("%s does not exist. If this is not already made, compile the document with the right command."),
				g_path_get_basename (doc_path));
		print_log_add (latexila.action_log.text_view, command_output, TRUE);
		g_free (command_output);
		g_free (doc_path);
		return;
	}

	gchar *command[] = {latexila.prefs.command_view, doc_path, NULL};
	start_command_without_output (command, NULL);
	g_free (doc_path);
}

void
view_document (gchar *title, gchar *filename)
{
	gchar *command_line = g_strdup_printf ("%s %s", latexila.prefs.command_view,
			filename);
	add_action (title, command_line);
	g_free (command_line);

	gchar *command[] = {latexila.prefs.command_view, filename, NULL};
	start_command_without_output (command, NULL);
}

void
convert_document (gchar *title, gchar *doc_extension, gchar *command)
{
	run_command_on_other_extension (
			title,
			_("Converting in progress. Please wait..."),
			command,
			doc_extension);
}

void
run_bibtex (void)
{
	run_command_on_other_extension (
			"BibTeX",
			_("BibTeX is running. Please wait..."),
			latexila.prefs.command_bibtex,
			".aux");
}

void
run_makeindex (void)
{
	run_command_on_other_extension (
			"MakeIndex",
			_("MakeIndex is running. Please wait..."),
			latexila.prefs.command_makeindex,
			".idx");
}

void
view_in_web_browser (gchar *title, gchar *filename)
{
	gchar *command_line = g_strdup_printf ("%s %s",
			latexila.prefs.command_web_browser, filename);
	add_action (title, command_line);
	g_free (command_line);

	gchar *command[] = {latexila.prefs.command_web_browser, filename, NULL};
	start_command_without_output (command, NULL);
}

void
stop_execution (void)
{
	kill (child_pid, SIGTERM);
	gtk_action_set_sensitive (latexila.actions.stop_execution, FALSE);
}

static void
add_action (const gchar *title, const gchar *command)
{
	static gint num = 1;
	gchar *title2 = g_strdup_printf ("%d. %s", num, title);

	// create a new text buffer
	GtkTextBuffer *new_text_buffer = gtk_text_buffer_new (
			latexila.action_log.tag_table);
	latexila.action_log.text_buffer = new_text_buffer;
	gtk_text_view_set_buffer (latexila.action_log.text_view, new_text_buffer);

	// title
	GtkTextIter end;
	gtk_text_buffer_get_end_iter (new_text_buffer, &end);
	gtk_text_buffer_insert_with_tags_by_name (new_text_buffer, &end, title2, -1,
			"bold", NULL);

	// command
    gtk_text_buffer_get_end_iter (new_text_buffer, &end);
    gchar *command2 = g_strdup_printf ("\n$ %s\n", command);
    gtk_text_buffer_insert (new_text_buffer, &end, command2, -1);
    g_free (command2);

	// append a new entry to the action list
	GtkTreeIter iter;
	gtk_list_store_append (latexila.action_log.list_store, &iter);
	gtk_list_store_set (latexila.action_log.list_store, &iter,
			COLUMN_ACTION_TITLE, title2,
			COLUMN_ACTION_TEXTBUFFER, new_text_buffer,
			-1);

	// select the new entry
	gtk_tree_selection_select_iter (latexila.action_log.list_selection, &iter);

	// scroll to the end
	GtkTreePath *path = gtk_tree_model_get_path (
			GTK_TREE_MODEL (latexila.action_log.list_store), &iter);
	gtk_tree_view_scroll_to_cell (latexila.action_log.list_view, path, NULL,
			FALSE, 0, 0);

	// delete the first entry
	if (num > 5)
	{
		GtkTreeIter first;
		gtk_tree_model_get_iter_first (
				GTK_TREE_MODEL (latexila.action_log.list_store), &first);
		gtk_list_store_remove (latexila.action_log.list_store, &first);
	}

	num++;
	g_free (title2);
}

static void
set_action_sensitivity (gboolean sensitive)
{
	// Lock the action list when a command is running so the user can not view
	// an other action.
	gtk_widget_set_sensitive (GTK_WIDGET (latexila.action_log.list_view), sensitive);

	gtk_action_set_sensitive (latexila.actions.compile_latex, sensitive);
	gtk_action_set_sensitive (latexila.actions.compile_pdflatex, sensitive);
	gtk_action_set_sensitive (latexila.actions.dvi_to_pdf, sensitive);
	gtk_action_set_sensitive (latexila.actions.dvi_to_ps, sensitive);
	gtk_action_set_sensitive (latexila.actions.view_dvi, sensitive);
	gtk_action_set_sensitive (latexila.actions.view_pdf, sensitive);
	gtk_action_set_sensitive (latexila.actions.view_ps, sensitive);
	gtk_action_set_sensitive (latexila.actions.bibtex, sensitive);
	gtk_action_set_sensitive (latexila.actions.makeindex, sensitive);
	gtk_action_set_sensitive (latexila.actions.stop_execution, ! sensitive);
}

static gchar *
get_command_line (gchar **command)
{
	if (command[0] == NULL)
		return NULL;

	gchar *command_line = g_strdup (command[0]);
	gchar *tmp;
	gchar **arg = command;
	arg++;
	while (*arg != NULL)
	{
		tmp = g_strdup_printf ("%s %s", command_line, *arg);
		g_free (command_line);
		command_line = tmp;
		arg++;
	}

	return command_line;
}

static void
start_command_without_output (gchar **command, gchar *message)
{
	GError *error = NULL;
	g_spawn_async (NULL, command, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error);

	if (error != NULL)
	{
		gchar *command_output = g_strdup_printf (_("execution failed: %s"),
				error->message);
		print_log_add (latexila.action_log.text_view, command_output, TRUE);
		g_free (command_output);
		g_error_free (error);
	}
	else if (message == NULL)
		print_log_add (latexila.action_log.text_view,
				_("Viewing in progress. Please wait..."), FALSE);
	else
		print_log_add (latexila.action_log.text_view, message, FALSE);
}

// Attention, before calling this function, set the variable "show_all_output"
// to TRUE or FALSE. If it is FALSE, the output will be filtered by the function
// latex_output_filter().
static void
start_command_with_output (gchar **command)
{
	gchar *dir = g_path_get_dirname (latexila.active_doc->path);
	GError *error = NULL;
	gint out;
	g_spawn_async_with_pipes (dir, command, NULL,
			G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH,
			(GSpawnChildSetupFunc) cb_spawn_setup, NULL,
			&child_pid, NULL, &out, NULL, &error);
	g_free (dir);

	// an error occured
	if (error != NULL)
	{
		gchar *command_output = g_strdup_printf (_("execution failed: %s"),
				error->message);
		print_log_add (latexila.action_log.text_view, command_output, TRUE);
		g_free (command_output);
		g_error_free (error);
		return;
	}

	output_status = OUTPUT_GO_FETCHING;
	exit_code_set = FALSE;

	// we want to know the exit code
	g_child_watch_add (child_pid, (GChildWatchFunc) cb_child_watch, NULL);

	// create the channel
	GIOChannel *out_channel = g_io_channel_unix_new (out);

	// convert the channel
	g_io_channel_set_encoding (out_channel, NULL, NULL);
	//g_io_channel_set_encoding (out_channel, "ISO-8859-1", NULL);

	// lock the action list and all the build actions
	set_action_sensitivity (FALSE);

	// add watches to channels
	g_io_add_watch (out_channel, G_IO_IN | G_IO_HUP,
			(GIOFunc) cb_watch_output_command, NULL);
}

static void
cb_spawn_setup (gpointer data)
{
	// include stderr in the output
	dup2 (STDOUT_FILENO, STDERR_FILENO);
}

static gboolean
cb_watch_output_command (GIOChannel *channel, GIOCondition condition,
		gpointer user_data)
{
	switch (output_status)
	{
		case OUTPUT_GO_FETCHING:
			break;

		case OUTPUT_IS_FETCHING:
			return FALSE;
			break;

		case OUTPUT_STOP_REQUEST:
			g_io_channel_unref (channel);
			finish_execute ();
			return FALSE;
			break;
	}

	output_status = OUTPUT_IS_FETCHING;

	if (condition & G_IO_IN)
	{
		int nb_lines = 0;

		GError *error = NULL;
		gchar *line = NULL;
		GIOStatus gio_status = g_io_channel_read_line (channel, &line, NULL, NULL, &error); 
		while (gio_status == G_IO_STATUS_NORMAL && output_status == OUTPUT_IS_FETCHING)
		{
			gchar *line_utf8 = NULL;
			if (line != NULL)
			{
				line_utf8 = g_locale_to_utf8 (line, -1, NULL, NULL, NULL);
				if (line_utf8 == NULL)
					line_utf8 = g_convert (line, -1, "UTF-8", "ISO-8859-1", NULL, NULL, NULL);
				g_free (line);
			}

			// the line is not showed if it contains bad characters!
			if (line_utf8 != NULL)
			{
				if (show_all_output)
				{
					print_log_add (latexila.action_log.text_view, line_utf8, FALSE);

					/* Flush the queue for the 200 first lines and then every 50 lines.
					 * This is for the fluidity of the output, without that the lines do not
					 * appear directly and it's ugly. But it is very slow, for a command that
					 * execute for example in 10 seconds, it could take 250 seconds (!) if we
					 * flush the queue at each line... But with commands that take 1
					 * second or so there is not a big difference.
					 */
					if (nb_lines < 200 || nb_lines % 50 == 0)
						flush_queue ();
					nb_lines++;
				}

				else
					latex_output_filter (line_utf8);

				g_free (line_utf8);
			}

			gio_status = g_io_channel_read_line (channel, &line, NULL, NULL, &error);
		}

		if (error != NULL)
		{
			print_warning ("IO channel error: %s", error->message);
			g_error_free (error);
		}
		
		if (gio_status == G_IO_STATUS_EOF)
		{
			output_status = OUTPUT_STOP_REQUEST;
			g_io_channel_unref (channel);
			finish_execute ();
			return FALSE;
		}
	}
	
	if (condition & G_IO_HUP)
	{
		output_status = OUTPUT_STOP_REQUEST;
		g_io_channel_unref (channel);
		finish_execute ();
		return FALSE;
	}

	if (output_status == OUTPUT_IS_FETCHING)
		output_status = OUTPUT_GO_FETCHING;

	return TRUE;
}

/*
static void
output_filter_line (const gchar *line)
{
	if (line == NULL || strlen (line) == 0)
		return;

	if (g_regex_match_simple ("[^:]+:[0-9]+:.*", line, 0, 0)
			|| g_regex_match_simple ("lines? [0-9]+", line, 0, 0)
			|| strstr (line, "LaTeX Error")
			|| strstr (line, "Output written on")
			|| strstr (line, "Warning")
			|| strstr (line, "Overfull")
			|| strstr (line, "Underfull"))
	{
		print_log_add (latexila.action_log.text_view, line, FALSE);
		flush_queue ();
	}
}
*/

static void
cb_child_watch (GPid pid, gint status, gpointer user_data)
{
	g_spawn_close_pid (pid);

	if (WIFEXITED (status))
		child_pid_exit_code = WEXITSTATUS (status);
	else
		child_pid_exit_code = -1;

	exit_code_set = TRUE;
	finish_execute ();
}

static void
finish_execute (void)
{
	if (! exit_code_set || output_status != OUTPUT_STOP_REQUEST)
		return;

	if (child_pid_exit_code > -1)
	{
		gchar *exit_code = g_strdup_printf ("exit code: %d\n",
				child_pid_exit_code);

		if (child_pid_exit_code == 0)
			print_log_add (latexila.action_log.text_view, exit_code, FALSE);
		else
			print_log_add (latexila.action_log.text_view, exit_code, TRUE);

		g_free (exit_code);
	}
	else
		print_log_add (latexila.action_log.text_view,
				_("the child process exited abnormally"), TRUE);

	flush_queue ();

	// unlock the action list and all the build actions
	set_action_sensitivity (TRUE);

	// pop the message from the statusbar
	guint context_id = gtk_statusbar_get_context_id (latexila.statusbar,
			"running-action");
	gtk_statusbar_pop (latexila.statusbar, context_id);

	cb_file_browser_refresh (NULL, NULL);
}

/* Run a command on the current document but with an other extension.
 * For example the current document is doc.tex, and we want to run
 * "bibtex doc.aux". In this case, command will be "bibtex" and extension
 * ".aux".
 * The output of the command is displayed.
 */
static void
run_command_on_other_extension (gchar *title, gchar *message, gchar *command,
		gchar *extension)
{
	if (latexila.active_doc == NULL)
		return;

	gchar *command_output;

	/* replace .tex by .aux */
	GRegex *regex = g_regex_new ("\\.tex$", 0, 0, NULL);
	gchar *doc_path = g_regex_replace_literal (regex,
			latexila.active_doc->path, -1, 0, extension, 0, NULL);
	g_regex_unref (regex);

	gchar *full_command = g_strdup_printf ("%s %s", command, doc_path);
	add_action (title, full_command);
	g_free (full_command);

	/* the document with the other extension exist? */
	if (! g_file_test (doc_path, G_FILE_TEST_IS_REGULAR))
	{
		command_output = g_strdup_printf (
				_("%s does not exist. If this is not already made, compile the document with the right command."),
				g_path_get_basename (doc_path));
		print_log_add (latexila.action_log.text_view, command_output, TRUE);
		g_free (command_output);
		g_free (doc_path);
		return;
	}

	/* print a message in the statusbar */
	guint context_id = gtk_statusbar_get_context_id (latexila.statusbar,
			"running-action");
	gtk_statusbar_push (latexila.statusbar, context_id, message);

	// without that, the message in the statusbar does not appear
	flush_queue ();

	/* run the command */
	gchar *argv[] = {command, doc_path, NULL};
	show_all_output = TRUE;
	start_command_with_output (argv);
	g_free (doc_path);
}

static gboolean
is_current_doc_tex_file (void)
{
	/* the current document is a *.tex file? */
	if (! g_str_has_suffix (latexila.active_doc->path, ".tex"))
	{
		gchar *command_output = g_strdup_printf (_("failed: %s is not a *.tex file"),
				g_path_get_basename (latexila.active_doc->path));
		print_log_add (latexila.action_log.text_view, command_output, TRUE);
		g_free (command_output);
		return FALSE;
	}

	return TRUE;
}
