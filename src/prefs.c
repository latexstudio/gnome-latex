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
#include <string.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcestylescheme.h>
#include <gtksourceview/gtksourcestyleschememanager.h>
#include <sys/stat.h> // for S_IRWXU

#include "main.h"
#include "prefs.h"
#include "config.h"
#include "print.h"
#include "callbacks.h"
#include "file_browser.h"

static void load_default_preferences (preferences_t *prefs);
static gchar * get_rc_file (void);
static void create_preferences (void);
static void cb_pref_dialog_close (GtkDialog *dialog, gint response_id,
		gpointer user_data);
static void cb_pref_line_numbers (GtkToggleButton *toggle_button,
		gpointer user_data);
static void cb_pref_tab_width (GtkSpinButton *spin_button, gpointer user_data);
static void cb_pref_spaces_instead_of_tabs (GtkToggleButton *toggle_button,
		gpointer user_data);
static void cb_pref_highlight_current_line (GtkToggleButton *toggle_button,
		gpointer user_data);
static void cb_pref_highlight_matching_brackets (GtkToggleButton *toggle_button,
		gpointer user_data);
static void cb_pref_font_set (GtkFontButton *font_button, gpointer user_data);
static void cb_pref_command_view (GtkEditable *editable, gpointer user_data);
static void cb_pref_command_latex (GtkEditable *editable, gpointer user_data);
static void cb_pref_command_pdflatex (GtkEditable *editable, gpointer user_data);
static void cb_pref_command_dvipdf (GtkEditable *editable, gpointer user_data);
static void cb_pref_command_dvips (GtkEditable *editable, gpointer user_data);
static void cb_pref_web_browser (GtkEditable *editable, gpointer user_data);
static void cb_pref_command_bibtex (GtkEditable *editable, gpointer user_data);
static void cb_pref_command_makeindex (GtkEditable *editable, gpointer user_data);
static void cb_pref_compile_show_all_output (GtkToggleButton *toggle_button,
		gpointer user_data);
static void cb_pref_compile_non_stop (GtkToggleButton *toggle_button,
		gpointer user_data);
static void cb_style_scheme_changed (GtkTreeSelection *selection,
		gpointer user_data);
static void cb_delete_aux_files (GtkToggleButton *toggle_button,
		gpointer user_data);
static void cb_toolbars_horizontal (GtkToggleButton *toggle_button,
		gpointer user_data);
static gint style_schemes_compare (gconstpointer a, gconstpointer b);
GSList * get_list_style_schemes_sorted (void);
static void fill_style_schemes_list_store (GtkListStore *store,
		GtkTreeSelection *selection);

/* default values */
// there is an underscore in the end for each variable name
static gboolean	show_line_numbers_				= FALSE;
static gboolean	show_side_pane_					= TRUE;
static gboolean show_edit_toolbar_				= TRUE;
static gint		window_width_					= 950;
static gint		window_height_					= 660;
static gboolean	window_maximised_				= FALSE;
static gint		main_hpaned_pos_				= 256;
static gint		vpaned_pos_						= 375;
static gint		log_hpaned_pos_					= 180;
static gchar	*font_							= "Monospace 10";
static gchar	*command_view_					= "gnome-open";
static gchar	*command_latex_					= COMMAND_LATEX;
static gchar	*command_pdflatex_				= COMMAND_PDFLATEX;
static gchar	*command_dvipdf_				= COMMAND_DVIPDF;
static gchar	*command_dvips_					= COMMAND_DVIPS;
static gchar	*command_web_browser_			= "gnome-open";
static gchar	*command_bibtex_				= COMMAND_BIBTEX;
static gchar	*command_makeindex_				= COMMAND_MAKEINDEX;
static gboolean	compile_show_all_output_		= FALSE;
static gboolean compile_non_stop_				= FALSE;
static gboolean delete_aux_files_				= FALSE;
static gboolean reopen_files_on_startup_		= TRUE;
static gboolean file_browser_show_all_files_	= FALSE;
static gchar	*style_scheme_id_				= "classic";
static gint		tab_width_						= 2;
static gboolean	spaces_instead_of_tabs_			= TRUE;
static gboolean	highlight_current_line_			= TRUE;
static gboolean highlight_matching_brackets_	= TRUE;
static gboolean	toolbars_horizontal_			= FALSE;
static gint		side_pane_page_					= 0;

void
load_preferences (preferences_t *prefs)
{
	gchar *rc_file = get_rc_file ();
	if (! g_file_test (rc_file, G_FILE_TEST_EXISTS))
	{
		g_free (rc_file);
		load_default_preferences (prefs);
		return;
	}

	GKeyFile *key_file = g_key_file_new ();
	GError *error = NULL;
	g_key_file_load_from_file (key_file, rc_file, G_KEY_FILE_NONE, &error);
	g_free (rc_file);

	if (error != NULL)
	{
		print_warning ("load user preferences failed: %s", error->message);
		g_error_free (error);
		load_default_preferences (prefs);
		return;
	}

	/* check if all keys exist
	 * if not, set the default value for that key
	 */
	prefs->show_line_numbers = g_key_file_get_boolean (key_file, PROGRAM_NAME,
			"show_line_numbers", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->show_line_numbers = show_line_numbers_;
		g_error_free (error);
		error = NULL;
	}

	prefs->show_side_pane = g_key_file_get_boolean (key_file, PROGRAM_NAME,
			"show_side_pane", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->show_side_pane = show_side_pane_;
		g_error_free (error);
		error = NULL;
	}

	prefs->show_edit_toolbar = g_key_file_get_boolean (key_file, PROGRAM_NAME,
			"show_edit_toolbar", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->show_edit_toolbar = show_edit_toolbar_;
		g_error_free (error);
		error = NULL;
	}

	prefs->window_width = g_key_file_get_integer (key_file, PROGRAM_NAME,
			"window_width", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->window_width = window_width_;
		g_error_free (error);
		error = NULL;
	}

	prefs->window_height = g_key_file_get_integer (key_file, PROGRAM_NAME,
			"window_height", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->window_height = window_height_;
		g_error_free (error);
		error = NULL;
	}

	prefs->window_maximised = g_key_file_get_boolean (key_file, PROGRAM_NAME,
			"window_maximised", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->window_maximised = window_maximised_;
		g_error_free (error);
		error = NULL;
	}

	// where is the end?
	
	prefs->main_hpaned_pos = g_key_file_get_integer (key_file, PROGRAM_NAME,
			"main_hpaned_pos", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->main_hpaned_pos = main_hpaned_pos_;
		g_error_free (error);
		error = NULL;
	}

	prefs->vpaned_pos = g_key_file_get_integer (key_file, PROGRAM_NAME,
			"vpaned_pos", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->vpaned_pos = vpaned_pos_;
		g_error_free (error);
		error = NULL;
	}

	prefs->log_hpaned_pos = g_key_file_get_integer (key_file, PROGRAM_NAME,
			"log_hpaned_pos", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->log_hpaned_pos = log_hpaned_pos_;
		g_error_free (error);
		error = NULL;
	}

	gchar *font = g_key_file_get_string (key_file, PROGRAM_NAME, "font", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->font_str = g_strdup (font_);
		set_current_font_prefs (prefs);
		g_error_free (error);
		error = NULL;
	}
	else
	{
		prefs->font_str = g_strdup (font);
		set_current_font_prefs (prefs);
		g_free (font);
	}

	prefs->command_view = g_key_file_get_string (key_file, PROGRAM_NAME,
			"command_view", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->command_view = g_strdup (command_view_);
		g_error_free (error);
		error = NULL;
	}

	prefs->command_latex = g_key_file_get_string (key_file, PROGRAM_NAME,
			"command_latex", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->command_latex = g_strdup (command_latex_);
		g_error_free (error);
		error = NULL;
	}

	prefs->command_pdflatex = g_key_file_get_string (key_file, PROGRAM_NAME,
			"command_pdflatex", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->command_pdflatex = g_strdup (command_pdflatex_);
		g_error_free (error);
		error = NULL;
	}

	prefs->command_dvipdf = g_key_file_get_string (key_file, PROGRAM_NAME,
			"command_dvipdf", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->command_dvipdf = g_strdup (command_dvipdf_);
		g_error_free (error);
		error = NULL;
	}

	prefs->command_dvips = g_key_file_get_string (key_file, PROGRAM_NAME,
			"command_dvips", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->command_dvips = g_strdup (command_dvips_);
		g_error_free (error);
		error = NULL;
	}

	prefs->command_web_browser = g_key_file_get_string (key_file, PROGRAM_NAME,
			"command_web_browser", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->command_web_browser = g_strdup (command_web_browser_);
		g_error_free (error);
		error = NULL;
	}

	prefs->command_bibtex = g_key_file_get_string (key_file, PROGRAM_NAME,
			"command_bibtex", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->command_bibtex = g_strdup (command_bibtex_);
		g_error_free (error);
		error = NULL;
	}

	prefs->command_makeindex = g_key_file_get_string (key_file, PROGRAM_NAME,
			"command_makeindex", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->command_makeindex = g_strdup (command_makeindex_);
		g_error_free (error);
		error = NULL;
	}

	prefs->compile_show_all_output = g_key_file_get_boolean (key_file, PROGRAM_NAME,
			"compile_show_all_output", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->compile_show_all_output = compile_show_all_output_;
		g_error_free (error);
		error = NULL;
	}

	prefs->compile_non_stop = g_key_file_get_boolean (key_file, PROGRAM_NAME,
			"compile_non_stop", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->compile_non_stop = compile_non_stop_;
		g_error_free (error);
		error = NULL;
	}

	prefs->file_chooser_dir = g_key_file_get_string (key_file, PROGRAM_NAME,
			"file_chooser_directory", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->file_chooser_dir = NULL;
		g_error_free (error);
		error = NULL;
	}

	prefs->file_browser_dir = g_key_file_get_string (key_file, PROGRAM_NAME,
			"file_browser_directory", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->file_browser_dir = g_strdup (g_get_home_dir ());
		g_error_free (error);
		error = NULL;
	}

	gchar **list_opened_docs = g_key_file_get_string_list (key_file, PROGRAM_NAME,
			"list_opened_documents", NULL, &error);
	prefs->list_opened_docs = g_ptr_array_new ();
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		g_error_free (error);
		error = NULL;
	}
	else
	{
		gchar **current = list_opened_docs;
		while (*current != NULL)
		{
			// the string must be freed later
			g_ptr_array_add (prefs->list_opened_docs,
					(gpointer) g_strdup (*current));
			current++;
		}

		g_strfreev (list_opened_docs);
	}

	prefs->reopen_files_on_startup = g_key_file_get_boolean (key_file,
			PROGRAM_NAME, "reopen_files_on_startup", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->reopen_files_on_startup = reopen_files_on_startup_;
		g_error_free (error);
		error = NULL;
	}

	prefs->file_browser_show_all_files = g_key_file_get_boolean (key_file,
			PROGRAM_NAME, "file_browser_show_all_files", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->file_browser_show_all_files = file_browser_show_all_files_;
		g_error_free (error);
		error = NULL;
	}

	prefs->delete_aux_files = g_key_file_get_boolean (key_file,
			PROGRAM_NAME, "delete_auxiliaries_files", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->delete_aux_files = delete_aux_files_;
		g_error_free (error);
		error = NULL;
	}

	prefs->style_scheme_id = g_key_file_get_string (key_file,
			PROGRAM_NAME, "style_scheme_id", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->style_scheme_id = g_strdup (style_scheme_id_);
		g_error_free (error);
		error = NULL;
	}

	prefs->tab_width = g_key_file_get_integer (key_file,
			PROGRAM_NAME, "tab_width", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->tab_width = tab_width_;
		g_error_free (error);
		error = NULL;
	}

	// look, I see light, we are close to the exit!
	
	prefs->spaces_instead_of_tabs = g_key_file_get_boolean (key_file,
			PROGRAM_NAME, "spaces_instead_of_tabs", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->spaces_instead_of_tabs = spaces_instead_of_tabs_;
		g_error_free (error);
		error = NULL;
	}

	prefs->highlight_current_line = g_key_file_get_boolean (key_file,
			PROGRAM_NAME, "highlight_current_line", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->highlight_current_line = highlight_current_line_;
		g_error_free (error);
		error = NULL;
	}

	prefs->highlight_matching_brackets = g_key_file_get_boolean (key_file,
			PROGRAM_NAME, "highlight_matching_brackets", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->highlight_matching_brackets = highlight_matching_brackets_;
		g_error_free (error);
		error = NULL;
	}

	prefs->toolbars_horizontal = g_key_file_get_boolean (key_file,
			PROGRAM_NAME, "toolbars_horizontal", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->toolbars_horizontal = toolbars_horizontal_;
		g_error_free (error);
		error = NULL;
	}

	prefs->side_pane_page = g_key_file_get_integer (key_file,
			PROGRAM_NAME, "side_pane_page", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->side_pane_page = side_pane_page_;
		g_error_free (error);
		error = NULL;
	}

	g_key_file_free (key_file);
}

void
save_preferences (preferences_t *prefs)
{
	GKeyFile *key_file = g_key_file_new ();

	/* set the keys that we can take directly from the prefs struct */
	g_key_file_set_boolean (key_file, PROGRAM_NAME, "show_line_numbers",
			prefs->show_line_numbers);
	g_key_file_set_boolean (key_file, PROGRAM_NAME, "show_side_pane",
			prefs->show_side_pane);
	g_key_file_set_boolean (key_file, PROGRAM_NAME, "show_edit_toolbar",
			prefs->show_edit_toolbar);
	g_key_file_set_string (key_file, PROGRAM_NAME, "font", prefs->font_str);
	g_key_file_set_string (key_file, PROGRAM_NAME, "command_view",
			prefs->command_view);
	g_key_file_set_string (key_file, PROGRAM_NAME, "command_latex",
			prefs->command_latex);
	g_key_file_set_string (key_file, PROGRAM_NAME, "command_pdflatex",
			prefs->command_pdflatex);
	g_key_file_set_string (key_file, PROGRAM_NAME, "command_dvipdf",
			prefs->command_dvipdf);
	g_key_file_set_string (key_file, PROGRAM_NAME, "command_dvips",
			prefs->command_dvips);
	g_key_file_set_string (key_file, PROGRAM_NAME, "command_web_browser",
			prefs->command_web_browser);
	g_key_file_set_string (key_file, PROGRAM_NAME, "command_bibtex",
			prefs->command_bibtex);
	g_key_file_set_string (key_file, PROGRAM_NAME, "command_makeindex",
			prefs->command_makeindex);
	g_key_file_set_boolean (key_file, PROGRAM_NAME, "compile_show_all_output",
			prefs->compile_show_all_output);
	g_key_file_set_boolean (key_file, PROGRAM_NAME, "compile_non_stop",
			prefs->compile_non_stop);
	if (prefs->file_chooser_dir != NULL)
		g_key_file_set_string (key_file, PROGRAM_NAME, "file_chooser_directory",
				prefs->file_chooser_dir);
	g_key_file_set_string (key_file, PROGRAM_NAME, "file_browser_directory",
			prefs->file_browser_dir);
	g_key_file_set_string_list (key_file, PROGRAM_NAME, "list_opened_documents",
			(const gchar **) prefs->list_opened_docs->pdata,
			prefs->list_opened_docs->len);
	g_key_file_set_boolean (key_file, PROGRAM_NAME, "reopen_files_on_startup",
			prefs->reopen_files_on_startup);
	g_key_file_set_boolean (key_file, PROGRAM_NAME, "file_browser_show_all_files",
			prefs->file_browser_show_all_files);
	g_key_file_set_boolean (key_file, PROGRAM_NAME, "delete_auxiliaries_files",
			prefs->delete_aux_files);
	g_key_file_set_string (key_file, PROGRAM_NAME, "style_scheme_id",
			prefs->style_scheme_id);
	g_key_file_set_integer (key_file, PROGRAM_NAME, "tab_width",
			prefs->tab_width);
	g_key_file_set_boolean (key_file, PROGRAM_NAME, "spaces_instead_of_tabs",
			prefs->spaces_instead_of_tabs);
	g_key_file_set_boolean (key_file, PROGRAM_NAME, "highlight_current_line",
			prefs->highlight_current_line);
	g_key_file_set_boolean (key_file, PROGRAM_NAME, "highlight_matching_brackets",
			prefs->highlight_matching_brackets);
	g_key_file_set_boolean (key_file, PROGRAM_NAME, "toolbars_horizontal",
			prefs->toolbars_horizontal);

	/* set the keys that must be taken from the widgets */
	GdkWindowState flag = gdk_window_get_state (gtk_widget_get_window (
				GTK_WIDGET (latexila.main_window)));
	gboolean window_maximised = flag & GDK_WINDOW_STATE_MAXIMIZED;
	g_key_file_set_boolean (key_file, PROGRAM_NAME, "window_maximised",
			window_maximised);

	gint window_width, window_height;
	gtk_window_get_size (latexila.main_window, &window_width, &window_height);

	// If window is maximized, store sizes that are a bit smaller than full
	// screen, else making window non-maximized the next time will have no
	// effect.
	// Attention, the left/top widgets must be packed in paned with
	// gtk_paned_add1 () and not "gtk_paned_pack1 (paned, widget, TRUE, TRUE)",
	// else the positions panes will inexplicably increase...
	if (window_maximised)
	{
		window_width -= 100;
		window_height -= 100;
	}

	g_key_file_set_integer (key_file, PROGRAM_NAME, "window_width",
			window_width);
	g_key_file_set_integer (key_file, PROGRAM_NAME, "window_height",
			window_height);

	gint main_hpaned_pos = gtk_paned_get_position (latexila.main_hpaned);
	gint vpaned_pos = gtk_paned_get_position (latexila.vpaned);
	gint log_hpaned_pos = gtk_paned_get_position (latexila.log_hpaned);
	g_key_file_set_integer (key_file, PROGRAM_NAME, "main_hpaned_pos",
			main_hpaned_pos);
	g_key_file_set_integer (key_file, PROGRAM_NAME, "vpaned_pos",
			vpaned_pos);
	g_key_file_set_integer (key_file, PROGRAM_NAME, "log_hpaned_pos",
			log_hpaned_pos);

	gint side_pane_page = gtk_notebook_get_current_page (
			GTK_NOTEBOOK (latexila.side_pane));
	g_key_file_set_integer (key_file, PROGRAM_NAME, "side_pane_page",
			side_pane_page);

	/* save the rc file */
	gchar *rc_file = get_rc_file ();
	gchar *rc_path = g_path_get_dirname (rc_file);
	g_mkdir_with_parents(rc_path, S_IRWXU);
	gchar *key_file_data = g_key_file_to_data (key_file, NULL, NULL);

	GError *error = NULL;
	g_file_set_contents (rc_file, key_file_data, -1, &error);
	
	if (error != NULL)
	{
		print_warning ("impossible to save preferences: %s", error->message);
		g_error_free (error);
		error = NULL;
	}

	g_free (rc_file);
	g_free (rc_path);
	g_free (key_file_data);
	g_key_file_free (key_file);
}

void
set_current_font_prefs (preferences_t *prefs)
{
	g_return_if_fail (prefs->font_str != NULL);

	prefs->font_desc = pango_font_description_from_string (prefs->font_str);
	prefs->font_size = pango_font_description_get_size (prefs->font_desc);
}

static void
load_default_preferences (preferences_t *prefs)
{
	prefs->show_line_numbers = show_line_numbers_;
	prefs->show_side_pane = show_side_pane_;
	prefs->show_edit_toolbar = show_edit_toolbar_;
	prefs->window_width = window_width_;
	prefs->window_height = window_height_;
	prefs->window_maximised = window_maximised_;
	prefs->main_hpaned_pos = main_hpaned_pos_;
	prefs->vpaned_pos = vpaned_pos_;
	prefs->log_hpaned_pos = log_hpaned_pos_;
	prefs->font_str = g_strdup (font_);
	prefs->command_view = g_strdup (command_view_);
	prefs->command_latex = g_strdup (command_latex_);
	prefs->command_pdflatex = g_strdup (command_pdflatex_);
	prefs->command_dvipdf = g_strdup (command_dvipdf_);
	prefs->command_dvips = g_strdup (command_dvips_);
	prefs->command_web_browser = g_strdup (command_web_browser_);
	prefs->command_bibtex = g_strdup (command_bibtex_);
	prefs->command_makeindex = g_strdup (command_makeindex_);
	prefs->compile_show_all_output = compile_show_all_output_;
	prefs->compile_non_stop = compile_non_stop_;
	prefs->file_chooser_dir = NULL;
	prefs->file_browser_dir = g_strdup (g_get_home_dir ());
	prefs->list_opened_docs = g_ptr_array_new ();
	prefs->reopen_files_on_startup = reopen_files_on_startup_;
	prefs->file_browser_show_all_files = file_browser_show_all_files_;
	prefs->delete_aux_files = delete_aux_files_;
	prefs->style_scheme_id = g_strdup (style_scheme_id_);
	prefs->tab_width = tab_width_;
	prefs->spaces_instead_of_tabs = spaces_instead_of_tabs_;
	prefs->highlight_current_line = highlight_current_line_;
	prefs->highlight_matching_brackets = highlight_matching_brackets_;
	prefs->toolbars_horizontal = toolbars_horizontal_;
	prefs->side_pane_page = side_pane_page_;

	set_current_font_prefs (prefs);
}

static gchar *
get_rc_file (void)
{
	// rc_file must be freed
	gchar *rc_file = g_build_filename (g_get_user_config_dir (), "latexila",
			"latexilarc", NULL);
	return rc_file;
}


/******************************************
 * Menu: Edit -> Preferences
 *****************************************/

static GtkWidget *pref_dialog = NULL;

void
cb_preferences (void)
{
	if (pref_dialog == NULL)
		create_preferences ();
	gtk_window_present (GTK_WINDOW (pref_dialog));
}

static void
cb_pref_dialog_close (GtkDialog *dialog, gint response_id, gpointer user_data)
{
	gtk_widget_hide (GTK_WIDGET (dialog));
}

static void
cb_pref_line_numbers (GtkToggleButton *toggle_button, gpointer user_data)
{
	gboolean show_line_numbers = gtk_toggle_button_get_active (toggle_button);
	latexila.prefs.show_line_numbers = show_line_numbers;

	// traverse the list
	GList *current = latexila.all_docs;
	while (current != NULL)
	{
		document_t *doc = current->data;
		gtk_source_view_set_show_line_numbers (
				GTK_SOURCE_VIEW (doc->source_view), show_line_numbers);
		current = g_list_next (current);
	}
}

static void
cb_pref_tab_width (GtkSpinButton *spin_button, gpointer user_data)
{
	gint value = gtk_spin_button_get_value_as_int (spin_button);
	latexila.prefs.tab_width = value;

	// traverse the list
	GList *current = latexila.all_docs;
	while (current != NULL)
	{
		document_t *doc = current->data;
		gtk_source_view_set_tab_width (GTK_SOURCE_VIEW (doc->source_view), value);
		current = g_list_next (current);
	}
}

static void
cb_pref_spaces_instead_of_tabs (GtkToggleButton *toggle_button,
		gpointer user_data)
{
	gboolean tmp = gtk_toggle_button_get_active (toggle_button);
	latexila.prefs.spaces_instead_of_tabs = tmp;

	// traverse the list
	GList *current = latexila.all_docs;
	while (current != NULL)
	{
		document_t *doc = current->data;
		gtk_source_view_set_insert_spaces_instead_of_tabs (
				GTK_SOURCE_VIEW (doc->source_view), tmp);
		current = g_list_next (current);
	}
}

static void
cb_pref_highlight_current_line (GtkToggleButton *toggle_button,
		gpointer user_data)
{
	gboolean tmp = gtk_toggle_button_get_active (toggle_button);
	latexila.prefs.highlight_current_line = tmp;

	// traverse the list
	GList *current = latexila.all_docs;
	while (current != NULL)
	{
		document_t *doc = current->data;
		gtk_source_view_set_highlight_current_line (
				GTK_SOURCE_VIEW (doc->source_view), tmp);
		current = g_list_next (current);
	}
}

static void
cb_pref_highlight_matching_brackets (GtkToggleButton *toggle_button,
		gpointer user_data)
{
	gboolean tmp = gtk_toggle_button_get_active (toggle_button);
	latexila.prefs.highlight_matching_brackets = tmp;

	// traverse the list
	GList *current = latexila.all_docs;
	while (current != NULL)
	{
		document_t *doc = current->data;
		gtk_source_buffer_set_highlight_matching_brackets (doc->source_buffer, tmp);
		current = g_list_next (current);
	}
}

static void
cb_pref_font_set (GtkFontButton *font_button, gpointer user_data)
{
	const gchar *font_string = gtk_font_button_get_font_name (font_button);
	g_free (latexila.prefs.font_str);
	latexila.prefs.font_str = g_strdup (font_string);
	set_current_font_prefs (&latexila.prefs);
	change_font_source_view ();
}

static void
cb_pref_command_view (GtkEditable *editable, gpointer user_data)
{
	GtkEntry *entry = GTK_ENTRY (editable);
	const gchar *new_command_view = gtk_entry_get_text (entry);
	g_free (latexila.prefs.command_view);
	latexila.prefs.command_view = g_strdup (new_command_view);
}

static void
cb_pref_command_latex (GtkEditable *editable, gpointer user_data)
{
	GtkEntry *entry = GTK_ENTRY (editable);
	const gchar *new_command = gtk_entry_get_text (entry);
	g_free (latexila.prefs.command_latex);
	latexila.prefs.command_latex = g_strdup (new_command);
}

static void
cb_pref_command_pdflatex (GtkEditable *editable, gpointer user_data)
{
	GtkEntry *entry = GTK_ENTRY (editable);
	const gchar *new_command = gtk_entry_get_text (entry);
	g_free (latexila.prefs.command_pdflatex);
	latexila.prefs.command_pdflatex = g_strdup (new_command);
}

static void
cb_pref_command_dvipdf (GtkEditable *editable, gpointer user_data)
{
	GtkEntry *entry = GTK_ENTRY (editable);
	const gchar *new_command = gtk_entry_get_text (entry);
	g_free (latexila.prefs.command_dvipdf);
	latexila.prefs.command_dvipdf = g_strdup (new_command);
}

static void
cb_pref_command_dvips (GtkEditable *editable, gpointer user_data)
{
	GtkEntry *entry = GTK_ENTRY (editable);
	const gchar *new_command = gtk_entry_get_text (entry);
	g_free (latexila.prefs.command_dvips);
	latexila.prefs.command_dvips = g_strdup (new_command);
}

static void
cb_pref_web_browser (GtkEditable *editable, gpointer user_data)
{
	GtkEntry *entry = GTK_ENTRY (editable);
	const gchar *new_command = gtk_entry_get_text (entry);
	g_free (latexila.prefs.command_web_browser);
	latexila.prefs.command_web_browser = g_strdup (new_command);
}

static void
cb_pref_command_bibtex (GtkEditable *editable, gpointer user_data)
{
	GtkEntry *entry = GTK_ENTRY (editable);
	const gchar *new_command = gtk_entry_get_text (entry);
	g_free (latexila.prefs.command_bibtex);
	latexila.prefs.command_bibtex = g_strdup (new_command);
}

static void
cb_pref_command_makeindex (GtkEditable *editable, gpointer user_data)
{
	GtkEntry *entry = GTK_ENTRY (editable);
	const gchar *new_command = gtk_entry_get_text (entry);
	g_free (latexila.prefs.command_makeindex);
	latexila.prefs.command_makeindex = g_strdup (new_command);
}

static void
cb_pref_compile_show_all_output (GtkToggleButton *toggle_button,
		gpointer user_data)
{
	latexila.prefs.compile_show_all_output =
		gtk_toggle_button_get_active (toggle_button);
}

static void
cb_pref_compile_non_stop (GtkToggleButton *toggle_button, gpointer user_data)
{
	latexila.prefs.compile_non_stop =
		gtk_toggle_button_get_active (toggle_button);
}

static void
cb_style_scheme_changed (GtkTreeSelection *selection, gpointer user_data)
{
	GtkTreeIter iter;
	GtkTreeModel *model;

	if (gtk_tree_selection_get_selected (selection, &model, &iter))
	{
		gchar *id;
		gtk_tree_model_get (model, &iter, COLUMN_STYLE_SCHEME_ID, &id, -1);
		g_free (latexila.prefs.style_scheme_id);
		latexila.prefs.style_scheme_id = id;

		GtkSourceStyleSchemeManager *style_scheme_manager =
			gtk_source_style_scheme_manager_get_default ();
		GtkSourceStyleScheme *style_scheme =
			gtk_source_style_scheme_manager_get_scheme (style_scheme_manager, id);

		// set the style scheme for all opened documents
		for (GList *current = latexila.all_docs ; current != NULL ;
				current = g_list_next (current))
		{
			document_t *doc = current->data;
			gtk_source_buffer_set_style_scheme (doc->source_buffer, style_scheme);
		}
	}
}

static void
cb_reopen_files_on_startup (GtkToggleButton *toggle_button, gpointer user_data)
{
	latexila.prefs.reopen_files_on_startup =
		gtk_toggle_button_get_active (toggle_button);
}

static void
cb_file_browser_show_all_files (GtkToggleButton *toggle_button,
		gpointer user_data)
{
	latexila.prefs.file_browser_show_all_files =
		gtk_toggle_button_get_active (toggle_button);
	cb_file_browser_refresh (NULL, NULL);
}

static void
cb_delete_aux_files (GtkToggleButton *toggle_button, gpointer user_data)
{
	latexila.prefs.delete_aux_files =
		gtk_toggle_button_get_active (toggle_button);
}

static void
cb_toolbars_horizontal (GtkToggleButton *toggle_button, gpointer user_data)
{
	latexila.prefs.toolbars_horizontal =
		gtk_toggle_button_get_active (toggle_button);
}

static gint
style_schemes_compare (gconstpointer a, gconstpointer b)
{
	GtkSourceStyleScheme *scheme_a = (GtkSourceStyleScheme *) a;
	GtkSourceStyleScheme *scheme_b = (GtkSourceStyleScheme *) b;

	const gchar *name_a = gtk_source_style_scheme_get_name (scheme_a);
	const gchar *name_b = gtk_source_style_scheme_get_name (scheme_b);

	return g_utf8_collate (name_a, name_b);
}

GSList *
get_list_style_schemes_sorted (void)
{
	GtkSourceStyleSchemeManager *manager =
		gtk_source_style_scheme_manager_get_default ();
	const gchar * const * ids =
		gtk_source_style_scheme_manager_get_scheme_ids (manager);

	GSList *list = NULL;

	while (*ids != NULL)
	{
		GtkSourceStyleScheme *style_scheme =
			gtk_source_style_scheme_manager_get_scheme (manager, *ids);
		list = g_slist_prepend (list, style_scheme);
		++ids;
	}

	if (list != NULL)
		list = g_slist_sort (list, (GCompareFunc) style_schemes_compare);

	return list;
}

static void
fill_style_schemes_list_store (GtkListStore *store, GtkTreeSelection *selection)
{
	GtkTreeIter iter;
	GSList *list = get_list_style_schemes_sorted ();

	while (list != NULL)
	{
		GtkSourceStyleScheme *style_scheme = list->data;

		const gchar *id = gtk_source_style_scheme_get_id (style_scheme);
		gchar *desc = g_strdup_printf ("%s (%s)",
				gtk_source_style_scheme_get_name (style_scheme),
				gtk_source_style_scheme_get_description (style_scheme));

		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter,
			COLUMN_STYLE_SCHEME_ID, id,
			COLUMN_STYLE_SCHEME_DESC, desc,
			-1);

		// select the style scheme selected
		if (strcmp (id, latexila.prefs.style_scheme_id) == 0)
			gtk_tree_selection_select_iter (selection, &iter);

		list = g_slist_next (list);
	}
}

static void
create_preferences (void)
{
	pref_dialog = gtk_dialog_new_with_buttons (_("Preferences"),
			latexila.main_window, 0,
			GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE, NULL);
	gtk_dialog_set_has_separator (GTK_DIALOG (pref_dialog), FALSE);
	gtk_container_set_border_width (GTK_CONTAINER (pref_dialog), 5);

	g_signal_connect (G_OBJECT (pref_dialog), "response",
			G_CALLBACK (cb_pref_dialog_close), NULL);
	g_signal_connect (G_OBJECT (pref_dialog), "destroy",
			G_CALLBACK (gtk_widget_destroyed), &pref_dialog);

	GtkWidget *content_area = gtk_dialog_get_content_area (
			GTK_DIALOG (pref_dialog));

	/* notebook */
	GtkWidget *notebook = gtk_notebook_new ();
	gtk_box_pack_start (GTK_BOX (content_area), notebook, TRUE, TRUE, 0);

	GtkWidget *vbox_editor = gtk_vbox_new (FALSE, 7);
	gtk_container_set_border_width (GTK_CONTAINER (vbox_editor), 4);
	GtkWidget *label_editor = gtk_label_new (_("Editor"));
	gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_editor, label_editor);

	GtkWidget *vbox_font_and_colors = gtk_vbox_new (FALSE, 10);
	gtk_container_set_border_width (GTK_CONTAINER (vbox_font_and_colors), 4);
	GtkWidget *label_font_and_colors = gtk_label_new (_("Font & Colors"));
	gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_font_and_colors,
			label_font_and_colors);

	GtkWidget *vbox_latex = gtk_vbox_new (FALSE, 7);
	gtk_container_set_border_width (GTK_CONTAINER (vbox_latex), 4);
	GtkWidget *label_latex = gtk_label_new ("LaTeX");
	gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_latex, label_latex);

	GtkWidget *vbox_other = gtk_vbox_new (FALSE, 7);
	gtk_container_set_border_width (GTK_CONTAINER (vbox_other), 4);
	GtkWidget *label_other = gtk_label_new (_("Other"));
	gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_other, label_other);

	/* show line numbers */
	{
		GtkWidget *line_numbers = gtk_check_button_new_with_label (
				_("Display line numbers"));
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (line_numbers),
				latexila.prefs.show_line_numbers);
		g_signal_connect (G_OBJECT (line_numbers), "toggled",
				G_CALLBACK (cb_pref_line_numbers), NULL);
		gtk_box_pack_start (GTK_BOX (vbox_editor), line_numbers, FALSE, FALSE, 0);
	}

	/* tab width */
	{
		GtkWidget *hbox = gtk_hbox_new (FALSE, 5);
		GtkWidget *label = gtk_label_new (_("Tab width:"));
		GtkAdjustment *spinner_adj = (GtkAdjustment *) gtk_adjustment_new (
				(gdouble) latexila.prefs.tab_width, 1.0, 24.0, 1.0, 0.0, 0.0);
		GtkWidget *tab_width = gtk_spin_button_new (spinner_adj, 1.0, 0);
		g_signal_connect (G_OBJECT (tab_width), "value-changed",
				G_CALLBACK (cb_pref_tab_width), NULL);
		gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (hbox), tab_width, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox_editor), hbox, FALSE, FALSE, 0);
	}
	
	/* spaces instead of tabs */
	{
		GtkWidget *spaces_instead_of_tabs = gtk_check_button_new_with_label (
				_("Insert spaces instead of tabs"));
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (spaces_instead_of_tabs),
				latexila.prefs.spaces_instead_of_tabs);
		g_signal_connect (G_OBJECT (spaces_instead_of_tabs), "toggled",
				G_CALLBACK (cb_pref_spaces_instead_of_tabs), NULL);
		gtk_box_pack_start (GTK_BOX (vbox_editor), spaces_instead_of_tabs, FALSE,
				FALSE, 0);
	}

	/* highlight current line */
	{
		GtkWidget *highlight_current_line = gtk_check_button_new_with_label (
				_("Highlight current line"));
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (highlight_current_line),
				latexila.prefs.highlight_current_line);
		g_signal_connect (G_OBJECT (highlight_current_line), "toggled",
				G_CALLBACK (cb_pref_highlight_current_line), NULL);
		gtk_box_pack_start (GTK_BOX (vbox_editor), highlight_current_line, FALSE,
				FALSE, 0);
	}

	/* highlight matching brackets */
	{
		GtkWidget *highlight_matching_brackets = gtk_check_button_new_with_label (
				_("Highlight matching brackets"));
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (highlight_matching_brackets),
				latexila.prefs.highlight_matching_brackets);
		g_signal_connect (G_OBJECT (highlight_matching_brackets), "toggled",
				G_CALLBACK (cb_pref_highlight_matching_brackets), NULL);
		gtk_box_pack_start (GTK_BOX (vbox_editor), highlight_matching_brackets, FALSE,
				FALSE, 0);
	}

	/* font */
	{
		GtkWidget *hbox = gtk_hbox_new (FALSE, 5);
		GtkWidget *label = gtk_label_new (_("Font:"));
		// TODO check memory leaks here
		GtkWidget *font_button = gtk_font_button_new_with_font (
				latexila.prefs.font_str);
		g_signal_connect (G_OBJECT (font_button), "font-set",
				G_CALLBACK (cb_pref_font_set), NULL);
		gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (hbox), font_button, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox_font_and_colors), hbox, FALSE, FALSE, 0);
	}

	/* style schemes */
	{
		GtkWidget *frame = gtk_frame_new (_("Color scheme"));
		gtk_box_pack_start (GTK_BOX (vbox_font_and_colors), frame, FALSE, FALSE, 0);

		GtkListStore *style_schemes_list_store = gtk_list_store_new (
				N_COLUMNS_STYLE_SCHEMES, G_TYPE_STRING, G_TYPE_STRING);

		GtkWidget *style_schemes_tree_view = gtk_tree_view_new_with_model (
				GTK_TREE_MODEL (style_schemes_list_store));
		gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (style_schemes_tree_view),
				FALSE);
		GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
		GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes (
				"Name and description", renderer,
				"text", COLUMN_STYLE_SCHEME_DESC,
				NULL);
		gtk_tree_view_append_column (GTK_TREE_VIEW (style_schemes_tree_view),
				column);

		GtkTreeSelection *select = gtk_tree_view_get_selection (
				GTK_TREE_VIEW (style_schemes_tree_view));
		gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
		g_signal_connect (G_OBJECT (select), "changed",
				G_CALLBACK (cb_style_scheme_changed), NULL);

		fill_style_schemes_list_store (style_schemes_list_store, select);

		gtk_container_add (GTK_CONTAINER (frame), style_schemes_tree_view);
	}

	/* command view entry */
	{
		GtkWidget *hbox = gtk_hbox_new (FALSE, 5);
		GtkWidget *label = gtk_label_new (_("Program for viewing documents:"));
		GtkWidget *command_view_entry = gtk_entry_new ();
		gtk_entry_set_text (GTK_ENTRY (command_view_entry),
				latexila.prefs.command_view);
		g_signal_connect (G_OBJECT (command_view_entry), "changed",
				G_CALLBACK (cb_pref_command_view), NULL);
		gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (hbox), command_view_entry, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox_latex), hbox, FALSE, FALSE, 0);
	}

	/* commands (latex, pdflatex, dvipdf, dvips, ...) */
	{
		GtkWidget *hbox = gtk_hbox_new (FALSE, 5);
		GtkWidget *label1 = gtk_label_new (_("latex command:"));
		GtkWidget *command_latex = gtk_entry_new ();
		gtk_entry_set_text (GTK_ENTRY (command_latex), latexila.prefs.command_latex);
		g_signal_connect (G_OBJECT (command_latex), "changed",
				G_CALLBACK (cb_pref_command_latex), NULL);
		gtk_box_pack_start (GTK_BOX (hbox), label1, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (hbox), command_latex, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox_latex), hbox, FALSE, FALSE, 0);

		hbox = gtk_hbox_new (FALSE, 5);
		GtkWidget *label2 = gtk_label_new (_("pdflatex command:"));
		GtkWidget *command_pdflatex = gtk_entry_new ();
		gtk_entry_set_text (GTK_ENTRY (command_pdflatex), latexila.prefs.command_pdflatex);
		g_signal_connect (G_OBJECT (command_pdflatex), "changed",
				G_CALLBACK (cb_pref_command_pdflatex), NULL);
		gtk_box_pack_start (GTK_BOX (hbox), label2, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (hbox), command_pdflatex, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox_latex), hbox, FALSE, FALSE, 0);

		hbox = gtk_hbox_new (FALSE, 5);
		GtkWidget *label3 = gtk_label_new (_("DVI to PDF command:"));
		GtkWidget *command_dvipdf = gtk_entry_new ();
		gtk_entry_set_text (GTK_ENTRY (command_dvipdf), latexila.prefs.command_dvipdf);
		g_signal_connect (G_OBJECT (command_dvipdf), "changed",
				G_CALLBACK (cb_pref_command_dvipdf), NULL);
		gtk_box_pack_start (GTK_BOX (hbox), label3, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (hbox), command_dvipdf, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox_latex), hbox, FALSE, FALSE, 0);

		hbox = gtk_hbox_new (FALSE, 5);
		GtkWidget *label4 = gtk_label_new (_("DVI to PS command:"));
		GtkWidget *command_dvips = gtk_entry_new ();
		gtk_entry_set_text (GTK_ENTRY (command_dvips), latexila.prefs.command_dvips);
		g_signal_connect (G_OBJECT (command_dvips), "changed",
				G_CALLBACK (cb_pref_command_dvips), NULL);
		gtk_box_pack_start (GTK_BOX (hbox), label4, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (hbox), command_dvips, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox_latex), hbox, FALSE, FALSE, 0);

		hbox = gtk_hbox_new (FALSE, 5);
		GtkWidget *label5 = gtk_label_new (_("BibTeX command:"));
		GtkWidget *command_bibtex = gtk_entry_new ();
		gtk_entry_set_text (GTK_ENTRY (command_bibtex), latexila.prefs.command_bibtex);
		g_signal_connect (G_OBJECT (command_bibtex), "changed",
				G_CALLBACK (cb_pref_command_bibtex), NULL);
		gtk_box_pack_start (GTK_BOX (hbox), label5, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (hbox), command_bibtex, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox_latex), hbox, FALSE, FALSE, 0);

		hbox = gtk_hbox_new (FALSE, 5);
		GtkWidget *label6 = gtk_label_new (_("MakeIndex command:"));
		GtkWidget *command_makeindex = gtk_entry_new ();
		gtk_entry_set_text (GTK_ENTRY (command_makeindex), latexila.prefs.command_makeindex);
		g_signal_connect (G_OBJECT (command_makeindex), "changed",
				G_CALLBACK (cb_pref_command_makeindex), NULL);
		gtk_box_pack_start (GTK_BOX (hbox), label6, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (hbox), command_makeindex, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox_latex), hbox, FALSE, FALSE, 0);

		// set the same width for the labels
		// the longer label is label6
		GtkRequisition size;
		gtk_widget_size_request (label6, &size);
		gtk_widget_set_size_request (label1, size.width, 0);
		gtk_widget_set_size_request (label2, size.width, 0);
		gtk_widget_set_size_request (label3, size.width, 0);
		gtk_widget_set_size_request (label4, size.width, 0);
		gtk_widget_set_size_request (label5, size.width, 0);

		// flush left
		gtk_misc_set_alignment (GTK_MISC (label1), 0.0, 0.5);
		gtk_misc_set_alignment (GTK_MISC (label2), 0.0, 0.5);
		gtk_misc_set_alignment (GTK_MISC (label4), 0.0, 0.5);
		gtk_misc_set_alignment (GTK_MISC (label5), 0.0, 0.5);
		gtk_misc_set_alignment (GTK_MISC (label6), 0.0, 0.5);
	}

	/* document compilation options */
	{
		GtkWidget *frame = gtk_frame_new (_("Document Compilation"));
		gtk_box_pack_start (GTK_BOX (vbox_latex), frame, FALSE, FALSE, 0);

		GtkWidget *vbox_compilation = gtk_vbox_new (FALSE, 3);
		gtk_container_add (GTK_CONTAINER (frame), vbox_compilation);

		// show all output
		GtkWidget *show_all_output = gtk_check_button_new_with_label (_("Show all output"));
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (show_all_output),
				latexila.prefs.compile_show_all_output);
		g_signal_connect (G_OBJECT (show_all_output), "toggled",
				G_CALLBACK (cb_pref_compile_show_all_output), NULL);
		gtk_box_pack_start (GTK_BOX (vbox_compilation), show_all_output, FALSE, FALSE, 0);

		// non stop mode interaction
		GtkWidget *non_stop = gtk_check_button_new_with_label ("-interaction=nonstopmode");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (non_stop),
				latexila.prefs.compile_non_stop);
		g_signal_connect (G_OBJECT (non_stop), "toggled",
				G_CALLBACK (cb_pref_compile_non_stop), NULL);
		gtk_box_pack_start (GTK_BOX (vbox_compilation), non_stop, FALSE, FALSE, 0);
	}


	/* web browser */
	{
		GtkWidget *hbox = gtk_hbox_new (FALSE, 5);
		GtkWidget *label = gtk_label_new (_("Web browser:"));
		GtkWidget *web_browser_entry = gtk_entry_new ();
		gtk_entry_set_text (GTK_ENTRY (web_browser_entry),
				latexila.prefs.command_web_browser);
		g_signal_connect (G_OBJECT (web_browser_entry), "changed",
				G_CALLBACK (cb_pref_web_browser), NULL);
		gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (hbox), web_browser_entry, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox_other), hbox, FALSE, FALSE, 0);
	}

	/* reopen files on startup */
	{
		GtkWidget *reopen = gtk_check_button_new_with_label (
				_("Reopen files on startup"));
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (reopen),
				latexila.prefs.reopen_files_on_startup);
		g_signal_connect (G_OBJECT (reopen), "toggled",
				G_CALLBACK (cb_reopen_files_on_startup), NULL);
		gtk_box_pack_start (GTK_BOX (vbox_other), reopen, FALSE, FALSE, 0);
	}

	/* file browser: show all files */
	{
		GtkWidget *fb_show_all_files = gtk_check_button_new_with_label (
				_("File browser: show all files"));
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (fb_show_all_files),
				latexila.prefs.file_browser_show_all_files);
		g_signal_connect (G_OBJECT (fb_show_all_files), "toggled",
				G_CALLBACK (cb_file_browser_show_all_files), NULL);
		gtk_box_pack_start (GTK_BOX (vbox_other), fb_show_all_files, FALSE, FALSE, 0);
	}

	/* delete auxiliaries files on exit */
	{
		GtkWidget *delete_aux_files = gtk_check_button_new_with_label (
				_("Clean-up build files after close (*.aux, *.log, *.out, *.toc, etc)"));
		gtk_widget_set_tooltip_text (delete_aux_files,
				".aux .bit .blg .lof .log .lot .glo .glx .gxg .gxs .idx .ilg .ind .out .url .svn .toc");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (delete_aux_files),
				latexila.prefs.delete_aux_files);
		g_signal_connect (G_OBJECT (delete_aux_files), "toggled",
				G_CALLBACK (cb_delete_aux_files), NULL);
		gtk_box_pack_start (GTK_BOX (vbox_other), delete_aux_files, FALSE, FALSE, 0);
	}

	/* toolbars horizontal */
	{
		GtkWidget *toolbars_horiz = gtk_check_button_new_with_label (
				_("Show the edit toolbar on the same line as the main toolbar (restart needed)"));
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (toolbars_horiz),
				latexila.prefs.toolbars_horizontal);
		g_signal_connect (G_OBJECT (toolbars_horiz), "toggled",
				G_CALLBACK (cb_toolbars_horizontal), NULL);
		gtk_box_pack_start (GTK_BOX (vbox_other), toolbars_horiz, FALSE, FALSE, 0);
	}

	gtk_widget_show_all (content_area);
}
