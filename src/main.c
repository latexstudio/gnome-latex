#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <libintl.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagemanager.h>

#include "main.h"
#include "callbacks.h"
#include "print.h"

latexila_t latexila = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL}; 

static struct {     
	gchar *filename;     
	gchar *stock_id;     
} stock_icons[] = {     
	{DATA_DIR "/icons/compile_latex.png", "compile_latex"},
	{DATA_DIR "/icons/compile_pdflatex.png", "compile_pdflatex"},
	{DATA_DIR "/icons/view_dvi.png", "view_dvi"},
	{DATA_DIR "/icons/view_pdf.png", "view_pdf"},
	{DATA_DIR "/icons/view_ps.png", "view_ps"},
	{DATA_DIR "/icons/dvi_to_pdf.png", "dvi_to_pdf"},
	{DATA_DIR "/icons/dvi_to_ps.png", "dvi_to_ps"}
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

int
main (int argc, char *argv[])
{
	gtk_init (&argc, &argv);

	/* localisation */
	setlocale (LC_ALL, "");
	bindtextdomain ("latexila", "/usr/share/locale");
	textdomain ("latexila");

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
	g_signal_connect (G_OBJECT (window), "delete_event",
			G_CALLBACK (cb_delete_event), NULL);
	gtk_window_set_title (GTK_WINDOW (window), PROGRAM_NAME);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);

	latexila.main_window = GTK_WINDOW (window);

	/* boxes */
	GtkWidget *main_vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (window), main_vbox);

	/* menubar and toolbar */
	register_my_stock_icons ();
	
	// all the actions (for the menu and the toolbar)
	// name, stock_id, label, accelerator, tooltip, callback
	// the names come from the XML file
	//
	// TODO try to place "entries" out of the main function without errors
	// because of gettext (the _() macro)
	GtkActionEntry entries[] =
	{
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

		{"View", NULL, _("View"), NULL, NULL, NULL},

		{"Search", NULL, _("Search"), NULL, NULL, NULL},
		{"SearchFind", GTK_STOCK_FIND, _("Find..."), "<Control>F",
			_("Search for text"), G_CALLBACK (cb_find)},
		{"SearchReplace", GTK_STOCK_FIND_AND_REPLACE, _("Replace..."), "<Control>H",
			_("Search for and replace text"), G_CALLBACK (cb_replace)},
		
		{"LaTeX", NULL, "LaTeX", NULL, NULL, NULL},
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

	// name, stock_id, label, accelerator, tooltip, callback
	GtkToggleActionEntry toggle_entries[] =
	{
		{"ViewLineNumbers", NULL, _("Line numbers"), NULL,
			_("Display line numbers"), G_CALLBACK (cb_line_numbers)}
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
	gtk_action_group_add_toggle_actions (action_group, toggle_entries, nb_toggle_entries, NULL);
	gtk_action_group_add_action (action_group, recent);
	GtkUIManager *ui_manager = gtk_ui_manager_new ();
	gtk_ui_manager_insert_action_group (ui_manager, action_group, 0);

	// load the xml file
	char *xml_file = "ui.xml";
	GError *error = NULL;
	gtk_ui_manager_add_ui_from_file (ui_manager, xml_file, &error);
	if (error)
	{
		print_error ("building menubar and toolbar failed: %s", error->message);
		g_error_free (error);
	}

	// get and put the menubar and the toolbar to the main vbox
	GtkWidget *menubar = gtk_ui_manager_get_widget (ui_manager, "/MainMenu");
	gtk_box_pack_start (GTK_BOX (main_vbox), menubar, FALSE, FALSE, 0);
	GtkWidget *toolbar = gtk_ui_manager_get_widget (ui_manager, "/MainToolbar");
	gtk_box_pack_start (GTK_BOX (main_vbox), toolbar, FALSE, FALSE, 0);

	// accelerators
	gtk_window_add_accel_group (GTK_WINDOW (window), 
			gtk_ui_manager_get_accel_group (ui_manager));

	// get actions
	latexila.undo = gtk_ui_manager_get_action (ui_manager, "/MainMenu/Edit/Undo");
	latexila.redo = gtk_ui_manager_get_action (ui_manager, "/MainMenu/Edit/Redo");


	/* vertical pane for the source view and the log zone */
	GtkWidget *vpaned = gtk_vpaned_new ();
	gtk_paned_set_position (GTK_PANED (vpaned), 380);
	gtk_box_pack_start (GTK_BOX (main_vbox), vpaned, TRUE, TRUE, 0);

	/* source view with tabs */
	GtkWidget *notebook = gtk_notebook_new ();
	g_signal_connect (G_OBJECT (notebook), "switch-page",
			G_CALLBACK (cb_page_change), NULL);
	gtk_paned_pack1 (GTK_PANED (vpaned), notebook, TRUE, TRUE);
	latexila.notebook = GTK_NOTEBOOK (notebook);

	/* log zone */
	//TODO set default height and width
	GtkWidget *hpaned = gtk_hpaned_new ();
	gtk_paned_set_position (GTK_PANED (hpaned), 190);
	gtk_paned_pack2 (GTK_PANED (vpaned), hpaned, TRUE, TRUE);

	// actions list
	GtkListStore *list_store = gtk_list_store_new (N_COLUMNS_ACTION,
			G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);
	latexila.list_store = list_store;
	
	GtkWidget *list_view = gtk_tree_view_new_with_model (
			GTK_TREE_MODEL (list_store));
	latexila.list_view = GTK_TREE_VIEW (list_view);
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
	GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes (
			_("Action history"), renderer, "text", COLUMN_ACTION_TITLE, NULL);	
	gtk_tree_view_append_column (latexila.list_view, column);
	
	// hide "Action history"
	//gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (list_view), FALSE);

	GtkTreeSelection *select = gtk_tree_view_get_selection (latexila.list_view);
	latexila.list_selection = select;
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed",
			G_CALLBACK (cb_action_list_changed), NULL);

	// with a scrollbar
	GtkWidget *sw1 = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw1),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_paned_pack1 (GTK_PANED (hpaned), sw1, TRUE, TRUE);
	gtk_container_add (GTK_CONTAINER (sw1), list_view);
	
	// log details
	GtkWidget *log_view = gtk_text_view_new ();
	GtkTextBuffer *log_buffer = gtk_text_view_get_buffer (
			GTK_TEXT_VIEW (log_view));
	gtk_text_buffer_set_text (log_buffer, _("Welcome to LaTeXila!"), -1);
	gtk_text_view_set_editable (GTK_TEXT_VIEW(log_view), FALSE);
	
	// with a scrollbar
	GtkWidget *sw2 = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw2),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_paned_pack2 (GTK_PANED (hpaned), sw2, TRUE, TRUE);
	gtk_container_add (GTK_CONTAINER (sw2), log_view);

	latexila.log = GTK_TEXT_VIEW (log_view);

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


	gtk_widget_show_all (window);
	gtk_main ();

	return EXIT_SUCCESS;
}

