/*
 * This file is part of LaTeXila.
 *
 * Copyright © 2009 Sébastien Wilmet
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
#include <locale.h>
#include <libintl.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gtksourceview/gtksourceview.h>

#include "main.h"
#include "config.h"
#include "actions.h"
#include "print.h"

static gchar * get_command_line (gchar **command);
static void command_running_finished (void);
static gboolean cb_watch_output_command (GIOChannel *channel,
		GIOCondition condition, gpointer user_data);
static void add_action (gchar *title, gchar *command, gchar *command_output,
		gboolean error);

static GSList *command_output_list = NULL;

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
command_running_finished (void)
{
	// build the string containing all the lines of the command output
	gchar *command_output_string = g_strdup ("");
	gchar *tmp;
	command_output_list = g_slist_reverse (command_output_list);
	guint nb_lines = g_slist_length (command_output_list);
	for (gint i = 0 ; i < nb_lines ; i++)
	{
		gchar *line = g_slist_nth_data (command_output_list, i);
		tmp = g_strdup_printf ("%s%s", command_output_string, line);
		g_free (command_output_string);
		command_output_string = tmp;
	}
	
	// store the string to the action list store
	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected (latexila.action_log->list_selection,
				NULL, &iter))
	{
		gtk_list_store_set (latexila.action_log->list_store, &iter,
				COLUMN_ACTION_COMMAND_OUTPUT, command_output_string,
				-1);
	}
	else
		print_warning ("no action selected in the list => the output command is not saved");

	// unlock the action list
	gtk_widget_set_sensitive (GTK_WIDGET (latexila.action_log->list_view), TRUE);

	// pop the message from the statusbar
	guint context_id = gtk_statusbar_get_context_id (latexila.statusbar,
			"running-action");
	gtk_statusbar_pop (latexila.statusbar, context_id);

	g_free (command_output_string);
	g_slist_foreach (command_output_list, (GFunc) g_free, NULL);
	g_slist_free (command_output_list);
	command_output_list = NULL;
}

static gboolean
cb_watch_output_command (GIOChannel *channel, GIOCondition condition,
		gpointer user_data)
{
	if (condition == G_IO_HUP)
	{
		g_io_channel_unref (channel);
		command_running_finished ();
		return FALSE;
	}
	
	GError *error = NULL;
	gchar *line;
	g_io_channel_read_line(channel, &line, NULL, NULL, &error);

	if (error != NULL)
	{
		print_warning ("read line from output command failed: %s", error->message);
		g_error_free (error);
		return FALSE;
	}

	// print the command output line to the log zone
	print_log_add (latexila.action_log->text_view, line, FALSE);

	// store temporarily the line to the GList
	// We insert the line at the beginning of the list, so we avoid to traverse
	// the entire list. The list is reversed when all elements have been added.
	command_output_list = g_slist_prepend (command_output_list, line);
	
	// the magic formula
	while (gtk_events_pending ())
		gtk_main_iteration ();

	return TRUE;
}

void
compile_document (gchar *title, gchar **command)
{
	if (latexila.active_doc == NULL)
		return;

	gchar *command_line = get_command_line (command);
	gchar *command_output;
	gboolean is_error = TRUE;

	/* the current document is a *.tex file? */
	gboolean tex_file = g_str_has_suffix (latexila.active_doc->path, ".tex");
	if (! tex_file)
	{
		command_output = g_strdup_printf (_("compilation failed: %s is not a *.tex file"),
				g_path_get_basename (latexila.active_doc->path));

		add_action (title, command_line, command_output, is_error);
		g_free (command_output);
		g_free (command_line);
		return;
	}

	/* print a message in the statusbar */
	guint context_id = gtk_statusbar_get_context_id (latexila.statusbar,
			"running-action");
	gtk_statusbar_push (latexila.statusbar, context_id,
			_("Compilation in progress. Please wait..."));

	// without that, the message in the statusbar does not appear
	while (gtk_events_pending ())
		gtk_main_iteration ();

	/* run the command */
	gchar *dir = g_path_get_dirname (latexila.active_doc->path);
	GError *error = NULL;
	GPid pid;
	gint out;
	g_spawn_async_with_pipes (dir, command, NULL, G_SPAWN_SEARCH_PATH, NULL,
			NULL, &pid, NULL, &out, NULL, &error);

	// an error occured
	if (error != NULL)
	{
		command_output = g_strdup_printf (_("execution failed: %s"),
				error->message);
		add_action (title, command_line, command_output, is_error);

		g_free (command_output);
		g_free (command_line);
		g_error_free (error);
		error = NULL;
		return;
	}

	// create the channels
	GIOChannel *out_channel = g_io_channel_unix_new (out);

	// the encoding of the output of the latex and the pdflatex commands is not
	// UTF-8...
	g_io_channel_set_encoding (out_channel, "ISO-8859-1", &error);

	if (error != NULL)
	{
		command_output = g_strdup_printf (
				"conversion of the command output failed: %s", error->message);
		add_action (title, command_line, command_output, is_error);

		g_free (command_output);
		g_free (command_line);
		g_error_free (error);
		error = NULL;
		return;
	}

	is_error = FALSE;

	// Lock the action list so the user can not view an other action while the
	// compilation is running.
	// It will be unlock when the compilation is finished.
	gtk_widget_set_sensitive (GTK_WIDGET (latexila.action_log->list_view),
			FALSE);

	add_action (title, command_line, "", is_error);

	// All the lines of the command output will be store temporarily in a GSList.
	// When the command is finished, the GSList is traversed and the full string
	// is store to the action list.
	// We use here a GSList and not a GArray or a GList because insertions
	// of new elements must be as fast as possible, and the list is iterated in
	// only one direction.

	// add watches to channels
	g_io_add_watch (out_channel, G_IO_IN | G_IO_HUP,
			(GIOFunc) cb_watch_output_command, NULL);

	g_free (command_line);
	g_free (dir);
}

void
view_document (gchar *title, gchar *doc_extension)
{
	if (latexila.active_doc == NULL)
		return;

	gchar *command, *command_output;
	gboolean is_error = TRUE;

	GError *error = NULL;
	GRegex *regex = g_regex_new ("\\.tex$", 0, 0, NULL);

	/* replace .tex by doc_extension (.pdf, .dvi, ...) */
	gchar *doc_path = g_regex_replace_literal (regex, latexila.active_doc->path,
			-1, 0, doc_extension, 0, NULL);

	command = g_strdup_printf ("%s %s", latexila.prefs->command_view, doc_path);

	/* the current document is a *.tex file? */
	gboolean tex_file = g_regex_match (regex, latexila.active_doc->path, 0, NULL);
	if (! tex_file)
	{
		command_output = g_strdup_printf (_("failed: %s is not a *.tex file"),
				g_path_get_basename (latexila.active_doc->path));

		add_action (title, command, command_output, is_error);
		g_free (command);
		g_free (command_output);
		g_free (doc_path);
		g_regex_unref (regex);
		return;
	}

	/* the document (PDF, DVI, ...) file exist? */
	if (! g_file_test (doc_path, G_FILE_TEST_IS_REGULAR))
	{
		command_output = g_strdup_printf (
				_("%s does not exist. If this is not already made, compile the document with the right command."),
				g_path_get_basename (doc_path));

		add_action (title, command, command_output, is_error);
		g_free (command);
		g_free (command_output);
		g_free (doc_path);
		g_regex_unref (regex);
		return;
	}

	/* run the command */
	print_info ("execution of the command: %s", command);
	g_spawn_command_line_async (command, &error);

	if (error != NULL)
	{
		command_output = g_strdup_printf (_("execution failed: %s"),
				error->message);
		g_error_free (error);
		error = NULL;
	}
	else
	{
		command_output = g_strdup (_("Viewing in progress. Please wait..."));
		is_error = FALSE;
	}

	add_action (title, command, command_output, is_error);

	g_free (command);
	g_free (command_output);
	g_free (doc_path);
	g_regex_unref (regex);
}

void
convert_document (gchar *title, gchar *doc_extension, gchar *command)
{
	if (latexila.active_doc == NULL)
		return;

	gchar *full_command, *command_output;
	gboolean is_error = TRUE;

	GError *error = NULL;
	GRegex *regex = g_regex_new ("\\.tex$", 0, 0, NULL);

	/* replace .tex by doc_extension (.pdf, .dvi, ...) */
	gchar *doc_path = g_regex_replace_literal (regex,
			latexila.active_doc->path, -1, 0, doc_extension, 0, NULL);

	full_command = g_strdup_printf ("%s %s", command, doc_path);

	/* the document to convert exist? */
	if (! g_file_test (doc_path, G_FILE_TEST_IS_REGULAR))
	{
		command_output = g_strdup_printf (
				_("%s does not exist. If this is not already made, compile the document with the right command."),
				g_path_get_basename (doc_path));

		add_action (title, full_command, command_output, is_error);
		g_free (full_command);
		g_free (command_output);
		g_free (doc_path);
		g_regex_unref (regex);
		return;
	}

	/* print a message in the statusbar */
	guint context_id = gtk_statusbar_get_context_id (latexila.statusbar,
			"running-action");
	gtk_statusbar_push (latexila.statusbar, context_id,
			_("Converting in progress. Please wait..."));

	// without that, the message in the statusbar does not appear
	while (gtk_events_pending ())
		gtk_main_iteration ();

	/* run the command in the directory where the .tex is */
	print_info ("execution of the command: %s", full_command);

	gchar *dir_backup = g_get_current_dir ();
	gchar *dir = g_path_get_dirname (latexila.active_doc->path);
	g_chdir (dir);
	g_spawn_command_line_sync (full_command, &command_output, NULL, NULL, &error);
	g_chdir (dir_backup);
	
	// an error occured
	if (error != NULL)
	{
		command_output = g_strdup_printf (_("execution failed: %s"),
				error->message);
		g_error_free (error);
		error = NULL;
	}
	else
		is_error = FALSE;

	add_action (title, full_command, command_output, is_error);

	gtk_statusbar_pop (latexila.statusbar, context_id);

	g_free (full_command);
	g_free (command_output);
	g_free (doc_path);
	g_regex_unref (regex);
}

static void
add_action (gchar *title, gchar *command, gchar *command_output, gboolean error)
{
	static gint num = 1;
	gchar *title2 = g_strdup_printf ("%d. %s", num, title);

	// append an new entry to the action list
	GtkTreeIter iter;
	gtk_list_store_append (latexila.action_log->list_store, &iter);
	gtk_list_store_set (latexila.action_log->list_store, &iter,
			COLUMN_ACTION_TITLE, title2,
			COLUMN_ACTION_COMMAND, command,
			COLUMN_ACTION_COMMAND_OUTPUT, command_output,
			COLUMN_ACTION_ERROR, error,
			-1);

	// the new entry is selected
	// cb_action_list_changed () is called, so the details are showed
	gtk_tree_selection_select_iter (latexila.action_log->list_selection, &iter);

	// scroll to the end
	GtkTreePath *path = gtk_tree_model_get_path (
			GTK_TREE_MODEL (latexila.action_log->list_store), &iter);
	gtk_tree_view_scroll_to_cell (latexila.action_log->list_view, path, NULL,
			FALSE, 0, 0);

	num++;
	g_free (title2);
}

