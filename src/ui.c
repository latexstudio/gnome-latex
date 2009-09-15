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
#include "cb_latex.h"

static void register_my_stock_icons (void);

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
	{DATA_DIR "/images/icons/dvi_to_ps.png", "dvi_to_ps"},
	{DATA_DIR "/images/icons/textbf.png", "bold"},
	{DATA_DIR "/images/icons/textit.png", "italic"},
	{DATA_DIR "/images/icons/texttt.png", "typewriter"},
	{DATA_DIR "/images/icons/underline.png", "underline"},
	{DATA_DIR "/images/icons/justify-center.png", "justify_center"},
	{DATA_DIR "/images/icons/justify-left.png", "justify_left"},
	{DATA_DIR "/images/icons/justify-right.png", "justify_right"},
};

// all the actions (for the menu and the toolbar)
// {name, stock_id, label, accelerator, tooltip, callback}
// the names come from the XML file
static GtkActionEntry entries[] = {
	{"File", NULL, N_("File"), NULL, NULL, NULL},
	{"FileNew", GTK_STOCK_NEW, N_("New"), "<Control>N",
		N_("New file"), G_CALLBACK (cb_new)},
	{"FileOpen", GTK_STOCK_OPEN, N_("Open..."), "<Control>O",
		N_("Open a file"), G_CALLBACK (cb_open)},
	{"FileSave", GTK_STOCK_SAVE, N_("Save..."), "<Control>S",
		N_("Save the current file"), G_CALLBACK (cb_save)},
	{"FileSaveAs", GTK_STOCK_SAVE_AS, N_("Save as..."), "<Shift><Control>S",
		N_("Save the current file with a different name"), G_CALLBACK (cb_save_as)},
	{"FileClose", GTK_STOCK_CLOSE, N_("Close"), "<Control>W",
		N_("Close the current file"), G_CALLBACK (cb_close)},
	{"FileQuit", GTK_STOCK_QUIT, N_("Quit"), "<Control>Q",
		N_("Quit the program"), G_CALLBACK (cb_quit)},
	
	{"Edit", NULL, N_("Edit"), NULL, NULL, NULL},
	{"EditUndo", GTK_STOCK_UNDO, N_("Undo"), "<Control>Z",
		N_("Undo the last action"), G_CALLBACK (cb_undo)},
	{"EditRedo", GTK_STOCK_REDO, N_("Redo"), "<Shift><Control>Z",
		N_("Redo the last undone action"), G_CALLBACK (cb_redo)},
	{"EditCut", GTK_STOCK_CUT, N_("Cut"), "<Control>X",
		N_("Cut the selection"), G_CALLBACK (cb_cut)},
	{"EditCopy", GTK_STOCK_COPY, N_("Copy"), "<Control>C",
		N_("Copy the selection"), G_CALLBACK (cb_copy)},
	{"EditPaste", GTK_STOCK_PASTE, N_("Paste"), "<Control>V",
		N_("Paste the clipboard"), G_CALLBACK (cb_paste)},
	{"EditDelete", GTK_STOCK_DELETE, N_("Delete"), NULL,
		N_("Delete the selected text"), G_CALLBACK (cb_delete)},
	{"EditSelectAll", GTK_STOCK_SELECT_ALL, N_("Select All"), "<Control>A",
		N_("Select the entire document"), G_CALLBACK (cb_select_all)},
	{"EditPreferences", GTK_STOCK_PREFERENCES, N_("Preferences"), NULL,
		N_("Configure the application"), G_CALLBACK (cb_preferences)},

	{"View", NULL, N_("View"), NULL, NULL, NULL},
	{"ViewZoomIn", GTK_STOCK_ZOOM_IN, N_("Zoom In"), "<Control>plus",
		N_("Enlarge the font"), G_CALLBACK (cb_zoom_in)},
	{"ViewZoomOut", GTK_STOCK_ZOOM_OUT, N_("Zoom Out"), "<Control>minus",
		N_("Shrink the font"), G_CALLBACK (cb_zoom_out)},
	{"ViewZoomReset", GTK_STOCK_ZOOM_100, N_("Reset Zoom"), "<Control>0",
		N_("Reset the size of the font"), G_CALLBACK (cb_zoom_reset)},

	{"Search", NULL, N_("Search"), NULL, NULL, NULL},
	{"SearchFind", GTK_STOCK_FIND, N_("Find..."), "<Control>F",
		N_("Search for text"), G_CALLBACK (cb_find)},
	{"SearchReplace", GTK_STOCK_FIND_AND_REPLACE, N_("Replace..."), "<Control>H",
		N_("Search for and replace text"), G_CALLBACK (cb_replace)},
	
	{"Build", NULL, N_("Build"), NULL, NULL, NULL},
	{"compile_latex", "compile_latex", N_("Compile (latex)"), "<Release>F5",
		N_("Produce the document in DVI format"), G_CALLBACK (cb_latex)},
	{"viewDVI", "view_dvi", N_("View DVI"), "<Release>F6",
		N_("View the DVI file"), G_CALLBACK (cb_view_dvi)},
	{"compile_pdflatex", "compile_pdflatex", N_("Compile (pdflatex)"), "<Release>F7",
		N_("Produce the document in PDF format"), G_CALLBACK (cb_pdflatex)},
	{"viewPDF", "view_pdf", N_("View PDF"), "<Release>F8",
		N_("View the PDF file"), G_CALLBACK (cb_view_pdf)},
	{"DVItoPDF", "dvi_to_pdf", N_("DVI to PDF"), NULL,
		N_("Convert the DVI document to the PDF format"), G_CALLBACK (cb_dvi_to_pdf)},
	{"DVItoPS", "dvi_to_ps", N_("DVI to PS"), NULL,
		N_("Convert the DVI document to the PostScript format"), G_CALLBACK (cb_dvi_to_ps)},
	{"viewPS", "view_ps", N_("View PS"), NULL,
		N_("View the PostScript file"), G_CALLBACK (cb_view_ps)},
	
	{"Help", NULL, N_("Help"), NULL, NULL, NULL},
	{"HelpAbout", GTK_STOCK_ABOUT, N_("About"), NULL,
		N_("About LaTeXila"), G_CALLBACK (cb_about_dialog)}
};

// {name, stock_id, label, accelerator, tooltip, callback}
static GtkActionEntry latex_entries[] = {
	{"Latex", NULL, N_("LaTeX"), NULL, NULL, NULL},

	{"FontStyles", NULL, N_("Font Styles"), NULL, NULL, NULL},
	{"Bold", "bold", N_("Bold - \\textbf{}"), NULL,
		N_("Bold - \\textbf{}"), G_CALLBACK (cb_text_bold)},
	{"Italic", "italic", N_("Italic - \\textit{}"), NULL,
		N_("Italic - \\textit{}"), G_CALLBACK (cb_text_italic)},
	{"Typewriter", "typewriter", N_("Typewriter - \\texttt{}"), NULL,
		N_("Typewriter - \\texttt{}"), G_CALLBACK (cb_text_typewriter)},
	{"Underline", "underline", N_("Underline - \\underline{}"), NULL,
		N_("Underline - \\underline{}"), G_CALLBACK (cb_text_underline)},
	{"Slanted", NULL, N_("Slanted - \\textsl{}"), NULL,
		N_("Slanted - \\textsl{}"), G_CALLBACK (cb_text_slanted)},
	{"SmallCaps", NULL, N_("Small Capitals - \\textsc{}"), NULL,
		N_("Small Capitals - \\textsc{}"), G_CALLBACK (cb_text_small_caps)},
	{"Emph", NULL, N_("Emphasized - \\emph{}"), NULL,
		N_("Emphasized - \\emph{}"), G_CALLBACK (cb_text_emph)},
	{"Strong", NULL, N_("Strong - \\strong{}"), NULL,
		N_("Strong - \\strong{}"), G_CALLBACK (cb_text_strong)},

	{"FontFamily", NULL, N_("Font Family"), NULL, NULL, NULL},
	{"FontFamilyRoman", NULL, N_("Roman - \\rmfamily"), NULL,
		N_("Roman - \\rmfamily"), G_CALLBACK (cb_text_font_family_roman)},
	{"FontFamilySansSerif", NULL, N_("Sans Serif - \\sffamily"), NULL,
		N_("Sans Serif - \\sffamily"), G_CALLBACK (cb_text_font_family_sans_serif)},
	{"FontFamilyMonospace", NULL, N_("Monospace - \\ttfamily"), NULL,
		N_("Monospace - \\ttfamily"), G_CALLBACK (cb_text_font_family_monospace)},

	{"FontSeries", NULL, N_("Font Series"), NULL, NULL, NULL},
	{"FontSeriesMedium", NULL, N_("Medium - \\mdseries"), NULL,
		N_("Medium - \\mdseries"), G_CALLBACK (cb_text_font_series_medium)},
	{"FontSeriesBold", NULL, N_("Bold - \\bfseries"), NULL,
		N_("Bold - \\bfseries"), G_CALLBACK (cb_text_font_series_bold)},

	{"FontShape", NULL, N_("Font Shape"), NULL, NULL, NULL},
	{"FontShapeUpright", NULL, N_("Upright - \\upshape"), NULL,
		N_("Upright - \\upshape"), G_CALLBACK (cb_text_font_shape_upright)},
	{"FontShapeItalic", NULL, N_("Italic - \\itshape"), NULL,
		N_("Italic - \\itshape"), G_CALLBACK (cb_text_font_shape_italic)},
	{"FontShapeSlanted", NULL, N_("Slanted - \\slshape"), NULL,
		N_("Slanted - \\slshape"), G_CALLBACK (cb_text_font_shape_slanted)},
	{"FontShapeSmallCaps", NULL, N_("Small Capitals - \\scshape"), NULL,
		N_("Small Capitals - \\scshape"), G_CALLBACK (cb_text_font_shape_small_caps)},

	{"Environments", NULL, N_("Environments"), NULL, NULL, NULL},
	{"EnvironmentCenter", "justify_center", N_("Center - \\begin{center}"), NULL,
		N_("Center - \\begin{center}"), G_CALLBACK (cb_env_center)},
	{"EnvironmentLeft", "justify_left", N_("Align Left - \\begin{flushleft}"), NULL,
		N_("Align Left - \\begin{flushleft}"), G_CALLBACK (cb_env_left)},
	{"EnvironmentRight", "justify_right", N_("Align Right - \\begin{flushright}"), NULL,
		N_("Align Right - \\begin{flushright}"), G_CALLBACK (cb_env_right)},
	{"EnvironmentMinipage", NULL, N_("Minipage - \\begin{minipage}"), NULL,
		N_("Minipage - \\begin{minipage}"), G_CALLBACK (cb_env_minipage)},
	{"EnvironmentQuote", NULL, N_("Quote - \\begin{quote}"), NULL,
		N_("Quote - \\begin{quote}"), G_CALLBACK (cb_env_quote)},
	{"EnvironmentQuotation", NULL, N_("Quotation - \\begin{quotation}"), NULL,
		N_("Quotation - \\begin{quotation}"), G_CALLBACK (cb_env_quotation)},
	{"EnvironmentVerse", NULL, N_("Verse - \\begin{verse}"), NULL,
		N_("Verse - \\begin{verse}"), G_CALLBACK (cb_env_verse)}
};

// {name, stock_id, label, accelerator, tooltip, callback}
static GtkToggleActionEntry toggle_entries[] = {
	{"ViewSymbols", NULL, N_("Symbol tables"), NULL,
		N_("Show or hide the symbol tables in the current window"),
		G_CALLBACK (cb_show_symbol_tables)}
};

static guint n_stock_icons = G_N_ELEMENTS (stock_icons);
static guint nb_entries = G_N_ELEMENTS (entries);
static guint nb_latex_entries = G_N_ELEMENTS (latex_entries);
static guint nb_toggle_entries = G_N_ELEMENTS (toggle_entries);

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

void
init_ui (GtkWidget *box)
{
	GError *error = NULL;

	register_my_stock_icons ();

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
#ifdef LATEXILA_NLS_ENABLED
	gtk_action_group_set_translation_domain (action_group, LATEXILA_NLS_PACKAGE);
#endif
	gtk_action_group_add_actions (action_group, entries, nb_entries, NULL);
	gtk_action_group_add_actions (action_group, latex_entries, nb_latex_entries,
			NULL);
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
	// toolbars with icons only
	GtkWidget *menubar = gtk_ui_manager_get_widget (ui_manager, "/MainMenu");
	gtk_box_pack_start (GTK_BOX (box), menubar, FALSE, FALSE, 0);

	GtkWidget *toolbar = gtk_ui_manager_get_widget (ui_manager, "/MainToolbar");
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_ICONS);
	gtk_box_pack_start (GTK_BOX (box), toolbar, FALSE, FALSE, 0);

	GtkWidget *edit_toolbar = gtk_ui_manager_get_widget (ui_manager,
			"/EditToolbar");
	gtk_toolbar_set_style (GTK_TOOLBAR (edit_toolbar), GTK_TOOLBAR_ICONS);
	gtk_box_pack_start (GTK_BOX (box), edit_toolbar, FALSE, FALSE, 0);

	// accelerators
	gtk_window_add_accel_group (latexila.main_window, 
			gtk_ui_manager_get_accel_group (ui_manager));

	// get actions
	latexila.undo = gtk_action_group_get_action (action_group, "EditUndo");
	latexila.redo = gtk_action_group_get_action (action_group, "EditRedo");
	GtkToggleAction *show_symbol_tables = GTK_TOGGLE_ACTION (
			gtk_action_group_get_action (action_group, "ViewSymbols"));
	gtk_toggle_action_set_active (show_symbol_tables,
			latexila.prefs->show_side_pane);
}
