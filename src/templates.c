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
#include <gtk/gtk.h>
#include <sys/stat.h> // for S_IRWXU
#include <glib/gstdio.h> // for g_remove()

#include "main.h"
#include "config.h"
#include "print.h"
#include "callbacks.h"
#include "templates.h"

static void add_template_from_string (GtkListStore *store, const gchar *name,
		const gchar *icon, const gchar *contents);
static void add_template_from_file (GtkListStore *store, const gchar *name,
		const gchar *icon, const gchar *path);
static GtkWidget * create_icon_view (GtkListStore *store);
static void cb_icon_view_selection_changed (GtkIconView *icon_view,
		gpointer other_icon_view);
static gchar * get_rc_file (void);
static gchar * get_rc_dir (void);
static void add_personnal_template (const gchar *name, const gchar *contents);
static void save_rc_file (void);
static void save_contents (void);

static GtkListStore *default_store;
static GtkListStore *personnal_store;
static gint nb_personnal_templates;

void
cb_new (void)
{
	GtkWidget *dialog = gtk_dialog_new_with_buttons (_("New File..."),
			latexila.main_window,
			GTK_DIALOG_NO_SEPARATOR,
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
			NULL);

	gtk_window_set_default_size (GTK_WINDOW (dialog), 420, 350);
	
	GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

	/* icon view for the default templates */
	GtkWidget *icon_view_default_templates = create_icon_view (default_store);

	// with a scrollbar (without that there is a problem for resizing the
	// dialog, we can make it bigger but not smaller...)
	GtkWidget *scrollbar = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollbar),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (scrollbar), icon_view_default_templates);

	// with a frame
	GtkWidget *frame = gtk_frame_new (_("Default templates"));
	gtk_container_add (GTK_CONTAINER (frame), scrollbar);

	gtk_box_pack_start (GTK_BOX (content_area), frame, TRUE, TRUE, 10);

	/* icon view for the personnal templates */
	GtkWidget *icon_view_personnal_templates = create_icon_view (personnal_store);

	// with a scrollbar
	scrollbar = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollbar),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (scrollbar), icon_view_personnal_templates);

	// with a frame
	frame = gtk_frame_new (_("Your personnal templates"));
	gtk_container_add (GTK_CONTAINER (frame), scrollbar);

	gtk_box_pack_start (GTK_BOX (content_area), frame, TRUE, TRUE, 10);

	gtk_widget_show_all (content_area);

	/* signals */
	g_signal_connect (G_OBJECT (icon_view_default_templates),
			"selection-changed",
			G_CALLBACK (cb_icon_view_selection_changed),
			GTK_ICON_VIEW (icon_view_personnal_templates));

	g_signal_connect (G_OBJECT (icon_view_personnal_templates),
			"selection-changed",
			G_CALLBACK (cb_icon_view_selection_changed),
			GTK_ICON_VIEW (icon_view_default_templates));

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		GList *selected_items = gtk_icon_view_get_selected_items (
				GTK_ICON_VIEW (icon_view_default_templates));
		GtkTreeModel *model = GTK_TREE_MODEL (default_store);

		// if no item is selected in the default templates, maybe one item is
		// selected in the personnal templates
		if (g_list_length (selected_items) == 0)
		{
			// free the GList
			g_list_foreach (selected_items, (GFunc) gtk_tree_path_free, NULL);
			g_list_free (selected_items);

			selected_items = gtk_icon_view_get_selected_items (
					GTK_ICON_VIEW (icon_view_personnal_templates));
			model = GTK_TREE_MODEL (personnal_store);
		}

		GtkTreePath *path = selected_items->data;
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
cb_create_template (void)
{
	if (latexila.active_doc == NULL)
		return;

	GtkWidget *dialog = gtk_dialog_new_with_buttons (_("New Template..."),
			latexila.main_window, 0,
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
			NULL);

	GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

	GtkWidget *hbox = gtk_hbox_new (FALSE, 5);
	GtkWidget *label = gtk_label_new (_("Name of the new template:"));
	GtkWidget *entry = gtk_entry_new ();

	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), entry, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (content_area), hbox, FALSE, FALSE, 10);

	gtk_widget_show_all (content_area);

	while (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		if (gtk_entry_get_text_length (GTK_ENTRY (entry)) == 0)
			continue;

		nb_personnal_templates++;

		const gchar *name = gtk_entry_get_text (GTK_ENTRY (entry));

		GtkTextBuffer *buffer =
			GTK_TEXT_BUFFER (latexila.active_doc->source_buffer);
		GtkTextIter start, end;
		gtk_text_buffer_get_bounds (buffer, &start, &end);
		gchar *contents = gtk_text_buffer_get_text (buffer, &start, &end,
				FALSE);

		add_template_from_string (personnal_store, name,
				DATA_DIR "/images/templates/article.png", contents);
		add_personnal_template (name, contents);

		g_free (contents);
		break;
	}

	gtk_widget_destroy (dialog);
}

void
cb_delete_template (void)
{
	GtkWidget *dialog = gtk_dialog_new_with_buttons (_("Delete Template(s)..."),
			latexila.main_window,
			GTK_DIALOG_NO_SEPARATOR,
			GTK_STOCK_DELETE, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_OK, GTK_RESPONSE_REJECT,
			NULL);

	gtk_window_set_default_size (GTK_WINDOW (dialog), 400, 200);
	
	GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

	/* icon view for the personnal templates */
	GtkWidget *icon_view = create_icon_view (personnal_store);
	gtk_icon_view_set_selection_mode (GTK_ICON_VIEW (icon_view),
			GTK_SELECTION_MULTIPLE);

	// with a scrollbar (without that there is a problem for resizing the
	// dialog, we can make it bigger but not smaller...)
	GtkWidget *scrollbar = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollbar),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (scrollbar), icon_view);

	// with a frame
	GtkWidget *frame = gtk_frame_new (_("Personnal templates"));
	gtk_container_add (GTK_CONTAINER (frame), scrollbar);

	gtk_box_pack_start (GTK_BOX (content_area), frame, TRUE, TRUE, 10);

	gtk_widget_show_all (content_area);

	gint nb_personnal_templates_before = nb_personnal_templates;

	while (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		GList *selected_items = gtk_icon_view_get_selected_items (
				GTK_ICON_VIEW (icon_view));
		GtkTreeModel *model = GTK_TREE_MODEL (personnal_store);
		
		guint nb_selected_items = g_list_length (selected_items);

		for (gint i = 0 ; i < nb_selected_items ; i++)
		{
			GtkTreePath *path = g_list_nth_data (selected_items, i);
			GtkTreeIter iter;
			gtk_tree_model_get_iter (model, &iter, path);
			gtk_list_store_remove (personnal_store, &iter);
		}

		nb_personnal_templates -= nb_selected_items;

		// free the GList
		g_list_foreach (selected_items, (GFunc) gtk_tree_path_free, NULL);
		g_list_free (selected_items);
	}

	if (nb_personnal_templates != nb_personnal_templates_before)
	{
		save_rc_file ();
		save_contents ();
	}

	gtk_widget_destroy (dialog);
}

void
init_templates (void)
{
	/* default templates */
	default_store = gtk_list_store_new (N_COLUMNS_TEMPLATE,
			GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING);

	add_template_from_string (default_store, _("Empty"),
			DATA_DIR "/images/templates/empty.png", "");

	// article
	gchar *path = g_strdup_printf (DATA_DIR "/templates/%s",
			_("article-en.tex"));
	add_template_from_file (default_store, _("Article"),
			DATA_DIR "/images/templates/article.png", path);
	g_free (path);

	// report
	path = g_strdup_printf (DATA_DIR "/templates/%s",
			_("report-en.tex"));
	add_template_from_file (default_store, _("Report"),
			DATA_DIR "/images/templates/report.png", path);
	g_free (path);

	// book
	path = g_strdup_printf (DATA_DIR "/templates/%s",
			_("book-en.tex"));
	add_template_from_file (default_store, _("Book"),
			DATA_DIR "/images/templates/book.png", path);
	g_free (path);

	// letter
	path = g_strdup_printf (DATA_DIR "/templates/%s",
			_("letter-en.tex"));
	add_template_from_file (default_store, _("Letter"),
			DATA_DIR "/images/templates/letter.png", path);
	g_free (path);


	/* personnal templates */
	personnal_store = gtk_list_store_new (N_COLUMNS_TEMPLATE,
			GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING);
	nb_personnal_templates = 0;

	gchar *rc_file = get_rc_file ();
	if (! g_file_test (rc_file, G_FILE_TEST_EXISTS))
	{
		g_free (rc_file);
		return;
	}

	GKeyFile *key_file = g_key_file_new ();
	GError *error = NULL;
	g_key_file_load_from_file (key_file, rc_file, G_KEY_FILE_NONE, &error);
	g_free (rc_file);

	if (error != NULL)
	{
		print_warning ("load templates failed: %s", error->message);
		g_error_free (error);
		return;
	}

	gsize length;
	gchar **names = g_key_file_get_string_list (key_file, PROGRAM_NAME,
			"names", &length, &error);
	if (error != NULL)
	{
		print_warning ("load templates failed: %s", error->message);
		g_error_free (error);
		g_key_file_free (key_file);
		return;
	}

	nb_personnal_templates = length;

	gchar *rc_dir = get_rc_dir ();

	for (gint i = 0 ; i < length ; i++)
	{
		gchar *file = g_strdup_printf ("%s/%d.tex", rc_dir, i);
		if (! g_file_test (file, G_FILE_TEST_EXISTS))
		{
			g_free (file);
			continue;
		}

		add_template_from_file (personnal_store, names[i],
				DATA_DIR "/images/templates/article.png", file);
		g_free (file);
	}

	g_strfreev (names);
	g_key_file_free (key_file);
	g_free (rc_dir);
}

static void
add_template_from_string (GtkListStore *store, const gchar *name,
		const gchar *icon, const gchar *contents)
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
add_template_from_file (GtkListStore *store, const gchar *name,
		const gchar *icon, const gchar *path)
{
	gchar *contents = NULL;
	GError *error = NULL;
	g_file_get_contents (path, &contents, NULL, &error);

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

static GtkWidget *
create_icon_view (GtkListStore *store)
{
	GtkWidget *icon_view = gtk_icon_view_new_with_model (
			GTK_TREE_MODEL (store));
	gtk_icon_view_set_selection_mode (GTK_ICON_VIEW (icon_view),
			GTK_SELECTION_SINGLE);
	gtk_icon_view_set_text_column (GTK_ICON_VIEW (icon_view),
			COLUMN_TEMPLATE_NAME);
	gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (icon_view),
			COLUMN_TEMPLATE_PIXBUF);

	return icon_view;
}

static void
cb_icon_view_selection_changed (GtkIconView *icon_view,
		gpointer other_icon_view)
{
	// only one item of the two icon views can be selected at once
	
	// we unselect all the items of the other icon view only if the current icon
	// view have an item selected, because when we unselect all the items the
	// "selection-changed" signal is emitted for the other icon view, so for the
	// other icon view this function is also called but no item is selected so
	// nothing is done and the item selected by the user keeps selected

	GList *selected_items = gtk_icon_view_get_selected_items (icon_view);

	if (g_list_length (selected_items) > 0)
		gtk_icon_view_unselect_all ((GtkIconView *) other_icon_view);

	// free the GList
	g_list_foreach (selected_items, (GFunc) gtk_tree_path_free, NULL);
	g_list_free (selected_items);
}

static gchar *
get_rc_file (void)
{
	// rc_file must be freed
	gchar *rc_file = g_build_filename (g_get_user_data_dir (), "latexila",
			"templatesrc", NULL);
	return rc_file;
}

static gchar *
get_rc_dir (void)
{
	// rc_dir must be freed
	gchar *rc_dir = g_build_filename (g_get_user_data_dir (), "latexila", NULL);
	return rc_dir;
}

static void
add_personnal_template (const gchar *name, const gchar *contents)
{
	save_rc_file ();

	gchar *rc_dir = get_rc_dir ();

	gchar *file = g_strdup_printf ("%s/%d.tex", rc_dir,
			nb_personnal_templates - 1);

	GError *error = NULL;
	g_file_set_contents (file, contents, -1, &error);

	if (error != NULL)
	{
		print_warning ("impossible to save templates: %s", error->message);
		g_error_free (error);
	}

	g_free (rc_dir);
	g_free (file);
}

static void
save_rc_file (void)
{
	if (nb_personnal_templates == 0)
	{
		gchar *rc_file = get_rc_file ();
		g_remove (rc_file);
		g_free (rc_file);
		return;
	}

	gchar **names = g_malloc ((nb_personnal_templates + 1) * sizeof (gchar *));
	gchar **names_i = names;
	
	// traverse the list store
	GtkTreeIter iter;
	GtkTreeModel *model = GTK_TREE_MODEL (personnal_store);
	gboolean valid_iter = gtk_tree_model_get_iter_first (model, &iter);
	while (valid_iter)
	{
		gtk_tree_model_get (model, &iter, COLUMN_TEMPLATE_NAME, names_i, -1);
		valid_iter = gtk_tree_model_iter_next (model, &iter);
		names_i++;
	}

	// the last element is NULL so we can use g_strfreev()
	*names_i = NULL;

	GKeyFile *key_file = g_key_file_new ();
	g_key_file_set_string_list (key_file, PROGRAM_NAME, "names",
			(const gchar * const *) names,
			nb_personnal_templates);

	/* save the rc file */
	gchar *rc_file = get_rc_file ();
	gchar *rc_dir = get_rc_dir ();
	g_mkdir_with_parents(rc_dir, S_IRWXU);
	gchar *key_file_data = g_key_file_to_data (key_file, NULL, NULL);

	GError *error = NULL;
	g_file_set_contents (rc_file, key_file_data, -1, &error);

	if (error != NULL)
	{
		print_warning ("impossible to save templates: %s", error->message);
		g_error_free (error);
	}

	g_strfreev (names);
	g_free (rc_file);
	g_free (rc_dir);
	g_free (key_file_data);
	g_key_file_free (key_file);
}

static void
save_contents (void)
{
	gchar *rc_dir = get_rc_dir ();

	// delete all the *.tex files
	gchar *command = g_strdup_printf ("rm -f %s/*.tex", rc_dir);
	system (command);
	g_free (command);

	// traverse the list store
	GtkTreeIter iter;
	GtkTreeModel *model = GTK_TREE_MODEL (personnal_store);
	gboolean valid_iter = gtk_tree_model_get_iter_first (model, &iter);
	gint i = 0;
	while (valid_iter)
	{
		gchar *contents;
		gtk_tree_model_get (model, &iter, COLUMN_TEMPLATE_CONTENTS, &contents, -1);

		gchar *file = g_strdup_printf ("%s/%d.tex", rc_dir, i);

		GError *error = NULL;
		g_file_set_contents (file, contents, -1, &error);

		if (error != NULL)
		{
			print_warning ("impossible to save the template: %s", error->message);
			g_error_free (error);
			error = NULL;
		}

		g_free (contents);
		g_free (file);

		valid_iter = gtk_tree_model_iter_next (model, &iter);
		i++;
	}

	g_free (rc_dir);
}
