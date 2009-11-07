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
#include <locale.h>
#include <libintl.h>
#include <string.h> // for strcmp
#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>

#include "main.h"
#include "config.h"
#include "print.h"
#include "file_browser.h"
#include "callbacks.h"

static void fill_list_store_with_current_dir (void);
static void cb_file_browser_row_activated (GtkTreeView *tree_view,
		GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
gint sort_list_alphabetical_order (gconstpointer a, gconstpointer b);

void
init_file_browser (void)
{
	GtkListStore *store = gtk_list_store_new (N_COLUMNS_FILE_BROWSER,
			GDK_TYPE_PIXBUF, G_TYPE_STRING);
	latexila.file_browser.list_store = store;
	latexila.file_browser.current_dir = g_strdup (g_get_home_dir ());

	fill_list_store_with_current_dir ();

	GtkWidget *tree_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
	g_signal_connect (G_OBJECT (tree_view), "row-activated",
			G_CALLBACK (cb_file_browser_row_activated), NULL);
	
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	renderer = gtk_cell_renderer_pixbuf_new ();
	column = gtk_tree_view_column_new_with_attributes (NULL, renderer, "pixbuf",
			COLUMN_FILE_BROWSER_PIXBUF, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (NULL, renderer, "text",
			COLUMN_FILE_BROWSER_FILE, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), column);

	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (tree_view), FALSE);

	// with a scrollbar
	GtkWidget *scrollbar = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollbar),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (scrollbar), tree_view);
	gtk_box_pack_start (GTK_BOX (latexila.file_browser.vbox), scrollbar,
			TRUE, TRUE, 0);
}

static void
fill_list_store_with_current_dir (void)
{
	GError *error = NULL;
	GDir *dir = g_dir_open (latexila.file_browser.current_dir, 0, &error);
	if (error != NULL)
	{
		print_warning ("File browser: %s", error->message);
		g_error_free (error);
		return;
	}

	gtk_list_store_clear (latexila.file_browser.list_store);

	GtkTreeIter iter;

	// get the pixbufs
	GdkPixbuf *pixbuf_dir = gtk_widget_render_icon (latexila.side_pane,
			GTK_STOCK_DIRECTORY, GTK_ICON_SIZE_MENU, NULL);
	GdkPixbuf *pixbuf_file = gtk_widget_render_icon (latexila.side_pane,
			GTK_STOCK_FILE, GTK_ICON_SIZE_MENU, NULL);

	// append first the parent directory
	gtk_list_store_append (latexila.file_browser.list_store, &iter);
	gtk_list_store_set (latexila.file_browser.list_store, &iter,
			COLUMN_FILE_BROWSER_PIXBUF, pixbuf_dir,
			COLUMN_FILE_BROWSER_FILE, "..",
			-1);

	/* append all the files contained in the directory */
	const gchar *read_name = NULL;
	GList *directory_list = NULL;
	GList *file_list = NULL;
	while ((read_name = g_dir_read_name (dir)) != NULL)
	{
		// not show hidden files
		if (read_name[0] == '.')
			continue;

		gchar *full_path = g_build_filename (latexila.file_browser.current_dir,
				read_name, NULL);

		if (g_file_test (full_path, G_FILE_TEST_IS_DIR))
			directory_list = g_list_prepend (directory_list, (gpointer) read_name);
		else
			file_list = g_list_prepend (file_list, (gpointer) read_name);

		g_free (full_path);
	}

	g_dir_close (dir);

	directory_list = g_list_sort (directory_list, sort_list_alphabetical_order);
	file_list = g_list_sort (file_list, sort_list_alphabetical_order);

	// traverse the directory list
	GList *current = directory_list;
	while (TRUE)
	{
		if (current == NULL)
			break;

		gchar *directory = g_list_nth_data (current, 0);

		// append the directory to the list store
		gtk_list_store_append (latexila.file_browser.list_store, &iter);
		gtk_list_store_set (latexila.file_browser.list_store, &iter,
				COLUMN_FILE_BROWSER_PIXBUF, pixbuf_dir,
				COLUMN_FILE_BROWSER_FILE, directory,
				-1);

		current = g_list_next (current);
	}

	// traverse the file list
	current = file_list;
	while (TRUE)
	{
		if (current == NULL)
			break;

		gchar *file = g_list_nth_data (current, 0);

		// append the file to the list store
		gtk_list_store_append (latexila.file_browser.list_store, &iter);
		gtk_list_store_set (latexila.file_browser.list_store, &iter,
				COLUMN_FILE_BROWSER_PIXBUF, pixbuf_file,
				COLUMN_FILE_BROWSER_FILE, file,
				-1);

		current = g_list_next (current);
	}

	g_object_unref (pixbuf_dir);
	g_object_unref (pixbuf_file);
	g_list_free (directory_list);
	g_list_free (file_list);
}

static void
cb_file_browser_row_activated (GtkTreeView *tree_view, GtkTreePath *path,
		GtkTreeViewColumn *column, gpointer user_data)
{
	GtkTreeIter iter;
	GtkTreeModel *model = GTK_TREE_MODEL (latexila.file_browser.list_store);
	gtk_tree_model_get_iter (model, &iter, path);
	
	gchar *file = NULL;
	gtk_tree_model_get (model, &iter, COLUMN_FILE_BROWSER_FILE, &file, -1);

	gchar *full_path = g_build_filename (latexila.file_browser.current_dir,
			file, NULL);

	// open the directory
	if (g_file_test (full_path, G_FILE_TEST_IS_DIR))
	{
		g_free (latexila.file_browser.current_dir);
		latexila.file_browser.current_dir = full_path;
		fill_list_store_with_current_dir ();
	}

	// open the file
	else
	{
		GError *error = NULL;
		gchar *uri = g_filename_to_uri (full_path, NULL, &error);
		if (error != NULL)
		{
			print_warning ("can not open the file \"%s\": %s", full_path,
					error->message);
			g_error_free (error);
			error = NULL;
		}
		else
			open_new_document (full_path, uri);

		g_free (full_path);
		g_free (uri);
	}
}

gint
sort_list_alphabetical_order (gconstpointer a, gconstpointer b)
{
	return strcmp ((char *) a, (char *) b);
}
