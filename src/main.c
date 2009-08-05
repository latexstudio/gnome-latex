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
#include "error.h"

docs_t docs = {NULL, NULL, NULL, NULL, NULL}; 

int
main (int argc, char *argv[])
{
	gtk_init (&argc, &argv);

	/* localisation */
	setlocale (LC_ALL, "");
	bindtextdomain ("latexila", "/usr/share/locale");
	textdomain ("latexila");

	/* main window */
	GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	g_signal_connect (G_OBJECT (window), "delete_event",
			G_CALLBACK (cb_delete_event), NULL);
	gtk_window_set_title (GTK_WINDOW (window), PROGRAM_NAME);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);

	docs.main_window = GTK_WINDOW (window);

	/* boxes */
	GtkWidget *main_vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (window), main_vbox);

	/* menubar and toolbar */
	
	// all the actions (for the menu and the toolbar)
	// name, stock_id, label, accelerator, tooltip, callback
	// the names come from the XML file
	//
	//TODO try to place "entries" out of the main function without errors
	//because of gettext (the _() macro)
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
			_("Undo the last action"), NULL},
		{"EditRedo", GTK_STOCK_REDO, _("Redo"), "<Shift><Control>Z",
			_("Redo the last undone action"), NULL},

		{"View", NULL, _("View"), NULL, NULL, NULL},
		
		{"LaTeX", NULL, "LaTeX", NULL, NULL, NULL},
		{"compile_latex", GTK_STOCK_EXECUTE, _("Compile (latex)"), "<Release>F5",
			_("Produce the document in DVI format"), NULL},
		{"viewDVI", GTK_STOCK_FILE, _("View DVI"), "<Release>F6",
			_("View the DVI file"), NULL},
		{"compile_pdflatex", GTK_STOCK_EXECUTE, _("Compile (pdflatex)"), "<Release>F7",
			_("Produce the document in PDF format"), NULL},
		{"viewPDF", GTK_STOCK_FILE, _("View PDF"), "<Release>F8",
			_("View the PDF file"), NULL},
		
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

	// create the action group and the ui manager
	GtkActionGroup *action_group = gtk_action_group_new ("menuActionGroup");
	gtk_action_group_add_actions (action_group, entries, nb_entries, NULL);
	gtk_action_group_add_toggle_actions (action_group, toggle_entries, nb_toggle_entries, NULL);
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


	/* vertical pane for the source view and the log zone */
	GtkWidget *vpaned = gtk_vpaned_new ();
	gtk_box_pack_start (GTK_BOX (main_vbox), vpaned, TRUE, TRUE, 0);

	/* source view with tabs */
	GtkWidget *notebook = gtk_notebook_new ();
	g_signal_connect (G_OBJECT (notebook), "switch-page",
			G_CALLBACK (cb_page_change), NULL);
	gtk_paned_pack1 (GTK_PANED (vpaned), notebook, TRUE, TRUE);
	docs.notebook = GTK_NOTEBOOK (notebook);

	GtkSourceLanguageManager *lm = gtk_source_language_manager_get_default ();
	docs.lm = lm;

	/* log zone */
	//TODO set a default height
	GtkWidget *log_view = gtk_text_view_new ();
	GtkTextBuffer *log_buffer = gtk_text_view_get_buffer (
			GTK_TEXT_VIEW (log_view));
	gtk_text_buffer_set_text (log_buffer, _("Welcome to LaTeXila!"), -1);
	gtk_text_view_set_editable (GTK_TEXT_VIEW(log_view), FALSE);
	
	// with a scrollbar
	GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_paned_pack2 (GTK_PANED (vpaned), sw, TRUE, TRUE);
	gtk_container_add (GTK_CONTAINER (sw), log_view);

	/* statusbar */
	GtkWidget *statusbar = gtk_statusbar_new ();
	gtk_box_pack_start (GTK_BOX (main_vbox), statusbar, FALSE, FALSE, 0);


	gtk_widget_show_all (window);
	gtk_main ();

	return EXIT_SUCCESS;
}

