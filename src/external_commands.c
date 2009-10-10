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
#include "external_commands.h"
#include "print.h"

static gchar * get_command_line (gchar **command);
static void command_running_finished (void);
static gboolean cb_watch_output_command (GIOChannel *channel,
		GIOCondition condition, gpointer user_data);
static void add_action (gchar *title, gchar *command);

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
	// the magic formula
	while (gtk_events_pending ())
		gtk_main_iteration ();

	// unlock the action list
	gtk_widget_set_sensitive (GTK_WIDGET (latexila.action_log.list_view), TRUE);

	// pop the message from the statusbar
	guint context_id = gtk_statusbar_get_context_id (latexila.statusbar,
			"running-action");
	gtk_statusbar_pop (latexila.statusbar, context_id);
}

static gboolean
cb_watch_output_command (GIOChannel *channel, GIOCondition condition,
		gpointer user_data)
{
	static int nb_lines = 0;

	if (condition == G_IO_HUP)
	{
		g_io_channel_unref (channel);
		command_running_finished ();
		nb_lines = 0;
		return FALSE;
	}
	
	GError *error = NULL;
	gchar *line;
	g_io_channel_read_line (channel, &line, NULL, NULL, &error);

	if (error != NULL)
	{
		print_warning ("read line from output command failed: %s", error->message);
		g_error_free (error);
		return FALSE;
	}

	if (line != NULL)
	{
		// print the command output line to the log zone
		print_log_add (latexila.action_log.text_view, line, FALSE);
	}
	
	/* Apply the magic formula for the 200 first lines and then every 20 lines.
	 * This is for the fluidity of the output, without that the lines do not
	 * appear directly and it's ugly. But it is very slow, for a command that
	 * execute for example in 10 seconds, it could take 250 seconds (!) if we
	 * apply the magic formula for each line... But with commands that take 1
	 * second or so there is not a big difference.
	 */
	if (nb_lines < 200 || nb_lines % 20 == 0)
	{
		while (gtk_events_pending ())
			gtk_main_iteration ();
	}

	nb_lines++;

	return TRUE;
}

void
compile_document (gchar *title, gchar **command)
{
	if (latexila.active_doc == NULL)
		return;

	gchar *command_output;

	gchar *command_line = get_command_line (command);
	add_action (title, command_line);
	print_info ("execution of the command: %s", command_line);
	g_free (command_line);

	/* the current document is a *.tex file? */
	gboolean tex_file = g_str_has_suffix (latexila.active_doc->path, ".tex");
	if (! tex_file)
	{
		command_output = g_strdup_printf (_("compilation failed: %s is not a *.tex file"),
				g_path_get_basename (latexila.active_doc->path));
		print_log_add (latexila.action_log.text_view, command_output, TRUE);
		g_free (command_output);
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
	gint out, err;
	g_spawn_async_with_pipes (dir, command, NULL, G_SPAWN_SEARCH_PATH, NULL,
			NULL, &pid, NULL, &out, &err, &error);
	g_free (dir);

	// an error occured
	if (error != NULL)
	{
		command_output = g_strdup_printf (_("execution failed: %s"),
				error->message);
		print_log_add (latexila.action_log.text_view, command_output, TRUE);

		g_free (command_output);
		g_error_free (error);
		return;
	}

	// create the channels
	GIOChannel *out_channel = g_io_channel_unix_new (out);
	GIOChannel *err_channel = g_io_channel_unix_new (err);

	// the encoding of the output of the latex and the pdflatex commands is not
	// UTF-8...
	g_io_channel_set_encoding (out_channel, "ISO-8859-1", &error);
	g_io_channel_set_encoding (err_channel, "ISO-8859-1", &error);

	if (error != NULL)
	{
		command_output = g_strdup_printf (
				"conversion of the command output failed: %s", error->message);
		print_log_add (latexila.action_log.text_view, command_output, TRUE);

		g_free (command_output);
		g_error_free (error);
		return;
	}

	// Lock the action list so the user can not view an other action while the
	// compilation is running.
	// It will be unlock when the compilation is finished.
	gtk_widget_set_sensitive (GTK_WIDGET (latexila.action_log.list_view),
			FALSE);

	// add watches to channels
	g_io_add_watch (out_channel, G_IO_IN | G_IO_HUP,
			(GIOFunc) cb_watch_output_command, NULL);
	g_io_add_watch (err_channel, G_IO_IN | G_IO_HUP,
			(GIOFunc) cb_watch_output_command, NULL);
}

void
view_document (gchar *title, gchar *doc_extension)
{
	if (latexila.active_doc == NULL)
		return;

	gchar *command_output;

	GRegex *regex = g_regex_new ("\\.tex$", 0, 0, NULL);

	/* replace .tex by doc_extension (.pdf, .dvi, ...) */
	gchar *doc_path = g_regex_replace_literal (regex, latexila.active_doc->path,
			-1, 0, doc_extension, 0, NULL);

	gchar *command_line = g_strdup_printf ("%s %s", latexila.prefs.command_view,
			doc_path);
	add_action (title, command_line);
	print_info ("execution of the command: %s", command_line);
	g_free (command_line);

	/* the current document is a *.tex file? */
	gboolean tex_file = g_regex_match (regex, latexila.active_doc->path, 0, NULL);
	g_regex_unref (regex);

	if (! tex_file)
	{
		command_output = g_strdup_printf (_("failed: %s is not a *.tex file"),
				g_path_get_basename (latexila.active_doc->path));
		print_log_add (latexila.action_log.text_view, command_output, TRUE);

		g_free (command_output);
		g_free (doc_path);
		return;
	}

	/* the document (PDF, DVI, ...) file exist? */
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

	/* run the command */
	// we use here g_spawn_async () and not g_spawn_command_line_async ()
	// because the spaces in doc_path are not escaped, so with the command line
	// it doesn't work fine...
	
	GError *error = NULL;
	gchar *argv[] = {latexila.prefs.command_view, doc_path, NULL};
	g_spawn_async (NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error);

	gboolean is_error = TRUE;

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

	print_log_add (latexila.action_log.text_view, command_output, is_error);

	g_free (command_output);
	g_free (doc_path);
}

void
convert_document (gchar *title, gchar *doc_extension, gchar *command)
{
	if (latexila.active_doc == NULL)
		return;

	gchar *command_output;

	GRegex *regex = g_regex_new ("\\.tex$", 0, 0, NULL);

	/* replace .tex by doc_extension (.pdf, .dvi, ...) */
	gchar *doc_path = g_regex_replace_literal (regex,
			latexila.active_doc->path, -1, 0, doc_extension, 0, NULL);
	g_regex_unref (regex);

	gchar *full_command = g_strdup_printf ("%s %s", command, doc_path);
	add_action (title, full_command);
	print_info ("execution of the command: %s", full_command);
	g_free (full_command);

	/* the document to convert exist? */
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
	gtk_statusbar_push (latexila.statusbar, context_id,
			_("Converting in progress. Please wait..."));

	// without that, the message in the statusbar does not appear
	while (gtk_events_pending ())
		gtk_main_iteration ();

	/* run the command */
	gchar *argv[] = {command, doc_path, NULL};

	GError *error = NULL;
	gchar *dir = g_path_get_dirname (latexila.active_doc->path);
	GPid pid;
	gint out, err;
	g_spawn_async_with_pipes (dir, argv, NULL, G_SPAWN_SEARCH_PATH, NULL,
			NULL, &pid, NULL, &out, &err, &error);

	g_free (dir);
	g_free (doc_path);

	// an error occured
	if (error != NULL)
	{
		command_output = g_strdup_printf (_("execution failed: %s"),
				error->message);
		print_log_add (latexila.action_log.text_view, command_output, TRUE);

		g_free (command_output);
		g_error_free (error);
		return;
	}

	// create the channels
	GIOChannel *out_channel = g_io_channel_unix_new (out);
	GIOChannel *err_channel = g_io_channel_unix_new (err);

	// Lock the action list so the user can not view an other action while the
	// compilation is running.
	// It will be unlock when the compilation is finished.
	gtk_widget_set_sensitive (GTK_WIDGET (latexila.action_log.list_view),
			FALSE);

	// add watches to channels
	g_io_add_watch (out_channel, G_IO_IN | G_IO_HUP,
			(GIOFunc) cb_watch_output_command, NULL);
	g_io_add_watch (err_channel, G_IO_IN | G_IO_HUP,
			(GIOFunc) cb_watch_output_command, NULL);
}

static void
add_action (gchar *title, gchar *command)
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

	// append an new entry to the action list
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

	num++;
	g_free (title2);
}
