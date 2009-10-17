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
#include <sys/stat.h> // for S_IRWXU

#include "main.h"
#include "prefs.h"
#include "config.h"
#include "print.h"
#include "callbacks.h"

static void load_default_preferences (preferences_t *prefs);
static gchar * get_rc_file (void);
static void create_preferences (void);
static void cb_pref_dialog_close (GtkDialog *dialog, gint response_id,
		gpointer user_data);
static void cb_pref_line_numbers (GtkToggleButton *toggle_button,
		gpointer user_data);
static void cb_pref_font_set (GtkFontButton *font_button, gpointer user_data);
static void cb_pref_command_view (GtkEditable *editable, gpointer user_data);
static void cb_pref_command_latex (GtkEditable *editable, gpointer user_data);
static void cb_pref_command_pdflatex (GtkEditable *editable, gpointer user_data);
static void cb_pref_command_dvipdf (GtkEditable *editable, gpointer user_data);
static void cb_pref_command_dvips (GtkEditable *editable, gpointer user_data);

/* default values */
// there is an underscore in the end of each variable name
static gboolean	show_line_numbers_	= FALSE;
static gboolean	show_side_pane_		= TRUE;
static gboolean show_edit_toolbar_	= TRUE;
static gint		window_width_		= 800;
static gint		window_height_		= 600;
static gboolean	window_maximised_	= FALSE;
static gint		main_hpaned_pos_	= 180;
static gint		vpaned_pos_			= 380;
static gint		log_hpaned_pos_		= 190;
static gchar	*font_				= "Monospace 10";
static gchar	*command_view_		= "evince";
static gchar	*command_latex_		= COMMAND_LATEX;
static gchar	*command_pdflatex_	= COMMAND_PDFLATEX;
static gchar	*command_dvipdf_	= COMMAND_DVIPDF;
static gchar	*command_dvips_		= COMMAND_DVIPS;

void
load_preferences (preferences_t *prefs)
{
	gchar *rc_file = get_rc_file ();
	if (! g_file_test (rc_file, G_FILE_TEST_EXISTS))
	{
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
		error = NULL;
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

	// look, I see light, we are close to the exit!
	
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

	prefs->file_chooser_dir = g_key_file_get_string (key_file, PROGRAM_NAME,
			"file_chooser_directory", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->file_chooser_dir_is_empty = TRUE;
		prefs->file_chooser_dir = NULL;
		g_error_free (error);
		error = NULL;
	}
	else
		prefs->file_chooser_dir_is_empty = FALSE;

	print_info ("load user preferences: OK");
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

	if (! prefs->file_chooser_dir_is_empty)
		g_key_file_set_string (key_file, PROGRAM_NAME, "file_chooser_directory",
				prefs->file_chooser_dir);

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
	prefs->file_chooser_dir_is_empty = TRUE;
	prefs->file_chooser_dir = NULL;

	set_current_font_prefs (prefs);
}

static gchar *
get_rc_file (void)
{
	// rc_file must be freed
	gchar *rc_file = g_build_filename (g_get_home_dir (), ".config", "latexila",
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

	if (latexila.active_doc == NULL)
		return;

	// traverse the list
	// an other solution is to call g_list_foreach ()
	GList *current = latexila.all_docs;
	do
	{
		document_t *doc = g_list_nth_data (current, 0);
		gtk_source_view_set_show_line_numbers (
				GTK_SOURCE_VIEW (doc->source_view), show_line_numbers);
	}
	while ((current = g_list_next (current)) != NULL);
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
	gtk_box_set_spacing (GTK_BOX (content_area), 3);

	/* show line numbers */
	GtkWidget *line_numbers = gtk_check_button_new_with_label (
			_("Display line numbers"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (line_numbers),
			latexila.prefs.show_line_numbers);
	g_signal_connect (G_OBJECT (line_numbers), "toggled",
			G_CALLBACK (cb_pref_line_numbers), NULL);
	gtk_box_pack_start (GTK_BOX (content_area), line_numbers, FALSE, FALSE, 0);

	/* font */
	GtkWidget *hbox = gtk_hbox_new (FALSE, 5);
	GtkWidget *label = gtk_label_new (_("Font:"));
	GtkWidget *font_button = gtk_font_button_new_with_font (
			latexila.prefs.font_str);
	g_signal_connect (G_OBJECT (font_button), "font-set",
			G_CALLBACK (cb_pref_font_set), NULL);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), font_button, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (content_area), hbox, FALSE, FALSE, 0);

	/* command view entry */
	hbox = gtk_hbox_new (FALSE, 5);
	label = gtk_label_new (_("Program for viewing documents:"));
	GtkWidget *command_view_entry = gtk_entry_new ();
	gtk_entry_set_text (GTK_ENTRY (command_view_entry),
			latexila.prefs.command_view);
	g_signal_connect (G_OBJECT (command_view_entry), "changed",
			G_CALLBACK (cb_pref_command_view), NULL);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), command_view_entry, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (content_area), hbox, FALSE, FALSE, 0);

	/* commands (latex, pdflatex, dvipdf, dvips) */
	GtkWidget *table = gtk_table_new (4, 2, FALSE);

	label = gtk_label_new (_("latex command:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	GtkWidget *command_latex = gtk_entry_new ();
	gtk_entry_set_text (GTK_ENTRY (command_latex),
			latexila.prefs.command_latex);
	g_signal_connect (G_OBJECT (command_latex), "changed",
			G_CALLBACK (cb_pref_command_latex), NULL);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 0, 1);
	gtk_table_attach_defaults (GTK_TABLE (table), command_latex, 1, 2, 0, 1);

	label = gtk_label_new (_("pdflatex command:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	GtkWidget *command_pdflatex = gtk_entry_new ();
	gtk_entry_set_text (GTK_ENTRY (command_pdflatex),
			latexila.prefs.command_pdflatex);
	g_signal_connect (G_OBJECT (command_pdflatex), "changed",
			G_CALLBACK (cb_pref_command_pdflatex), NULL);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 1, 2);
	gtk_table_attach_defaults (GTK_TABLE (table), command_pdflatex, 1, 2, 1, 2);

	label = gtk_label_new (_("DVI to PDF command:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	GtkWidget *command_dvipdf = gtk_entry_new ();
	gtk_entry_set_text (GTK_ENTRY (command_dvipdf),
			latexila.prefs.command_dvipdf);
	g_signal_connect (G_OBJECT (command_dvipdf), "changed",
			G_CALLBACK (cb_pref_command_dvipdf), NULL);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 2, 3);
	gtk_table_attach_defaults (GTK_TABLE (table), command_dvipdf, 1, 2, 2, 3);

	label = gtk_label_new (_("DVI to PS command:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	GtkWidget *command_dvips = gtk_entry_new ();
	gtk_entry_set_text (GTK_ENTRY (command_dvips),
			latexila.prefs.command_dvips);
	g_signal_connect (G_OBJECT (command_dvips), "changed",
			G_CALLBACK (cb_pref_command_dvips), NULL);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 3, 4);
	gtk_table_attach_defaults (GTK_TABLE (table), command_dvips, 1, 2, 3, 4);

	gtk_box_pack_start (GTK_BOX (content_area), table, TRUE, TRUE, 5);

	gtk_widget_show_all (content_area);
}

