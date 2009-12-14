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
#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>

#include "main.h"
#include "config.h"
#include "print.h"
#include "callbacks.h"
#include "templates.h"

static void add_template_from_string (GtkListStore *store, gchar *name,
		gchar *icon, gchar *contents);
static void add_template_from_file (GtkListStore *store, gchar *name,
		gchar *icon, gchar *filename);

static GtkListStore *store;

void
cb_new (void)
{
	GtkWidget *dialog = gtk_dialog_new_with_buttons (_("New File..."),
			latexila.main_window,
			GTK_DIALOG_NO_SEPARATOR,
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
			NULL);

	gtk_window_set_default_size (GTK_WINDOW (dialog), 400, 200);
	
	GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

	GtkWidget *icon_view = gtk_icon_view_new_with_model (
			GTK_TREE_MODEL (store));
	gtk_icon_view_set_selection_mode (GTK_ICON_VIEW (icon_view),
			GTK_SELECTION_SINGLE);
	gtk_icon_view_set_columns (GTK_ICON_VIEW (icon_view), -1);
	gtk_icon_view_set_text_column (GTK_ICON_VIEW (icon_view),
			COLUMN_TEMPLATE_NAME);
	gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (icon_view),
			COLUMN_TEMPLATE_PIXBUF);

	// with a scrollbar (without that there is a problem for resizing the
	// dialog, we can make it bigger but not smaller...)
	GtkWidget *scrollbar = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollbar),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (scrollbar), icon_view);

	gtk_box_pack_start (GTK_BOX (content_area), scrollbar, TRUE, TRUE, 0);
	gtk_widget_show_all (content_area);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		GList *selected_items = gtk_icon_view_get_selected_items (
				GTK_ICON_VIEW (icon_view));
		GtkTreePath *path = g_list_nth_data (selected_items, 0);
		GtkTreeModel *model = GTK_TREE_MODEL (store);
		GtkTreeIter iter;

		gchar *contents = NULL;
		if (path != NULL && gtk_tree_model_get_iter (model, &iter, path))
		{
			gtk_tree_model_get (model, &iter,
					COLUMN_TEMPLATE_CONTENTS, &contents,
					-1);
		}
		else
			contents = g_strdup ("");

		create_document_in_new_tab (NULL, contents, _("New document"));

		g_free (contents);

		// free the GList
		g_list_foreach (selected_items, (GFunc) gtk_tree_path_free, NULL);
		g_list_free (selected_items);
	}

	gtk_widget_destroy (dialog);
}

void
init_templates (void)
{
	store = gtk_list_store_new (N_COLUMNS_TEMPLATE,
			GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING);

	add_template_from_string (store, _("Empty"),
			DATA_DIR "/images/templates/empty.png", "");
	add_template_from_file (store, _("Article"),
			DATA_DIR "/images/templates/article.png", _("article-en.tex"));
	add_template_from_file (store, _("Report"),
			DATA_DIR "/images/templates/report.png", _("report-en.tex"));
	add_template_from_file (store, _("Book"),
			DATA_DIR "/images/templates/book.png", _("book-en.tex"));
	add_template_from_file (store, _("Letter"),
			DATA_DIR "/images/templates/letter.png", _("letter-en.tex"));

}

static void
add_template_from_string (GtkListStore *store, gchar *name, gchar *icon,
		gchar *contents)
{
	if (contents == NULL)
		return;

	GError *error = NULL;
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file (icon, &error);
	if (error != NULL)
	{
		print_warning ("impossible to load the icon of the template: %s",
				error->message);
		g_error_free (error);
		return;
	}

	GtkTreeIter iter;
	gtk_list_store_append (store, &iter);
	gtk_list_store_set (store, &iter,
			COLUMN_TEMPLATE_PIXBUF, pixbuf,
			COLUMN_TEMPLATE_NAME, name,
			COLUMN_TEMPLATE_CONTENTS, contents,
			-1);

	g_object_unref (pixbuf);
}

static void
add_template_from_file (GtkListStore *store, gchar *name, gchar *icon,
		gchar *filename)
{
	gchar *contents = NULL;
	GError *error = NULL;
	gchar *path = g_strdup_printf (DATA_DIR "/templates/%s", filename);
	g_file_get_contents (path, &contents, NULL, &error);
	g_free (path);

	if (error != NULL)
	{
		print_warning ("impossible to load the template \"%s\": %s", name,
				error->message);
		g_error_free (error);
		return;
	}

	gchar *text_utf8 = g_locale_to_utf8 (contents, -1, NULL, NULL, NULL);
	add_template_from_string (store, name, icon, text_utf8);

	g_free (text_utf8);
	g_free (contents);
}
