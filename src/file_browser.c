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
#include "external_commands.h"

static void fill_list_store_with_current_dir (void);
static void cb_go_to_home_dir (GtkButton *button, gpointer user_data);
static void cb_go_to_parent_dir (GtkButton *button, gpointer user_data);
static void cb_refresh (GtkButton *button, gpointer user_data);
static void cb_file_browser_row_activated (GtkTreeView *tree_view,
		GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
static gint sort_list_alphabetical_order (gconstpointer a, gconstpointer b);

void
init_file_browser (void)
{
	/* mini-toolbar */
	// go to the home user directory
	GtkWidget *home_button = gtk_button_new ();
	gtk_button_set_relief (GTK_BUTTON (home_button), GTK_RELIEF_NONE);
	GtkWidget *home_icon = gtk_image_new_from_stock (GTK_STOCK_HOME,
			GTK_ICON_SIZE_BUTTON);
	gtk_container_add (GTK_CONTAINER (home_button), home_icon);
	gtk_widget_set_tooltip_text (home_button, _("Go to the home directory"));
	g_signal_connect (G_OBJECT (home_button), "clicked",
			G_CALLBACK (cb_go_to_home_dir), NULL);

	// go to the parent directory
	GtkWidget *parent_dir_button = gtk_button_new ();
	gtk_button_set_relief (GTK_BUTTON (parent_dir_button), GTK_RELIEF_NONE);
	GtkWidget *parent_dir_icon = gtk_image_new_from_stock (GTK_STOCK_GO_UP,
			GTK_ICON_SIZE_BUTTON);
	gtk_container_add (GTK_CONTAINER (parent_dir_button), parent_dir_icon);
	gtk_widget_set_tooltip_text (parent_dir_button, _("Go to the parent directory"));
	g_signal_connect (G_OBJECT (parent_dir_button), "clicked",
			G_CALLBACK (cb_go_to_parent_dir), NULL);

	// refresh
	GtkWidget *refresh_button = gtk_button_new ();
	gtk_button_set_relief (GTK_BUTTON (refresh_button), GTK_RELIEF_NONE);
	GtkWidget *refresh_icon = gtk_image_new_from_stock (GTK_STOCK_REFRESH,
			GTK_ICON_SIZE_BUTTON);
	gtk_container_add (GTK_CONTAINER (refresh_button), refresh_icon);
	gtk_widget_set_tooltip_text (refresh_button, _("Refresh"));
	g_signal_connect (G_OBJECT (refresh_button), "clicked",
			G_CALLBACK (cb_refresh), NULL);

	GtkWidget *hbox = gtk_hbox_new (TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), home_button, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), parent_dir_button, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), refresh_button, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (latexila.file_browser.vbox), hbox,
			FALSE, FALSE, 0);

	/* list of files and directories */
	GtkListStore *store = gtk_list_store_new (N_COLUMNS_FILE_BROWSER,
			GDK_TYPE_PIXBUF, G_TYPE_STRING);
	latexila.file_browser.list_store = store;

	fill_list_store_with_current_dir ();

	GtkWidget *tree_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
	// dubble-click on a row will open the file
	g_signal_connect (G_OBJECT (tree_view), "row-activated",
			G_CALLBACK (cb_file_browser_row_activated), NULL);
	
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	// show the icon
	renderer = gtk_cell_renderer_pixbuf_new ();
	column = gtk_tree_view_column_new_with_attributes (NULL, renderer, "pixbuf",
			COLUMN_FILE_BROWSER_PIXBUF, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), column);

	// show the file name
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
	GDir *dir = g_dir_open (latexila.prefs.file_browser_dir, 0, &error);
	if (error != NULL)
	{
		print_warning ("File browser: %s", error->message);
		g_error_free (error);
		return;
	}

	gtk_list_store_clear (latexila.file_browser.list_store);

	/* append all the files contained in the directory */
	const gchar *read_name = NULL;
	GList *directory_list = NULL;
	GList *file_list = NULL;
	while ((read_name = g_dir_read_name (dir)) != NULL)
	{
		// not show hidden files
		if (read_name[0] == '.')
			continue;

		gchar *full_path = g_build_filename (latexila.prefs.file_browser_dir,
				read_name, NULL);

		// make a copy of read_name, else there are memory errors if the
		// directory is very big (/usr/bin for example)
		gchar *tmp = g_strdup (read_name);

		if (g_file_test (full_path, G_FILE_TEST_IS_DIR))
			directory_list = g_list_prepend (directory_list, (gpointer) tmp);
		else
			file_list = g_list_prepend (file_list, (gpointer) tmp);

		g_free (full_path);
	}

	g_dir_close (dir);

	// sort the lists in alphabetical order
	directory_list = g_list_sort (directory_list, sort_list_alphabetical_order);
	file_list = g_list_sort (file_list, sort_list_alphabetical_order);

	// get the pixbufs
	GdkPixbuf *pixbuf_dir = gtk_widget_render_icon (latexila.side_pane,
			GTK_STOCK_DIRECTORY, GTK_ICON_SIZE_MENU, NULL);
	GdkPixbuf *pixbuf_file = gtk_widget_render_icon (latexila.side_pane,
			GTK_STOCK_FILE, GTK_ICON_SIZE_MENU, NULL);
	GdkPixbuf *pixbuf_tex = gtk_widget_render_icon (latexila.side_pane,
			GTK_STOCK_EDIT, GTK_ICON_SIZE_MENU, NULL);
	GdkPixbuf *pixbuf_pdf = gtk_widget_render_icon (latexila.side_pane,
			"view_pdf", GTK_ICON_SIZE_MENU, NULL);
	GdkPixbuf *pixbuf_dvi = gtk_widget_render_icon (latexila.side_pane,
			"view_dvi", GTK_ICON_SIZE_MENU, NULL);
	GdkPixbuf *pixbuf_ps = gtk_widget_render_icon (latexila.side_pane,
			"view_ps", GTK_ICON_SIZE_MENU, NULL);

	GtkTreeIter iter;

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

		g_free (directory);
		current = g_list_next (current);
	}

	// traverse the file list
	current = file_list;
	while (TRUE)
	{
		if (current == NULL)
			break;

		gchar *file = g_list_nth_data (current, 0);

		GdkPixbuf *pixbuf;
		if (g_str_has_suffix (file, ".tex"))
			pixbuf = pixbuf_tex;
		else if (g_str_has_suffix (file, ".pdf"))
			pixbuf = pixbuf_pdf;
		else if (g_str_has_suffix (file, ".dvi"))
			pixbuf = pixbuf_dvi;
		else if (g_str_has_suffix (file, ".ps"))
			pixbuf = pixbuf_ps;
		else
			pixbuf = pixbuf_file;


		// append the file to the list store
		gtk_list_store_append (latexila.file_browser.list_store, &iter);
		gtk_list_store_set (latexila.file_browser.list_store, &iter,
				COLUMN_FILE_BROWSER_PIXBUF, pixbuf,
				COLUMN_FILE_BROWSER_FILE, file,
				-1);

		g_free (file);
		current = g_list_next (current);
	}

	g_object_unref (pixbuf_dir);
	g_object_unref (pixbuf_file);
	g_object_unref (pixbuf_tex);
	g_object_unref (pixbuf_pdf);
	g_object_unref (pixbuf_dvi);
	g_object_unref (pixbuf_ps);
	g_list_free (directory_list);
	g_list_free (file_list);
}

static void
cb_go_to_home_dir (GtkButton *button, gpointer user_data)
{
	g_free (latexila.prefs.file_browser_dir);
	latexila.prefs.file_browser_dir = g_strdup (g_get_home_dir ());
	fill_list_store_with_current_dir ();
}

static void
cb_go_to_parent_dir (GtkButton *button, gpointer user_data)
{
	gchar *path = g_path_get_dirname (latexila.prefs.file_browser_dir);
	g_free (latexila.prefs.file_browser_dir);
	latexila.prefs.file_browser_dir = path;
	fill_list_store_with_current_dir ();
}

static void
cb_refresh (GtkButton *button, gpointer user_data)
{
	fill_list_store_with_current_dir ();
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

	gchar *full_path = g_build_filename (latexila.prefs.file_browser_dir,
			file, NULL);

	// open the directory
	if (g_file_test (full_path, G_FILE_TEST_IS_DIR))
	{
		g_free (latexila.prefs.file_browser_dir);
		latexila.prefs.file_browser_dir = g_strdup (full_path);
		fill_list_store_with_current_dir ();
	}

	// view the document
	else if (g_str_has_suffix (full_path, ".pdf"))
		view_document (_("View PDF"), full_path);
	else if (g_str_has_suffix (full_path, ".dvi"))
		view_document (_("View DVI"), full_path);
	else if (g_str_has_suffix (full_path, ".ps"))
		view_document (_("View PS"), full_path);

	// open the file
	else
		open_new_document_without_uri (full_path);

	g_free (full_path);
}

static gint
sort_list_alphabetical_order (gconstpointer a, gconstpointer b)
{
	return strcmp ((char *) a, (char *) b);
}
