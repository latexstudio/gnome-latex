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
#include "print.h"
#include "prefs.h"

static void register_my_stock_icons (void);
static gboolean option_version (const gchar *option_name, const gchar *value,
		gpointer data, GError **error);

latexila_t latexila = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL}; 

static struct {
	gchar *filename;
	gchar *stock_id;
} stock_icons[] = {
	{DATA_DIR "/images/icons/compile_latex.png", "compile_latex"},
	{DATA_DIR "/images/icons/compile_pdflatex.png", "compile_pdflatex"},
	{DATA_DIR "/images/icons/view_dvi.png", "view_dvi"},
	{DATA_DIR "/images/icons/view_pdf.png", "view_pdf"},
	{DATA_DIR "/images/icons/view_ps.png", "view_ps"},
	{DATA_DIR "/images/icons/dvi_to_pdf.png", "dvi_to_pdf"},
	{DATA_DIR "/images/icons/dvi_to_ps.png", "dvi_to_ps"}
};

static gint n_stock_icons = G_N_ELEMENTS (stock_icons);

static void
register_my_stock_icons (void)
{
	GtkIconFactory *icon_factory = gtk_icon_factory_new ();

	for (gint i = 0; i < n_stock_icons; i++)
	{
		GtkIconSet *icon_set = gtk_icon_set_new ();
		GtkIconSource *icon_source = gtk_icon_source_new ();
		gtk_icon_source_set_filename (icon_source, stock_icons[i].filename);
		gtk_icon_set_add_source (icon_set, icon_source);
		gtk_icon_source_free (icon_source);
		gtk_icon_factory_add (icon_factory, stock_icons[i].stock_id, icon_set);
		gtk_icon_set_unref (icon_set);
	}

	gtk_icon_factory_add_default (icon_factory);
	g_object_unref (icon_factory);
}

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
	GOptionEntry options[] = {
		{ "version", 'v', G_OPTION_FLAG_IN_MAIN | G_OPTION_FLAG_NO_ARG,
			G_OPTION_ARG_CALLBACK, (gpointer) option_version,
			N_("Display version informations"), NULL },
		{NULL}
	};

	GOptionContext *context = g_option_context_new ("[FILES]");
#ifdef LATEXILA_NLS_ENABLED
	g_option_context_add_main_entries (context, options, LATEXILA_NLS_PACKAGE);
#else
	g_option_context_add_main_entries (context, options, NULL);
#endif
	g_option_context_add_group (context, gtk_get_option_group (TRUE));
	if (! g_option_context_parse (context, &argc, &argv, &error))
		print_error ("option parsing failed: %s\n", error->message);

#ifdef LATEXILA_NLS_ENABLED
	gtk_init_with_args (&argc, &argv, NULL, options, LATEXILA_NLS_PACKAGE,
			&error);
#else
	gtk_init_with_args (&argc, &argv, NULL, options, NULL, &error);
#endif
	if (error != NULL)
		print_error ("%s", error->message);

	/* localisation */
#ifdef LATEXILA_NLS_ENABLED
	setlocale (LC_ALL, "");
	bindtextdomain (LATEXILA_NLS_PACKAGE, LATEXILA_NLS_LOCALEDIR);
	bind_textdomain_codeset (LATEXILA_NLS_PACKAGE, "UTF-8");
	textdomain (LATEXILA_NLS_PACKAGE);
#endif

	/* preferences */
	latexila.prefs = g_malloc (sizeof (preferences_t));
	load_preferences (latexila.prefs);

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
			latexila.prefs->window_width, latexila.prefs->window_height);
	if (latexila.prefs->window_maximised)
		gtk_window_maximize (GTK_WINDOW (window));

	/* boxes */
	GtkWidget *main_vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (window), main_vbox);

	/* menubar and toolbar */
	register_my_stock_icons ();
	
	// all the actions (for the menu and the toolbar)
	// {name, stock_id, label, accelerator, tooltip, callback}
	// the names come from the XML file
	//
	// TODO try to place "entries" out of the main function without errors
	// because of gettext (the _() macro)
	GtkActionEntry entries[] = {
		{"File", NULL, _("File"), NULL, NULL, NULL},
		{"FileNew", GTK_STOCK_NEW, _("New"), "<Control>N",
			_("New file"), G_CALLBACK (cb_new)},
		{"FileOpen", GTK_STOCK_OPEN, _("Open..."), "<Control>O",
			_("Open a file"), G_CALLBACK (cb_open)},
		{"FileSave", GTK_STOCK_SAVE, _("Save..."), "<Control>S",
			_("Save the current file"), G_CALLBACK (cb_save)},
		{"FileSaveAs", GTK_STOCK_SAVE_AS, _("Save as..."), "<Shift><Control>S",
			_("Save the current file with a different name"), G_CALLBACK (cb_save_as)},
		{"FileClose", GTK_STOCK_CLOSE, _("Close"), "<Control>W",
			_("Close the current file"), G_CALLBACK (cb_close)},
		{"FileQuit", GTK_STOCK_QUIT, _("Quit"), "<Control>Q",
			_("Quit the program"), G_CALLBACK (cb_quit)},
		
		{"Edit", NULL, _("Edit"), NULL, NULL, NULL},
		{"EditUndo", GTK_STOCK_UNDO, _("Undo"), "<Control>Z",
			_("Undo the last action"), G_CALLBACK (cb_undo)},
		{"EditRedo", GTK_STOCK_REDO, _("Redo"), "<Shift><Control>Z",
			_("Redo the last undone action"), G_CALLBACK (cb_redo)},
		{"EditCut", GTK_STOCK_CUT, _("Cut"), "<Control>X",
			_("Cut the selection"), G_CALLBACK (cb_cut)},
		{"EditCopy", GTK_STOCK_COPY, _("Copy"), "<Control>C",
			_("Copy the selection"), G_CALLBACK (cb_copy)},
		{"EditPaste", GTK_STOCK_PASTE, _("Paste"), "<Control>V",
			_("Paste the clipboard"), G_CALLBACK (cb_paste)},
		{"EditDelete", GTK_STOCK_DELETE, _("Delete"), NULL,
			_("Delete the selected text"), G_CALLBACK (cb_delete)},
		{"EditSelectAll", GTK_STOCK_SELECT_ALL, _("Select All"), "<Control>A",
			_("Select the entire document"), G_CALLBACK (cb_select_all)},
		{"EditPreferences", GTK_STOCK_PREFERENCES, _("Preferences"), NULL,
			_("Configure the application"), G_CALLBACK (cb_preferences)},

		{"View", NULL, _("View"), NULL, NULL, NULL},
		{"ViewZoomIn", GTK_STOCK_ZOOM_IN, _("Zoom In"), "<Control>plus",
			_("Enlarge the font"), G_CALLBACK (cb_zoom_in)},
		{"ViewZoomOut", GTK_STOCK_ZOOM_OUT, _("Zoom Out"), "<Control>minus",
			_("Shrink the font"), G_CALLBACK (cb_zoom_out)},
		{"ViewZoomReset", GTK_STOCK_ZOOM_100, _("Reset Zoom"), "<Control>0",
			_("Reset the size of the font"), G_CALLBACK (cb_zoom_reset)},

		{"Search", NULL, _("Search"), NULL, NULL, NULL},
		{"SearchFind", GTK_STOCK_FIND, _("Find..."), "<Control>F",
			_("Search for text"), G_CALLBACK (cb_find)},
		{"SearchReplace", GTK_STOCK_FIND_AND_REPLACE, _("Replace..."), "<Control>H",
			_("Search for and replace text"), G_CALLBACK (cb_replace)},
		
		{"Build", NULL, _("Build"), NULL, NULL, NULL},
		{"compile_latex", "compile_latex", _("Compile (latex)"), "<Release>F5",
			_("Produce the document in DVI format"), G_CALLBACK (cb_latex)},
		{"viewDVI", "view_dvi", _("View DVI"), "<Release>F6",
			_("View the DVI file"), G_CALLBACK (cb_view_dvi)},
		{"compile_pdflatex", "compile_pdflatex", _("Compile (pdflatex)"), "<Release>F7",
			_("Produce the document in PDF format"), G_CALLBACK (cb_pdflatex)},
		{"viewPDF", "view_pdf", _("View PDF"), "<Release>F8",
			_("View the PDF file"), G_CALLBACK (cb_view_pdf)},
		{"DVItoPDF", "dvi_to_pdf", _("DVI to PDF"), NULL,
			_("Convert the DVI document to the PDF format"), G_CALLBACK (cb_dvi_to_pdf)},
		{"DVItoPS", "dvi_to_ps", _("DVI to PS"), NULL,
			_("Convert the DVI document to the PostScript format"), G_CALLBACK (cb_dvi_to_ps)},
		{"viewPS", "view_ps", _("View PS"), NULL,
			_("View the PostScript file"), G_CALLBACK (cb_view_ps)},
		
		{"Help", NULL, _("Help"), NULL, NULL, NULL},
		{"HelpAbout", GTK_STOCK_ABOUT, _("About"), NULL,
			_("About LaTeXila"), G_CALLBACK (cb_about_dialog)}
	};

	// {name, stock_id, label, accelerator, tooltip, callback}
	GtkToggleActionEntry toggle_entries[] = {
		{"ViewSymbols", NULL, _("Symbol tables"), NULL,
			_("Show or hide the symbol tables in the current window"),
			G_CALLBACK (cb_show_symbol_tables)}
	};

	guint nb_entries = G_N_ELEMENTS (entries);
	guint nb_toggle_entries = G_N_ELEMENTS (toggle_entries);

	// recent document
	GtkAction *recent = gtk_recent_action_new ("FileOpenRecent",
			_("Open Recent"), _("Open recently used files"), NULL);
	g_signal_connect (G_OBJECT (recent), "item-activated",
			G_CALLBACK (cb_recent_item_activated), NULL);

	GtkRecentFilter *filter = gtk_recent_filter_new ();
	gtk_recent_filter_add_application (filter, "latexila");
	gtk_recent_chooser_add_filter (GTK_RECENT_CHOOSER (recent), filter);

	// create the action group and the ui manager
	GtkActionGroup *action_group = gtk_action_group_new ("menuActionGroup");
	gtk_action_group_add_actions (action_group, entries, nb_entries, NULL);
	gtk_action_group_add_toggle_actions (action_group, toggle_entries,
			nb_toggle_entries, NULL);
	gtk_action_group_add_action (action_group, recent);
	GtkUIManager *ui_manager = gtk_ui_manager_new ();
	gtk_ui_manager_insert_action_group (ui_manager, action_group, 0);

	// load the xml file
	gchar *xml_file = DATA_DIR "/ui.xml";
	gtk_ui_manager_add_ui_from_file (ui_manager, xml_file, &error);
	if (error != NULL)
	{
		print_error ("building menubar and toolbar failed: %s", error->message);
		g_error_free (error);
		error = NULL;
	}

	// get and put the menubar and the toolbar to the main vbox
	GtkWidget *menubar = gtk_ui_manager_get_widget (ui_manager, "/MainMenu");
	gtk_box_pack_start (GTK_BOX (main_vbox), menubar, FALSE, FALSE, 0);
	GtkWidget *toolbar = gtk_ui_manager_get_widget (ui_manager, "/MainToolbar");
	// toolbar with icons only
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_ICONS);
	gtk_box_pack_start (GTK_BOX (main_vbox), toolbar, FALSE, FALSE, 0);

	// accelerators
	gtk_window_add_accel_group (GTK_WINDOW (window), 
			gtk_ui_manager_get_accel_group (ui_manager));

	// get actions
	latexila.undo = gtk_action_group_get_action (action_group, "EditUndo");
	latexila.redo = gtk_action_group_get_action (action_group, "EditRedo");
	GtkToggleAction *show_symbol_tables = GTK_TOGGLE_ACTION (
			gtk_action_group_get_action (action_group, "ViewSymbols"));
	gtk_toggle_action_set_active (show_symbol_tables,
			latexila.prefs->show_side_pane);

	/* horizontal pane
	 * left: symbol tables
	 * right: the source view and the log zone
	 */
	GtkWidget *main_hpaned = gtk_hpaned_new ();
	latexila.main_hpaned = GTK_PANED (main_hpaned);
	gtk_paned_set_position (GTK_PANED (main_hpaned),
			latexila.prefs->main_hpaned_pos);
	gtk_box_pack_start (GTK_BOX (main_vbox), main_hpaned, TRUE, TRUE, 0);

	/* symbol tables */
	latexila.symbols = g_malloc (sizeof (symbols_t));
	GtkWidget *vbox_symbols = gtk_vbox_new (FALSE, 0);
	latexila.symbols->vbox = vbox_symbols;
	gtk_paned_pack1 (GTK_PANED (main_hpaned), vbox_symbols, TRUE, TRUE);

	init_symbols ();

	/* vertical pane
	 * top: source view
	 * bottom: log zone
	 */
	GtkWidget *vpaned = gtk_vpaned_new ();
	latexila.vpaned = GTK_PANED (vpaned);
	gtk_paned_set_position (GTK_PANED (vpaned), latexila.prefs->vpaned_pos);
	gtk_paned_pack2 (GTK_PANED (main_hpaned), vpaned, TRUE, TRUE);

	/* source view with tabs */
	GtkWidget *notebook = gtk_notebook_new ();
	latexila.notebook = GTK_NOTEBOOK (notebook);
	gtk_notebook_set_scrollable (GTK_NOTEBOOK (notebook), TRUE);
	g_signal_connect (G_OBJECT (notebook), "switch-page",
			G_CALLBACK (cb_page_change), NULL);
	gtk_paned_pack1 (GTK_PANED (vpaned), notebook, TRUE, TRUE);

	/* log zone */
	latexila.action_log = g_malloc (sizeof (action_log_t));

	// horizontal pane:
	// left: action history
	// right: details
	GtkWidget *hpaned = gtk_hpaned_new ();
	latexila.log_hpaned = GTK_PANED (hpaned);
	gtk_paned_set_position (GTK_PANED (hpaned), latexila.prefs->log_hpaned_pos);
	gtk_paned_pack2 (GTK_PANED (vpaned), hpaned, TRUE, TRUE);

	// action history
	GtkListStore *list_store = gtk_list_store_new (N_COLUMNS_ACTION,
			G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);
	latexila.action_log->list_store = list_store;
	
	GtkWidget *list_view = gtk_tree_view_new_with_model (
			GTK_TREE_MODEL (list_store));
	latexila.action_log->list_view = GTK_TREE_VIEW (list_view);
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (
			_("Action history"), renderer, "text", COLUMN_ACTION_TITLE, NULL);	
	gtk_tree_view_append_column (latexila.action_log->list_view, column);
	
	// hide "Action history"
	//gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (list_view), FALSE);

	GtkTreeSelection *select =
		gtk_tree_view_get_selection (latexila.action_log->list_view);
	latexila.action_log->list_selection = select;
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed",
			G_CALLBACK (cb_action_list_changed), NULL);

	// with a scrollbar
	scrollbar = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollbar),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_paned_pack1 (GTK_PANED (hpaned), scrollbar, TRUE, TRUE);
	gtk_container_add (GTK_CONTAINER (scrollbar), list_view);
	
	// log details
	GtkWidget *log_view = gtk_text_view_new ();
	GtkTextBuffer *log_buffer = gtk_text_view_get_buffer (
			GTK_TEXT_VIEW (log_view));

	latexila.action_log->text_view = GTK_TEXT_VIEW (log_view);
	latexila.action_log->text_buffer = log_buffer;

	gtk_text_buffer_set_text (log_buffer, _("Welcome to LaTeXila!"), -1);
	gtk_text_view_set_editable (GTK_TEXT_VIEW (log_view), FALSE);
	
	// with a scrollbar
	scrollbar = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollbar),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_paned_pack2 (GTK_PANED (hpaned), scrollbar, TRUE, TRUE);
	gtk_container_add (GTK_CONTAINER (scrollbar), log_view);

	// tags
	gtk_text_buffer_create_tag (log_buffer, "bold",
			"weight", PANGO_WEIGHT_BOLD,
			NULL);
	gtk_text_buffer_create_tag (log_buffer, "error",
			"foreground", "red",
			NULL);

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

		gchar *uri = g_filename_to_uri (path, NULL, &error);
		if (error != NULL)
		{
			print_warning ("can not open the file \"%s\": %s", argv[i],
					error->message);
			g_error_free (error);
			error = NULL;
		}
		else
			open_new_document (path, uri);

		g_free (path);
		g_free (uri);
	}


	gtk_main ();

	return EXIT_SUCCESS;
}

