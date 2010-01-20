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

#include <gtk/gtk.h>

#include "main.h"
#include "log.h"

static void cb_action_history_changed (GtkTreeSelection *selection,
		gpointer user_data);
static GtkListStore * get_new_output_list_store (void);
static void scroll_to_end (GtkTreeIter *iter);

static GtkTreeView *history_view;
static GtkTreeView *output_view;

void
init_log_zone (GtkPaned *log_hpaned)
{
	/* action history */
	{
		GtkListStore *history_list_store = gtk_list_store_new (N_COLS_ACTION,
				G_TYPE_STRING, G_TYPE_POINTER);
		
		history_view = GTK_TREE_VIEW (gtk_tree_view_new_with_model (
					GTK_TREE_MODEL (history_list_store)));
		GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
		GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes (
				_("Action history"), renderer, "text", COL_ACTION_TITLE, NULL);	
		gtk_tree_view_append_column (history_view, column);
		
		GtkTreeSelection *select = gtk_tree_view_get_selection (history_view);
		gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
		g_signal_connect (G_OBJECT (select), "changed",
				G_CALLBACK (cb_action_history_changed), NULL);

		// with a scrollbar
		GtkWidget *scrollbar = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollbar),
				GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_container_add (GTK_CONTAINER (scrollbar), GTK_WIDGET (history_view));
		gtk_paned_add1 (log_hpaned, scrollbar);
	}
	
	/* log details */
	{
		GtkListStore *output_list_store = get_new_output_list_store ();

		// tree view with 3 columns: basename, line, message
		output_view = GTK_TREE_VIEW (gtk_tree_view_new_with_model (
					GTK_TREE_MODEL (output_list_store)));
		g_object_unref (output_list_store);

		// we can now show some text (output_view must be initialized)
		print_output_normal (_("Welcome to LaTeXila!"));

		gtk_tree_view_set_headers_visible (output_view, FALSE);
		gtk_tree_view_set_tooltip_column (output_view, COL_OUTPUT_FILENAME);

		GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
		g_object_set (renderer, "weight-set", TRUE, NULL);

		// basename
		GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes (
				NULL, renderer,
				"text", COL_OUTPUT_BASENAME,
				"foreground", COL_OUTPUT_COLOR,
				"foreground-set", COL_OUTPUT_COLOR_SET,
				"weight", COL_OUTPUT_WEIGHT,
				NULL);
		gtk_tree_view_append_column (output_view, column);

		// line number
		column = gtk_tree_view_column_new_with_attributes (
				NULL, renderer,
				"text", COL_OUTPUT_LINE_NUMBER,
				"foreground", COL_OUTPUT_COLOR,
				"foreground-set", COL_OUTPUT_COLOR_SET,
				"weight", COL_OUTPUT_WEIGHT,
				NULL);
		gtk_tree_view_append_column (output_view, column);

		// message
		column = gtk_tree_view_column_new_with_attributes (
				NULL, renderer,
				"text", COL_OUTPUT_MESSAGE,
				"foreground", COL_OUTPUT_COLOR,
				"foreground-set", COL_OUTPUT_COLOR_SET,
				"weight", COL_OUTPUT_WEIGHT,
				NULL);
		gtk_tree_view_append_column (output_view, column);

		// with a scrollbar
		GtkWidget *scrollbar = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollbar),
				GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_container_add (GTK_CONTAINER (scrollbar), GTK_WIDGET (output_view));
		gtk_paned_add2 (log_hpaned, scrollbar);
	}
}

static void
cb_action_history_changed (GtkTreeSelection *selection, gpointer user_data)
{
	GtkTreeIter iter;
	GtkTreeModel *history_model;
	if (gtk_tree_selection_get_selected (selection, &history_model, &iter))
	{
		GtkTreeModel *output_model;
		gtk_tree_model_get (history_model, &iter,
				COL_ACTION_OUTPUT_STORE, &output_model,
				-1);
		gtk_tree_view_set_model (output_view, output_model);
		output_view_columns_autosize ();
	}
}

static GtkListStore *
get_new_output_list_store (void)
{
	GtkListStore *output_list_store = gtk_list_store_new (N_COLS_OUTPUT,
			G_TYPE_STRING,		// basename
			G_TYPE_STRING,		// filename
			G_TYPE_STRING,		// line number
			G_TYPE_STRING,		// message
			G_TYPE_INT,			// message type
			G_TYPE_STRING,		// color
			G_TYPE_BOOLEAN,		// color set
			G_TYPE_INT			// weight
			);
	return output_list_store;
}

void
add_action (const gchar *title, const gchar *command)
{
	static gint num = 1;

	GtkListStore *output_list_store = get_new_output_list_store ();
	gtk_tree_view_set_model (output_view, GTK_TREE_MODEL (output_list_store));
	g_object_unref (output_list_store);

	// print title and command to the new list store
	gchar *title_with_num = g_strdup_printf ("%d. %s", num, title);
	print_output_title (title_with_num);

    gchar *command_with_dolar = g_strdup_printf ("$ %s", command);
	print_output_info (command_with_dolar);
	g_free (command_with_dolar);

	// append a new entry to the history action list
	GtkTreeModel *history_tree_model = gtk_tree_view_get_model (history_view);
	GtkTreeIter iter;
	gtk_list_store_append (GTK_LIST_STORE (history_tree_model), &iter);
	gtk_list_store_set (GTK_LIST_STORE (history_tree_model), &iter,
			COL_ACTION_TITLE, title_with_num,
			COL_ACTION_OUTPUT_STORE, output_list_store,
			-1);

	// select the new entry
	GtkTreeSelection *selection = gtk_tree_view_get_selection (history_view);
	gtk_tree_selection_select_iter (selection, &iter);

	// scroll to the end
	GtkTreePath *path = gtk_tree_model_get_path (history_tree_model, &iter);
	gtk_tree_view_scroll_to_cell (history_view, path, NULL, FALSE, 0, 0);

	// delete the first entry
	if (num > 5)
	{
		GtkTreeIter first;
		gtk_tree_model_get_iter_first (history_tree_model, &first);
		gtk_list_store_remove (GTK_LIST_STORE (history_tree_model), &first);
	}

	num++;
	g_free (title_with_num);
}

void
set_history_sensitivity (gboolean sensitive)
{
	gtk_widget_set_sensitive (GTK_WIDGET (history_view), sensitive);
}

void
output_view_columns_autosize (void)
{
	gtk_tree_view_columns_autosize (output_view);
}

void
print_output_title (const gchar *title)
{
	GtkListStore *list_store =
		GTK_LIST_STORE (gtk_tree_view_get_model (output_view));
	GtkTreeIter iter;
	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter,
			COL_OUTPUT_BASENAME, INFO_MESSAGE,
			COL_OUTPUT_LINE_NUMBER, "",
			COL_OUTPUT_MESSAGE, title,
			COL_OUTPUT_MESSAGE_TYPE, MESSAGE_TYPE_OTHER,
			COL_OUTPUT_COLOR_SET, FALSE,
			COL_OUTPUT_WEIGHT, WEIGHT_BOLD,
			-1);
	scroll_to_end (&iter);
}

void
print_output_info (const gchar *info)
{
	GtkListStore *list_store =
		GTK_LIST_STORE (gtk_tree_view_get_model (output_view));
	GtkTreeIter iter;
	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter,
			COL_OUTPUT_BASENAME, INFO_MESSAGE,
			COL_OUTPUT_LINE_NUMBER, "",
			COL_OUTPUT_MESSAGE, info,
			COL_OUTPUT_MESSAGE_TYPE, MESSAGE_TYPE_OTHER,
			COL_OUTPUT_COLOR_SET, FALSE,
			COL_OUTPUT_WEIGHT, WEIGHT_NORMAL,
			-1);
	scroll_to_end (&iter);
}

// if message != NULL the exit_code is not taken into account
void
print_output_exit (const gint exit_code, const gchar *message)
{
	GtkListStore *list_store =
		GTK_LIST_STORE (gtk_tree_view_get_model (output_view));
	GtkTreeIter iter;
	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter,
			COL_OUTPUT_BASENAME, INFO_MESSAGE,
			COL_OUTPUT_LINE_NUMBER, "",
			COL_OUTPUT_MESSAGE_TYPE, MESSAGE_TYPE_OTHER,
			COL_OUTPUT_COLOR_SET, TRUE,
			COL_OUTPUT_WEIGHT, WEIGHT_NORMAL,
			-1);

	if (message != NULL)
	{
		gtk_list_store_set (list_store, &iter,
				COL_OUTPUT_MESSAGE, message,
				COL_OUTPUT_COLOR, COLOR_RED,
				-1);
	}
	else if (exit_code == 0)
	{
		gtk_list_store_set (list_store, &iter,
				COL_OUTPUT_MESSAGE, _("Done!"),
				COL_OUTPUT_COLOR, COLOR_GREEN,
				-1);
	}
	else
	{
		gchar *tmp = g_strdup_printf (_("Finished with exit code %d"), exit_code);
		gtk_list_store_set (list_store, &iter,
				COL_OUTPUT_MESSAGE, tmp,
				COL_OUTPUT_COLOR, COLOR_RED,
				-1);
		g_free (tmp);
	}
	scroll_to_end (&iter);
}

void
print_output_message (const gchar *filename, const gint line_number,
		const gchar *message, gint message_type)
{
	GtkListStore *list_store =
		GTK_LIST_STORE (gtk_tree_view_get_model (output_view));

	gchar *basename;
	if (filename != NULL)
		basename = g_path_get_basename (filename);
	else
		basename = g_strdup ("");

	gchar *line_number_str;
	if (line_number != NO_LINE)
		line_number_str = g_strdup_printf ("%d", line_number);
	else
		line_number_str = g_strdup ("");

	gchar *color;
	switch (message_type)
	{
		case MESSAGE_TYPE_ERROR:
			color = COLOR_RED;
			break;
		case MESSAGE_TYPE_WARNING:
			color = COLOR_ORANGE;
			break;
		case MESSAGE_TYPE_BADBOX:
			color = COLOR_BROWN;
			break;
		default:
			color = "black";
			break;
	}

	GtkTreeIter iter;
	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter,
			COL_OUTPUT_BASENAME, basename, 
			COL_OUTPUT_FILENAME, filename != NULL ? filename : "",
			COL_OUTPUT_LINE_NUMBER, line_number_str,
			COL_OUTPUT_MESSAGE, message,
			COL_OUTPUT_MESSAGE_TYPE, message_type,
			COL_OUTPUT_COLOR, color,
			COL_OUTPUT_COLOR_SET, TRUE,
			COL_OUTPUT_WEIGHT, WEIGHT_NORMAL,
			-1);
	scroll_to_end (&iter);

	g_free (basename);
	g_free (line_number_str);
}

void
print_output_normal (const gchar *message)
{
	GtkListStore *list_store =
		GTK_LIST_STORE (gtk_tree_view_get_model (output_view));
	GtkTreeIter iter;
	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter,
			COL_OUTPUT_BASENAME, "", 
			COL_OUTPUT_LINE_NUMBER, "",
			COL_OUTPUT_MESSAGE, message,
			COL_OUTPUT_MESSAGE_TYPE, MESSAGE_TYPE_OTHER,
			COL_OUTPUT_COLOR_SET, FALSE,
			COL_OUTPUT_WEIGHT, WEIGHT_NORMAL,
			-1);
	scroll_to_end (&iter);
}

static void
scroll_to_end (GtkTreeIter *iter)
{
	GtkTreeModel *tree_model = gtk_tree_view_get_model (output_view);
	GtkTreePath *path = gtk_tree_model_get_path (tree_model, iter);
	gtk_tree_view_scroll_to_cell (output_view, path, NULL, FALSE, 0, 0);
}
