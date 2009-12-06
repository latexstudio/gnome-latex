/*
 * This file is part of LaTeXila.
 *
 * Copyright © 2009 Sébastien Wilmet, Stéphane Roy
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
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagemanager.h>

#include "main.h"
#include "config.h"
#include "callbacks.h"
#include "symbols.h"
#include "file_browser.h"
#include "print.h"
#include "prefs.h"
#include "ui.h"

static gboolean option_version (const gchar *option_name, const gchar *value,
		gpointer data, GError **error);

latexila_t latexila = {NULL};

static gboolean
option_version (const gchar *option_name, const gchar *value, gpointer data,
		GError **error)
{
	print_info ("%s %s", PROGRAM_NAME, PROGRAM_VERSION);
	print_info ("\tGTK+ %d.%d.%d", GTK_MAJOR_VERSION, GTK_MINOR_VERSION,
			GTK_MICRO_VERSION);
	print_info ("\tGLib %d.%d.%d", GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION,
			GLIB_MICRO_VERSION);
	print_info ("\tGtkSourceView %s", GTKSOURCEVIEW_VERSION);

	/*
	// more information
	print_info ("\t%s GTK+ %s %d.%d.%d (%s %d.%d.%d)",
			_("using"), _("version"),
			gtk_major_version, gtk_minor_version, gtk_micro_version,
			_("compiled against version"),
			GTK_MAJOR_VERSION, GTK_MINOR_VERSION, GTK_MICRO_VERSION);

	print_info ("\t%s GLib %s %d.%d.%d (%s %d.%d.%d)",
			_("using"), _("version"),
			glib_major_version, glib_minor_version, glib_micro_version,
			_("compiled against version"),
			GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION, GLIB_MICRO_VERSION);
	*/

	exit (EXIT_SUCCESS);
	return TRUE;
}

int
main (int argc, char *argv[])
{
	GError *error = NULL;
	GtkWidget *scrollbar;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	/* command line options */
	gboolean option_new_document = FALSE;

	gchar *latexila_nls_package = NULL;
#ifdef LATEXILA_NLS_ENABLED
	latexila_nls_package = LATEXILA_NLS_PACKAGE;
#endif

	GOptionEntry options[] = {
		{ "version", 'v', G_OPTION_FLAG_IN_MAIN | G_OPTION_FLAG_NO_ARG,
			G_OPTION_ARG_CALLBACK, (gpointer) option_version,
			N_("Display version informations"), NULL },
		{ "new-document", 'n', G_OPTION_FLAG_IN_MAIN | G_OPTION_FLAG_NO_ARG,
			G_OPTION_ARG_NONE, &option_new_document,
			N_("Create new document"), NULL },
		{NULL}
	};

	GOptionContext *context = g_option_context_new ("[FILES]");
	g_option_context_add_main_entries (context, options, latexila_nls_package);
	g_option_context_add_group (context, gtk_get_option_group (TRUE));

	// TODO with valgrind there are a lot of memory leaks with this, can we do
	// something?
	if (! g_option_context_parse (context, &argc, &argv, &error))
		print_error ("option parsing failed: %s\n", error->message);

	gtk_init_with_args (&argc, &argv, NULL, options, latexila_nls_package,
			&error);
	if (error != NULL)
		print_error ("%s", error->message);

	g_option_context_free (context);

	/* localisation */
#ifdef LATEXILA_NLS_ENABLED
	setlocale (LC_ALL, "");
	bindtextdomain (LATEXILA_NLS_PACKAGE, LATEXILA_NLS_LOCALEDIR);
	bind_textdomain_codeset (LATEXILA_NLS_PACKAGE, "UTF-8");
	textdomain (LATEXILA_NLS_PACKAGE);
#endif

	/* preferences */
	load_preferences (&latexila.prefs);

	/* personal style */
	// make the close buttons in tabs smaller
	// we use gtk_widget_set_name (widget, "my-close-button") to apply this
	// style
	gtk_rc_parse_string (
		"style \"my-button-style\"\n"
		"{\n"
		"  GtkWidget::focus-padding = 0\n"
		"  GtkWidget::focus-line-width = 0\n"
		"  xthickness = 0\n"
		"  ythickness = 0\n"
		"}\n"
		"widget \"*.my-close-button\" style \"my-button-style\"");

	/* main window */
	GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	latexila.main_window = GTK_WINDOW (window);
	g_signal_connect (G_OBJECT (window), "delete_event",
			G_CALLBACK (cb_delete_event), NULL);
	gtk_window_set_title (GTK_WINDOW (window), PROGRAM_NAME);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size (GTK_WINDOW (window),
			latexila.prefs.window_width, latexila.prefs.window_height);

	if (latexila.prefs.window_maximised)
		gtk_window_maximize (GTK_WINDOW (window));


	/* menubar and toolbar */
	GtkWidget *main_vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (window), main_vbox);

	init_ui (main_vbox);
	
	/* horizontal pane
	 * left: side pane (symbol tables and the file browser)
	 * right: the source view and the log zone
	 */
	GtkWidget *main_hpaned = gtk_hpaned_new ();
	latexila.main_hpaned = GTK_PANED (main_hpaned);
	gtk_paned_set_position (GTK_PANED (main_hpaned),
			latexila.prefs.main_hpaned_pos);
	gtk_box_pack_start (GTK_BOX (main_vbox), main_hpaned, TRUE, TRUE, 0);

	/* side pane */
	GtkWidget *side_pane_notebook = gtk_notebook_new ();
	latexila.side_pane = side_pane_notebook;
	gtk_notebook_set_scrollable (GTK_NOTEBOOK (side_pane_notebook), TRUE);
	gtk_paned_add1 (GTK_PANED (main_hpaned), side_pane_notebook);

	// symbol tables
	GtkWidget *vbox_symbols = gtk_vbox_new (FALSE, 0);
	latexila.symbols.vbox = vbox_symbols;

	GtkWidget *tab_label = gtk_hbox_new (FALSE, 3);
	GtkWidget *label = gtk_label_new (_("Symbols"));
	GtkWidget *image = gtk_image_new_from_file (DATA_DIR "/images/greek/01.png");
	gtk_box_pack_start (GTK_BOX (tab_label), image, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (tab_label), label, TRUE, TRUE, 0);
	gtk_widget_show_all (tab_label);

	gtk_notebook_append_page (GTK_NOTEBOOK (side_pane_notebook), vbox_symbols,
			tab_label);
	init_symbols ();

	// file browser
	GtkWidget *vbox_file_browser = gtk_vbox_new (FALSE, 0);
	latexila.file_browser.vbox = vbox_file_browser;

	tab_label = gtk_hbox_new (FALSE, 3);
	label = gtk_label_new (_("File Browser"));
	image = gtk_image_new_from_stock (GTK_STOCK_OPEN,
			GTK_ICON_SIZE_MENU);
	gtk_box_pack_start (GTK_BOX (tab_label), image, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (tab_label), label, TRUE, TRUE, 0);
	gtk_widget_show_all (tab_label);

	gtk_notebook_append_page (GTK_NOTEBOOK (side_pane_notebook),
			vbox_file_browser, tab_label);
	init_file_browser ();

	/* vertical pane
	 * top: source view, go to line, search and replace
	 * bottom: log zone
	 */
	GtkWidget *vpaned = gtk_vpaned_new ();
	latexila.vpaned = GTK_PANED (vpaned);
	gtk_paned_set_position (GTK_PANED (vpaned), latexila.prefs.vpaned_pos);
	gtk_paned_add2 (GTK_PANED (main_hpaned), vpaned);

	GtkWidget *vbox_source_view = gtk_vbox_new (FALSE, 2);
	gtk_paned_add1 (GTK_PANED (vpaned), vbox_source_view);

	/* source view with tabs */
	GtkWidget *notebook = gtk_notebook_new ();
	latexila.notebook = GTK_NOTEBOOK (notebook);
	gtk_notebook_set_scrollable (GTK_NOTEBOOK (notebook), TRUE);
	g_signal_connect (G_OBJECT (notebook), "switch-page",
			G_CALLBACK (cb_page_change), NULL);
	g_signal_connect (G_OBJECT (notebook), "page-reordered",
			G_CALLBACK (cb_page_reordered), NULL);
	gtk_box_pack_start (GTK_BOX (vbox_source_view), notebook, TRUE, TRUE, 0);

	/* go to line */
	GtkWidget *go_to_line = gtk_hbox_new (FALSE, 3);
	latexila.go_to_line = go_to_line;
	gtk_box_pack_start (GTK_BOX (vbox_source_view), go_to_line, FALSE, FALSE, 0);

	GtkWidget *close_button = gtk_button_new ();
	gtk_button_set_relief (GTK_BUTTON (close_button), GTK_RELIEF_NONE);
	image = gtk_image_new_from_stock (GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU);
	gtk_container_add (GTK_CONTAINER (close_button), image);
	g_signal_connect (G_OBJECT (close_button), "clicked",
			G_CALLBACK (cb_close_go_to_line), NULL);
	gtk_box_pack_start (GTK_BOX (go_to_line), close_button, FALSE, FALSE, 0);
	
	label = gtk_label_new (_("Go to Line:"));
	gtk_box_pack_start (GTK_BOX (go_to_line), label, FALSE, FALSE, 2);

	GtkWidget *entry_go_to_line = gtk_entry_new ();
	latexila.go_to_line_entry = entry_go_to_line;
	gtk_entry_set_icon_from_stock (GTK_ENTRY (entry_go_to_line),
			GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_JUMP_TO);
	gtk_widget_set_tooltip_text (entry_go_to_line,
			_("Line you want to move the cursor to"));
	g_signal_connect (G_OBJECT (entry_go_to_line), "activate",
			G_CALLBACK (cb_go_to_line_entry), NULL);
	gtk_box_pack_start (GTK_BOX (go_to_line), entry_go_to_line, FALSE, FALSE, 0);

	/* find */
	GtkWidget *find = gtk_hbox_new (FALSE, 3);
	latexila.find = find;
	gtk_box_pack_start (GTK_BOX (vbox_source_view), find, FALSE, FALSE, 0);

	close_button = gtk_button_new ();
	gtk_button_set_relief (GTK_BUTTON (close_button), GTK_RELIEF_NONE);
	image = gtk_image_new_from_stock (GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU);
	gtk_container_add (GTK_CONTAINER (close_button), image);
	g_signal_connect (G_OBJECT (close_button), "clicked",
			G_CALLBACK (cb_close_find), NULL);
	gtk_box_pack_start (GTK_BOX (find), close_button, FALSE, FALSE, 0);
	
	label = gtk_label_new (_("Find:"));
	gtk_box_pack_start (GTK_BOX (find), label, FALSE, FALSE, 2);

	GtkWidget *find_entry = gtk_entry_new ();
	latexila.find_entry = find_entry;
	g_signal_connect (G_OBJECT (find_entry), "activate",
			G_CALLBACK (cb_find_entry), NULL);
	gtk_box_pack_start (GTK_BOX (find), find_entry, FALSE, FALSE, 0);

	GtkWidget *previous_button = gtk_button_new_with_label (_("Previous"));
	image = gtk_image_new_from_stock (GTK_STOCK_GO_BACK, GTK_ICON_SIZE_MENU);
	gtk_button_set_image (GTK_BUTTON (previous_button), image);
	gtk_button_set_relief (GTK_BUTTON (previous_button), GTK_RELIEF_NONE);
	g_signal_connect (G_OBJECT (previous_button), "clicked",
			G_CALLBACK (cb_find_previous), NULL);
	gtk_box_pack_start (GTK_BOX (find), previous_button, FALSE, FALSE, 0);

	GtkWidget *next_button = gtk_button_new_with_label (_("Next"));
	image = gtk_image_new_from_stock (GTK_STOCK_GO_FORWARD, GTK_ICON_SIZE_MENU);
	gtk_button_set_image (GTK_BUTTON (next_button), image);
	gtk_button_set_relief (GTK_BUTTON (next_button), GTK_RELIEF_NONE);
	g_signal_connect (G_OBJECT (next_button), "clicked",
			G_CALLBACK (cb_find_next), NULL);
	gtk_box_pack_start (GTK_BOX (find), next_button, FALSE, FALSE, 0);

	GtkWidget *match_case = gtk_check_button_new_with_label (_("Match case"));
	latexila.find_match_case = match_case;
	gtk_box_pack_start (GTK_BOX (find), match_case, FALSE, FALSE, 0);

	/* log zone */
	// horizontal pane:
	// left: action history
	// right: details
	GtkWidget *hpaned = gtk_hpaned_new ();
	latexila.log_hpaned = GTK_PANED (hpaned);
	gtk_paned_set_position (GTK_PANED (hpaned), latexila.prefs.log_hpaned_pos);
	gtk_paned_add2 (GTK_PANED (vpaned), hpaned);

	// action history
	GtkListStore *list_store = gtk_list_store_new (N_COLUMNS_ACTION,
			G_TYPE_STRING, G_TYPE_POINTER);
	latexila.action_log.list_store = list_store;
	
	GtkWidget *list_view = gtk_tree_view_new_with_model (
			GTK_TREE_MODEL (list_store));
	latexila.action_log.list_view = GTK_TREE_VIEW (list_view);
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (
			_("Action history"), renderer, "text", COLUMN_ACTION_TITLE, NULL);	
	gtk_tree_view_append_column (latexila.action_log.list_view, column);
	
	// hide "Action history"
	//gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (list_view), FALSE);

	GtkTreeSelection *select =
		gtk_tree_view_get_selection (latexila.action_log.list_view);
	latexila.action_log.list_selection = select;
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed",
			G_CALLBACK (cb_action_list_changed), NULL);

	// with a scrollbar
	scrollbar = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollbar),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_paned_add1 (GTK_PANED (hpaned), scrollbar);
	gtk_container_add (GTK_CONTAINER (scrollbar), list_view);
	
	// log details
	GtkWidget *log_view = gtk_text_view_new ();
	GtkTextBuffer *log_buffer = gtk_text_view_get_buffer (
			GTK_TEXT_VIEW (log_view));

	latexila.action_log.text_view = GTK_TEXT_VIEW (log_view);
	latexila.action_log.text_buffer = log_buffer;

	gtk_text_buffer_set_text (log_buffer, _("Welcome to LaTeXila!"), -1);
	gtk_text_view_set_editable (GTK_TEXT_VIEW (log_view), FALSE);
	
	// with a scrollbar
	scrollbar = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollbar),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_paned_add2 (GTK_PANED (hpaned), scrollbar);
	gtk_container_add (GTK_CONTAINER (scrollbar), log_view);

	// tags
	gtk_text_buffer_create_tag (log_buffer, "bold",
			"weight", PANGO_WEIGHT_BOLD,
			NULL);
	gtk_text_buffer_create_tag (log_buffer, "error",
			"foreground", "red",
			NULL);
	latexila.action_log.tag_table = gtk_text_buffer_get_tag_table (log_buffer);

	/* statusbar */
	GtkWidget *statusbar = gtk_statusbar_new ();
	latexila.statusbar = GTK_STATUSBAR (statusbar);
	gtk_box_pack_start (GTK_BOX (main_vbox), statusbar, FALSE, FALSE, 0);

	GtkWidget *cursor_position_statusbar = gtk_statusbar_new ();
	latexila.cursor_position = GTK_STATUSBAR (cursor_position_statusbar);
	gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (cursor_position_statusbar),
			FALSE);
	gtk_widget_set_size_request (cursor_position_statusbar, 150, -1);
	gtk_box_pack_end (GTK_BOX (statusbar), cursor_position_statusbar,
			FALSE, TRUE, 0);


	/* show the window */
	gtk_widget_show_all (window);

	if (! latexila.prefs.show_side_pane)
		gtk_widget_hide (latexila.side_pane);

	if (! latexila.prefs.show_edit_toolbar)
		gtk_widget_hide (latexila.edit_toolbar);

	gtk_widget_hide (latexila.go_to_line);
	gtk_widget_hide (latexila.find);

	/* reopen files on startup */
	gchar ** list_opened_docs = (gchar **) latexila.prefs.list_opened_docs->pdata;
	for (int i = 0 ; i < latexila.prefs.list_opened_docs->len
			&& latexila.prefs.reopen_files_on_startup ; i++)
	{
		open_new_document_without_uri (list_opened_docs[i]);
	}
	g_ptr_array_free (latexila.prefs.list_opened_docs, TRUE);
	latexila.prefs.list_opened_docs = g_ptr_array_new ();

	/* if --new-document option is used */
	if (option_new_document)
		cb_new ();

	/* open documents given in arguments */
	for (int i = 1 ; i < argc ; i++)
	{
		gchar *path;
		if (g_path_is_absolute (argv[i]))
			path = g_strdup (argv[i]);
		else
		{
			gchar *current_dir = g_get_current_dir ();
			path = g_build_filename (current_dir, argv[i], NULL);
			g_free (current_dir);
		}

		open_new_document_without_uri (path);
		g_free (path);
	}

	gtk_main ();

	return EXIT_SUCCESS;
}

