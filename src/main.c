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
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagemanager.h>

#include "main.h"
#include "config.h"
#include "callbacks.h"
#include "print.h"

static void register_my_stock_icons (void);
static GtkListStore * get_symbol_store (const struct symbol symbols[]);

latexila_t latexila = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL}; 

static struct symbol symbols_greek[] = {
	{DATA_DIR "/images/greek/01-alpha.png", "\\alpha", NULL},
	{DATA_DIR "/images/greek/02-beta.png", "\\beta", NULL},
	{DATA_DIR "/images/greek/03-gamma.png", "\\gamma", NULL},
	{DATA_DIR "/images/greek/04-delta.png", "\\delta", NULL},
	{DATA_DIR "/images/greek/05-epsilon.png", "\\epsilon", NULL},
	{DATA_DIR "/images/greek/06-varepsilon.png", "\\varepsilon", NULL},
	{DATA_DIR "/images/greek/07-zeta.png", "\\zeta", NULL},
	{DATA_DIR "/images/greek/08-eta.png", "\\eta", NULL},
	{DATA_DIR "/images/greek/09-theta.png", "\\theta", NULL},
	{DATA_DIR "/images/greek/10-vartheta.png", "\\vartheta", NULL},
	{DATA_DIR "/images/greek/11-iota.png", "\\iota", NULL},
	{DATA_DIR "/images/greek/12-kappa.png", "\\kappa", NULL},
	{DATA_DIR "/images/greek/13-lambda.png", "\\lambda", NULL},
	{DATA_DIR "/images/greek/14-mu.png", "\\mu", NULL},
	{DATA_DIR "/images/greek/15-nu.png", "\\nu", NULL},
	{DATA_DIR "/images/greek/16-xi.png", "\\xi", NULL},
	{DATA_DIR "/images/greek/17-o.png", "o", NULL},
	{DATA_DIR "/images/greek/18-pi.png", "\\pi", NULL},
	{DATA_DIR "/images/greek/19-varpi.png", "\\varpi", NULL},
	{DATA_DIR "/images/greek/20-rho.png", "\\rho", NULL},
	{DATA_DIR "/images/greek/21-varrho.png", "\\varrho", NULL},
	{DATA_DIR "/images/greek/22-sigma.png", "\\sigma", NULL},
	{DATA_DIR "/images/greek/23-varsigma.png", "\\varsigma", NULL},
	{DATA_DIR "/images/greek/24-tau.png", "\\tau", NULL},
	{DATA_DIR "/images/greek/25-upsilon.png", "\\upsilon", NULL},
	{DATA_DIR "/images/greek/26-phi.png", "\\phi", NULL},
	{DATA_DIR "/images/greek/27-varphi.png", "\\varphi", NULL},
	{DATA_DIR "/images/greek/28-chi.png", "\\chi", NULL},
	{DATA_DIR "/images/greek/29-psi.png", "\\psi", NULL},
	{DATA_DIR "/images/greek/30-omega.png", "\\omega", NULL},
	{DATA_DIR "/images/greek/31-A.png", "A", NULL},
	{DATA_DIR "/images/greek/32-B.png", "B", NULL},
	{DATA_DIR "/images/greek/33-Gamma.png", "\\Gamma", NULL},
	{DATA_DIR "/images/greek/34-varGamma.png", "\\varGamma", "amsmath"},
	{DATA_DIR "/images/greek/35-Delta.png", "\\Delta", NULL},
	{DATA_DIR "/images/greek/36-varDelta.png", "\\varDelta", "amsmath"},
	{DATA_DIR "/images/greek/37-E.png", "E", NULL},
	{DATA_DIR "/images/greek/38-Z.png", "Z", NULL},
	{DATA_DIR "/images/greek/39-H.png", "H", NULL},
	{DATA_DIR "/images/greek/40-Theta.png", "\\Theta", NULL},
	{DATA_DIR "/images/greek/41-varTheta.png", "\\varTheta", "amsmath"},
	{DATA_DIR "/images/greek/42-I.png", "I", NULL},
	{DATA_DIR "/images/greek/43-K.png", "K", NULL},
	{DATA_DIR "/images/greek/44-Lambda.png", "\\Lambda", NULL},
	{DATA_DIR "/images/greek/45-varLambda.png", "\\varLambda", "amsmath"},
	{DATA_DIR "/images/greek/46-M.png", "M", NULL},
	{DATA_DIR "/images/greek/47-N.png", "N", NULL},
	{DATA_DIR "/images/greek/48-Xi.png", "\\Xi", NULL},
	{DATA_DIR "/images/greek/49-varXi.png", "\\varXi", "amsmath"},
	{DATA_DIR "/images/greek/50-O.png", "O", NULL},
	{DATA_DIR "/images/greek/51-Pi.png", "\\Pi", NULL},
	{DATA_DIR "/images/greek/52-varPi.png", "\\varPi", "amsmath"},
	{DATA_DIR "/images/greek/53-P.png", "P", NULL},
	{DATA_DIR "/images/greek/54-Sigma.png", "\\Sigma", NULL},
	{DATA_DIR "/images/greek/55-varSigma.png", "\\varSigma", "amsmath"},
	{DATA_DIR "/images/greek/56-T.png", "T", NULL},
	{DATA_DIR "/images/greek/57-Upsilon.png", "\\Upsilon", NULL},
	{DATA_DIR "/images/greek/58-varUpsilon.png", "\\varUpsilon", "amsmath"},
	{DATA_DIR "/images/greek/59-Phi.png", "\\Phi", NULL},
	{DATA_DIR "/images/greek/60-varPhi.png", "\\varPhi", "amsmath"},
	{DATA_DIR "/images/greek/61-X.png", "X", NULL},
	{DATA_DIR "/images/greek/62-Psi.png", "\\Psi", NULL},
	{DATA_DIR "/images/greek/63-varPsi.png", "\\varPsi", "amsmath"},
	{DATA_DIR "/images/greek/64-Omega.png", "\\Omega", NULL},
	{DATA_DIR "/images/greek/65-varOmega.png", "\\varOmega", "amsmath"},
	{NULL, NULL, NULL}
};

static struct symbol symbols_arrows[] = {
	{DATA_DIR "/images/arrows/01.png", "\\leftarrow", NULL},
	{DATA_DIR "/images/arrows/02.png", "\\leftrightarrow", NULL},
	{DATA_DIR "/images/arrows/03.png", "\\rightarrow", NULL},
	{DATA_DIR "/images/arrows/04.png", "\\mapsto", NULL},
	{DATA_DIR "/images/arrows/05.png", "\\longleftarrow", NULL},
	{DATA_DIR "/images/arrows/06.png", "\\longleftrightarrow", NULL},
	{DATA_DIR "/images/arrows/07.png", "\\longrightarrow", NULL},
	{DATA_DIR "/images/arrows/08.png", "\\longmapsto", NULL},
	{DATA_DIR "/images/arrows/09.png", "\\downarrow", NULL},
	{DATA_DIR "/images/arrows/10.png", "\\updownarrow", NULL},
	{DATA_DIR "/images/arrows/11.png", "\\uparrow", NULL},
	{DATA_DIR "/images/arrows/12.png", "\\nwarrow", NULL},
	{DATA_DIR "/images/arrows/13.png", "\\searrow", NULL},
	{DATA_DIR "/images/arrows/14.png", "\\nearrow", NULL},
	{DATA_DIR "/images/arrows/15.png", "\\swarrow", NULL},
	{DATA_DIR "/images/arrows/16.png", "\\textdownarrow", "textcomp"},
	{DATA_DIR "/images/arrows/17.png", "\\textuparrow", "textcomp"},
	{DATA_DIR "/images/arrows/18.png", "\\textleftarrow", "textcomp"},
	{DATA_DIR "/images/arrows/19.png", "\\textrightarrow", "textcomp"},
	{DATA_DIR "/images/arrows/20.png", "\\nleftarrow", "amssymb"},
	{DATA_DIR "/images/arrows/21.png", "\\nleftrightarrow", "amssymb"},
	{DATA_DIR "/images/arrows/22.png", "\\nrightarrow", "amssymb"},
	{DATA_DIR "/images/arrows/23.png", "\\hookleftarrow", NULL},
	{DATA_DIR "/images/arrows/24.png", "\\hookrightarrow", NULL},
	{DATA_DIR "/images/arrows/25.png", "\\twoheadleftarrow", "amssymb"},
	{DATA_DIR "/images/arrows/26.png", "\\twoheadrightarrow", "amssymb"},
	{DATA_DIR "/images/arrows/27.png", "\\leftarrowtail", "amssymb"},
	{DATA_DIR "/images/arrows/28.png", "\\rightarrowtail", "amssymb"},
	{DATA_DIR "/images/arrows/29.png", "\\Leftarrow", NULL},
	{DATA_DIR "/images/arrows/30.png", "\\Leftrightarrow", NULL},
	{DATA_DIR "/images/arrows/31.png", "\\Rightarrow", NULL},
	{DATA_DIR "/images/arrows/32.png", "\\Longleftarrow", NULL},
	{DATA_DIR "/images/arrows/33.png", "\\Longleftrightarrow", NULL},
	{DATA_DIR "/images/arrows/34.png", "\\Longrightarrow", NULL},
	{DATA_DIR "/images/arrows/35.png", "\\Updownarrow", NULL},
	{DATA_DIR "/images/arrows/36.png", "\\Uparrow", NULL},
	{DATA_DIR "/images/arrows/37.png", "\\Downarrow", NULL},
	{DATA_DIR "/images/arrows/38.png", "\\nLeftarrow", "amssymb"},
	{DATA_DIR "/images/arrows/39.png", "\\nLeftrightarrow", "amssymb"},
	{DATA_DIR "/images/arrows/40.png", "\\nRightarrow", "amssymb"},
	{DATA_DIR "/images/arrows/41.png", "\\leftleftarrows", "amssymb"},
	{DATA_DIR "/images/arrows/42.png", "\\leftrightarrows", "amssymb"},
	{DATA_DIR "/images/arrows/43.png", "\\rightleftarrows", "amssymb"},
	{DATA_DIR "/images/arrows/44.png", "\\rightrightarrows", "amssymb"},
	{DATA_DIR "/images/arrows/45.png", "\\downdownarrows", "amssymb"},
	{DATA_DIR "/images/arrows/46.png", "\\upuparrows", "amssymb"},
	{DATA_DIR "/images/arrows/47.png", "\\circlearrowleft", "amssymb"},
	{DATA_DIR "/images/arrows/48.png", "\\circlearrowright", "amssymb"},
	{DATA_DIR "/images/arrows/49.png", "\\curvearrowleft", "amssymb"},
	{DATA_DIR "/images/arrows/50.png", "\\curvearrowright", "amssymb"},
	{DATA_DIR "/images/arrows/51.png", "\\Lsh", "amssymb"},
	{DATA_DIR "/images/arrows/52.png", "\\Rsh", "amssymb"},
	{DATA_DIR "/images/arrows/53.png", "\\looparrowleft", "amssymb"},
	{DATA_DIR "/images/arrows/54.png", "\\looparrowright", "amssymb"},
	{DATA_DIR "/images/arrows/55.png", "\\dashleftarrow", "amssymb"},
	{DATA_DIR "/images/arrows/56.png", "\\dashrightarrow", "amssymb"},
	{DATA_DIR "/images/arrows/57.png", "\\leftrightsquigarrow", "amssymb"},
	{DATA_DIR "/images/arrows/58.png", "\\rightsquigarrow", "amssymb"},
	{DATA_DIR "/images/arrows/59.png", "\\Lleftarrow", "amssymb"},
	{DATA_DIR "/images/arrows/60.png", "\\leftharpoondown", NULL},
	{DATA_DIR "/images/arrows/61.png", "\\rightharpoondown", NULL},
	{DATA_DIR "/images/arrows/62.png", "\\leftharpoonup", NULL},
	{DATA_DIR "/images/arrows/63.png", "\\rightharpoonup", NULL},
	{DATA_DIR "/images/arrows/64.png", "\\rightleftharpoons", NULL},
	{DATA_DIR "/images/arrows/65.png", "\\leftrightharpoons", "amssymb"},
	{DATA_DIR "/images/arrows/66.png", "\\downharpoonleft", "amssymb"},
	{DATA_DIR "/images/arrows/67.png", "\\upharpoonleft", "amssymb"},
	{DATA_DIR "/images/arrows/68.png", "\\downharpoonright", "amssymb"},
	{DATA_DIR "/images/arrows/69.png", "\\upharpoonright", "amssymb"},
	{NULL, NULL, NULL}
};

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

// symbols must be NULL-terminated
static GtkListStore *
get_symbol_store (const struct symbol symbols[])
{
	GError *error = NULL;
	GtkListStore *symbol_store = gtk_list_store_new (N_COLUMNS_SYMBOL,
			GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING);

	for (gint i = 0 ; symbols[i].filename != NULL ; i++)
	{
		struct symbol current_symbol = symbols[i];

		GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file (current_symbol.filename,
				&error);
		if (error != NULL)
		{
			print_warning ("impossible to load the symbol: %s", error->message);
			g_error_free (error);
			continue;
		}

		gchar *tooltip;
		if (current_symbol.package_required != NULL)
		{
			tooltip = g_strdup_printf ("%s (package %s)",
					current_symbol.latex_command,
					current_symbol.package_required);
		}
		else
			tooltip = g_strdup (current_symbol.latex_command);

		GtkTreeIter iter;
		gtk_list_store_append (symbol_store, &iter);
		gtk_list_store_set (symbol_store, &iter,
				COLUMN_SYMBOL_PIXBUF, pixbuf,
				COLUMN_SYMBOL_COMMAND, current_symbol.latex_command,
				COLUMN_SYMBOL_TOOLTIP, tooltip,
				-1);
		g_free (tooltip);
	}

	return symbol_store;
}

int
main (int argc, char *argv[])
{
	GError *error = NULL;
	GtkWidget *scrollbar;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	gtk_init (&argc, &argv);

#ifdef LATEXILA_NLS_ENABLED
	/* i18n, l10n */
	setlocale (LC_ALL, "");
	bindtextdomain (LATEXILA_NLS_PACKAGE, LATEXILA_NLS_LOCALEDIR);
	bind_textdomain_codeset (LATEXILA_NLS_PACKAGE, "UTF-8");
	textdomain (LATEXILA_NLS_PACKAGE);
#endif

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
	gtk_toggle_action_set_active (show_symbol_tables, TRUE);

	/* horizontal pane
	 * left: symbol tables
	 * right: the source view and the log zone
	 */
	GtkWidget *main_hpaned = gtk_hpaned_new ();
	gtk_paned_set_position (GTK_PANED (main_hpaned), 180);
	gtk_box_pack_start (GTK_BOX (main_vbox), main_hpaned, TRUE, TRUE, 0);

	/* symbol tables */
	GtkWidget *vbox_symbols = gtk_vbox_new (FALSE, 0);
	latexila.symbol_tables = vbox_symbols;
	gtk_paned_pack1 (GTK_PANED (main_hpaned), vbox_symbols, TRUE, TRUE);

	// store the categories
	struct {
		gchar *name;
		gchar *icon;
	} categories[] = {
		{"Greek", DATA_DIR "/images/greek/18-pi.png"},
		{"Arrows", DATA_DIR "/images/arrows/58.png"}
	};

	GtkListStore *categories_store = gtk_list_store_new (N_COLUMNS_CAT,
			GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT);

	gint nb_categories = G_N_ELEMENTS (categories);
	for (gint i = 0 ; i < nb_categories ; i++)
	{
		GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file (categories[i].icon,
				&error);
		if (error != NULL)
		{
			print_warning ("impossible to load the symbol: %s", error->message);
			g_error_free (error);
			continue;
		}

		GtkTreeIter iter;
		gtk_list_store_append (categories_store, &iter);
		gtk_list_store_set (categories_store, &iter,
				COLUMN_CAT_ICON, pixbuf,
				COLUMN_CAT_NAME, categories[i].name,
				COLUMN_CAT_NUM, i,
				-1);
	}

	// show the categories
	GtkWidget *categories_view = gtk_icon_view_new_with_model (
			GTK_TREE_MODEL (categories_store));
	gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (categories_view),
			COLUMN_CAT_ICON);
	gtk_icon_view_set_text_column (GTK_ICON_VIEW (categories_view),
			COLUMN_CAT_NAME);
	gtk_icon_view_set_selection_mode (GTK_ICON_VIEW (categories_view),
			GTK_SELECTION_SINGLE);
	/*
	gtk_icon_view_set_orientation (GTK_ICON_VIEW (categories_view),
			GTK_ORIENTATION_HORIZONTAL);
	gtk_icon_view_set_columns (GTK_ICON_VIEW (categories_view), 1);
	*/
	gtk_icon_view_set_spacing (GTK_ICON_VIEW (categories_view), 0);
	gtk_icon_view_set_row_spacing (GTK_ICON_VIEW (categories_view), 0);
	gtk_icon_view_set_column_spacing (GTK_ICON_VIEW (categories_view), 0);

	g_signal_connect (G_OBJECT (categories_view), "selection-changed",
			G_CALLBACK (cb_category_symbols_selected), NULL);

	gtk_box_pack_start (GTK_BOX (vbox_symbols), categories_view, FALSE, FALSE, 0);

	// store all the symbols
	// Attention, we must save the GtkListStore in latexila.symbol_stores in
	// the same order than the structure "categories" above
	latexila.symbol_stores[0] = get_symbol_store (symbols_greek);
	latexila.symbol_stores[1] = get_symbol_store (symbols_arrows);

	// show the symbols
	GtkWidget *symbol_view = gtk_icon_view_new_with_model (
			GTK_TREE_MODEL (latexila.symbol_stores[0]));
	latexila.symbol_view = GTK_ICON_VIEW (symbol_view);
	gtk_icon_view_set_pixbuf_column (latexila.symbol_view,
			COLUMN_SYMBOL_PIXBUF);
	gtk_icon_view_set_tooltip_column (latexila.symbol_view,
			COLUMN_SYMBOL_TOOLTIP);
	gtk_icon_view_set_selection_mode (latexila.symbol_view,
			GTK_SELECTION_SINGLE);
	gtk_icon_view_set_spacing (latexila.symbol_view, 0);
	gtk_icon_view_set_row_spacing (latexila.symbol_view, 0);
	gtk_icon_view_set_column_spacing (latexila.symbol_view, 0);

	g_signal_connect (G_OBJECT (symbol_view), "selection-changed",
			G_CALLBACK (cb_symbol_selected), NULL);

	// with a scrollbar
	scrollbar = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollbar),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (scrollbar), symbol_view);
	gtk_box_pack_start (GTK_BOX (vbox_symbols), scrollbar, TRUE, TRUE, 0);

	/* vertical pane
	 * top: source view
	 * bottom: log zone
	 */
	GtkWidget *vpaned = gtk_vpaned_new ();
	gtk_paned_set_position (GTK_PANED (vpaned), 380);
	gtk_paned_pack2 (GTK_PANED (main_hpaned), vpaned, TRUE, TRUE);

	/* source view with tabs */
	GtkWidget *notebook = gtk_notebook_new ();
	latexila.notebook = GTK_NOTEBOOK (notebook);
	g_signal_connect (G_OBJECT (notebook), "switch-page",
			G_CALLBACK (cb_page_change), NULL);
	gtk_paned_pack1 (GTK_PANED (vpaned), notebook, TRUE, TRUE);

	/* log zone */
	// horizontal pane:
	// left: action history
	// right: details
	GtkWidget *hpaned = gtk_hpaned_new ();
	gtk_paned_set_position (GTK_PANED (hpaned), 190);
	gtk_paned_pack2 (GTK_PANED (vpaned), hpaned, TRUE, TRUE);

	// action history
	GtkListStore *list_store = gtk_list_store_new (N_COLUMNS_ACTION,
			G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);
	latexila.list_store = list_store;
	
	GtkWidget *list_view = gtk_tree_view_new_with_model (
			GTK_TREE_MODEL (list_store));
	latexila.list_view = GTK_TREE_VIEW (list_view);
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (
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
	scrollbar = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollbar),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_paned_pack1 (GTK_PANED (hpaned), scrollbar, TRUE, TRUE);
	gtk_container_add (GTK_CONTAINER (scrollbar), list_view);
	
	// log details
	GtkWidget *log_view = gtk_text_view_new ();
	GtkTextBuffer *log_buffer = gtk_text_view_get_buffer (
			GTK_TEXT_VIEW (log_view));
	gtk_text_buffer_set_text (log_buffer, _("Welcome to LaTeXila!"), -1);
	gtk_text_view_set_editable (GTK_TEXT_VIEW(log_view), FALSE);
	
	// with a scrollbar
	scrollbar = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollbar),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_paned_pack2 (GTK_PANED (hpaned), scrollbar, TRUE, TRUE);
	gtk_container_add (GTK_CONTAINER (scrollbar), log_view);

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

	/* user preferences */
	gboolean default_value_show_line_numbers = FALSE;
	gchar *default_value_font = "Monospace 10";
	gchar *default_value_command_view = "evince";

	gchar *pref_file = g_strdup_printf ("%s/.latexila", g_get_home_dir ());
	latexila.pref_file = pref_file;
	latexila.key_file = g_key_file_new ();
	gboolean prefs_loaded = FALSE;
	error = NULL;

	if (g_file_test (pref_file, G_FILE_TEST_IS_REGULAR))
	{
		g_key_file_load_from_file (latexila.key_file, pref_file,
				G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &error);
		if (error != NULL)
		{
			print_warning ("load user preferences failed: %s", error->message);
			g_error_free (error);
		}
		else
		{
			/* check if all keys exist
			 * if not, set the default value for that key
			 */
			gboolean prefs_saved = TRUE;
			g_key_file_get_boolean (latexila.key_file, PROGRAM_NAME,
					"show_line_numbers", &error);
			if (error != NULL)
			{
				g_key_file_set_boolean (latexila.key_file, PROGRAM_NAME,
						"show_line_numbers", default_value_show_line_numbers);
				g_error_free (error);
				prefs_saved = FALSE;
			}

			g_key_file_get_string (latexila.key_file, PROGRAM_NAME,
					"font", &error);
			if (error != NULL)
			{
				g_key_file_set_string (latexila.key_file, PROGRAM_NAME,
						"font", default_value_font);
				g_error_free (error);
				prefs_saved = FALSE;
			}

			g_key_file_get_string (latexila.key_file, PROGRAM_NAME,
					"command_view", &error);
			if (error != NULL)
			{
				g_key_file_set_string (latexila.key_file, PROGRAM_NAME,
						"command_view", default_value_command_view);
				g_error_free (error);
				prefs_saved = FALSE;
			}

			if (! prefs_saved)
				save_preferences ();

			print_info ("load user preferences: OK");
			prefs_loaded = TRUE;
		}
	}

	if (! prefs_loaded)
	{
		// set default values
		g_key_file_set_boolean (latexila.key_file, PROGRAM_NAME,
				"show_line_numbers", default_value_show_line_numbers);
		g_key_file_set_string (latexila.key_file, PROGRAM_NAME,
				"font", default_value_font);
		g_key_file_set_string (latexila.key_file, PROGRAM_NAME,
				"command_view", default_value_command_view);

		save_preferences ();
	}

	gchar *font_string = g_key_file_get_string (latexila.key_file,
			PROGRAM_NAME, "font", NULL);
	latexila.font_desc = pango_font_description_from_string (font_string);
	latexila.font_size = pango_font_description_get_size (latexila.font_desc);


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
			path = g_strdup_printf ("%s/%s", current_dir, argv[i]);
			g_free (current_dir);
		}

		gchar *uri = g_filename_to_uri (path, NULL, NULL);
		if (uri != NULL)
			open_new_document (path, uri);
		else
			print_warning ("can not open the file \"%s\"", argv[i]);

		g_free (path);
		g_free (uri);
	}

	gtk_main ();

	return EXIT_SUCCESS;
}

