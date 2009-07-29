#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <locale.h>
#include <libintl.h>
#include <gtk/gtk.h>

#include "main.h"

static void
menu_add_widget (GtkUIManager *ui_manager, GtkWidget *widget, GtkContainer *box)
{
   gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
   gtk_widget_show (widget);
}

int
main (int argc, char *argv[])
{
	/* localisation */	
	setlocale (LC_ALL, "");
	bindtextdomain ("latexila", "/usr/share/locale");
	textdomain ("latexila");

	char *xml_file = "ui.xml";

	// all the actions (for the menu and the toolbar)
	// name, stock_id, label, accelerator, tooltip, callback
	// the names come from the XML file
	GtkActionEntry entries[] =
	{
		{"File", NULL, _("File"), NULL, NULL, NULL},
		{"FileNew", GTK_STOCK_NEW, _("New"), "<Control>N", _("New file"),
			NULL},
		{"FileOpen", GTK_STOCK_OPEN, _("Open..."), "<Control>O",
			_("Open a file"), NULL},
		{"FileSave", GTK_STOCK_SAVE, _("Save..."), "<Control>S",
			_("Save the current file"), NULL},
		{"FileSaveAs", GTK_STOCK_SAVE_AS, _("Save as..."), "<Shift><Control>S",
			_("Save the current file with a different name"), NULL},
		{"FileQuit", GTK_STOCK_QUIT, _("Quit"), "<Control>Q",
			_("Quit the program"), gtk_main_quit},
		
		{"Edit", NULL, _("Edit"), NULL, NULL, NULL},
		{"EditUndo", GTK_STOCK_UNDO, _("Undo"), "<Control>Z",
			_("Undo the last action"), NULL},
		{"EditRedo", GTK_STOCK_REDO, _("Redo"), "<Shift><Control>Z",
			_("Redo the last undone action"), NULL},
		
		{"LaTeX", NULL, "LaTeX", NULL, NULL, NULL},
		{"compile_latex", GTK_STOCK_EXECUTE, _("Compile (latex)"),
			"<Release>F5", _("Produce the document in DVI format"), NULL},
		{"viewDVI", GTK_STOCK_FILE, _("View DVI"), "<Release>F6",
			_("View the DVI file"), NULL},
		{"compile_pdflatex", GTK_STOCK_EXECUTE, _("Compile (pdflatex)"),
			"<Release>F7", _("Produce the document in PDF format"), NULL},
		{"viewPDF", GTK_STOCK_FILE, _("View PDF"), "<Release>F8",
			_("View the PDF file"), NULL},
		
		{"Help", NULL, _("Help"), NULL, NULL, NULL},
		{"HelpAbout", GTK_STOCK_ABOUT, _("About"), NULL, _("About LaTeXila"), NULL}
	};

	int nb_entries = sizeof (entries) / sizeof (entries[0]);


	gtk_init (&argc, &argv);

	/* main window */

	GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	g_signal_connect (G_OBJECT (window), "destroy",
			G_CALLBACK (gtk_main_quit), NULL);
	g_signal_connect (G_OBJECT (window), "delete_event",
			G_CALLBACK (gtk_main_quit), NULL);
	gtk_window_set_title (GTK_WINDOW (window), "LaTeXila");

	/* boxes and panes */

	GtkWidget *vbox1 = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (window), vbox1);

	GtkWidget *vbox2 = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_end (GTK_BOX (vbox1), vbox2, TRUE, TRUE, 0);

	GtkWidget *vpaned = gtk_vpaned_new ();
	gtk_box_pack_start (GTK_BOX (vbox2), vpaned, TRUE, TRUE, 0);

	/* menubar and toolbar */
	GtkUIManager *ui_manager = gtk_ui_manager_new ();
	GtkActionGroup *action_group = gtk_action_group_new ("menuActionGroup");
	gtk_action_group_add_actions (action_group, entries, nb_entries, NULL);
	gtk_ui_manager_insert_action_group (ui_manager, action_group, 0);
	int tmp = gtk_ui_manager_add_ui_from_file (ui_manager, xml_file, NULL);
	if (tmp == 0)
	{
		fprintf (stderr, "Error with %s\n", xml_file);
		return EXIT_FAILURE;
	}
	g_signal_connect (ui_manager, "add-widget", G_CALLBACK (menu_add_widget),
			vbox1);

	/* source view */
	GtkWidget *source_view = gtk_text_view_new ();
	GtkTextBuffer *source_buffer = gtk_text_view_get_buffer (
			GTK_TEXT_VIEW (source_view));
	char *source_default_text = "\\documentclass[a4paper,11pt]{article}\n"
		"\\begin{document}\n"
		"\\end{document}";
	gtk_text_buffer_set_text (source_buffer, source_default_text, -1);

	// with a scrollbar
	GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_paned_pack1 (GTK_PANED (vpaned), sw, TRUE, TRUE);
	gtk_container_add (GTK_CONTAINER (sw), source_view);
	
	/* log zone */
	GtkWidget *log_view = gtk_text_view_new ();
	GtkTextBuffer *log_buffer = gtk_text_view_get_buffer (
			GTK_TEXT_VIEW (log_view));
	gtk_text_buffer_set_text (log_buffer, _("Welcome to LaTeXila!"), -1);
	gtk_text_view_set_editable (GTK_TEXT_VIEW(log_view), FALSE);
	
	// with a scrollbar
	sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_paned_pack2 (GTK_PANED (vpaned), sw, TRUE, TRUE);
	gtk_container_add (GTK_CONTAINER (sw), log_view);

	/* statusbar */
	GtkWidget *statusbar = gtk_statusbar_new ();
	gtk_box_pack_start (GTK_BOX (vbox2), statusbar, FALSE, FALSE, 0);


	gtk_widget_show_all (window);

	gtk_main ();

	return EXIT_SUCCESS;
}
