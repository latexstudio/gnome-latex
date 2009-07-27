#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <gtk/gtk.h>

static void
menu_add_widget (GtkUIManager *ui_manager, GtkWidget *widget, GtkContainer *box)
{
   gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);
   gtk_widget_show (widget);
}

int
main (int argc, char *argv[])
{
	char *xml_file = "ui.xml";

	// all the actions (for the menu and the toolbar)
	// name, stock_id, label, accelerator, tooltip, callback
	// the names come from the XML file
	GtkActionEntry entries[] =
	{
		{"File", NULL, "File", NULL, NULL, NULL},
		{"FileNew", GTK_STOCK_NEW, "New", "<Control>N", "New file", NULL},
		{"FileOpen", GTK_STOCK_OPEN, "Open...", "<Control>O", "Open a file",
			NULL},
		{"FileSave", GTK_STOCK_SAVE, "Save...", "<Control>S",
			"Save the current file", NULL},
		{"FileSaveAs", GTK_STOCK_SAVE_AS, "Save as...", "<Shift><Control>S",
			"Save the current file with a different name", NULL},
		{"FileQuit", GTK_STOCK_QUIT, "Quit", "<Control>Q", "Quit the program",
			gtk_main_quit},
		
		{"Edit", NULL, "Edit", NULL, NULL, NULL},
		{"EditUndo", GTK_STOCK_UNDO, "Undo", "<Control>Z",
			"Undo the last action", NULL},
		{"EditRedo", GTK_STOCK_REDO, "Redo", "<Shift><Control>Z",
			"Redo the last undone action", NULL},
		
		{"LaTeX", NULL, "LaTeX", NULL, NULL, NULL},
		{"compile_latex", GTK_STOCK_EXECUTE, "Compile (latex)", "<Release>F5",
			"Produce the document in DVI format", NULL},
		{"viewDVI", GTK_STOCK_FILE, "View DVI", "<Release>F6",
			"View the DVI file", NULL},
		{"compile_pdflatex", GTK_STOCK_EXECUTE, "Compile (pdflatex)",
			"<Release>F7", "Produce the document in PDF format", NULL},
		{"viewPDF", GTK_STOCK_FILE, "View PDF", "<Release>F8",
			"View the PDF file", NULL},
		
		{"Help", NULL, "Help", NULL, NULL, NULL},
		{"HelpAbout", GTK_STOCK_ABOUT, "About", NULL, "About LaTeXila", NULL}
	};

	int nb_entries = sizeof (entries) / sizeof (entries[0]);


	gtk_init (&argc, &argv);

	GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	g_signal_connect (G_OBJECT (window), "destroy",
			G_CALLBACK (gtk_main_quit), NULL);
	g_signal_connect (G_OBJECT (window), "delete_event",
			G_CALLBACK (gtk_main_quit), NULL);
	gtk_window_set_title (GTK_WINDOW (window), "LaTeXila");

	GtkWidget *main_vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (window), main_vbox);

	/* menubar */
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
			main_vbox);

	/* statusbar */
	GtkWidget *statusbar = gtk_statusbar_new ();
	gtk_box_pack_end (GTK_BOX (main_vbox), statusbar, FALSE, FALSE, 0);


	gtk_widget_show_all (window);

	gtk_main ();

	return EXIT_SUCCESS;
}
