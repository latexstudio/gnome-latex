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
#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>
#include <sys/stat.h> // for S_IRWXU

#include "main.h"
#include "prefs.h"
#include "config.h"
#include "print.h"

static void load_default_preferences (preferences_t *prefs);
static gchar * get_rc_file (void);

/* default values */
// there is an underscore in the end of each variable name
static gboolean	show_line_numbers_	= FALSE;
static gboolean	show_side_pane_		= TRUE;
static gint		window_width_		= 800;
static gint		window_height_		= 600;
static gboolean	window_maximised_	= FALSE;
static gint		main_hpaned_pos_	= 180;
static gint		vpaned_pos_			= 380;
static gint		log_hpaned_pos_		= 190;
static gchar	*command_view_		= "evince";
static gchar	*font_				= "Monospace 10";

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

	// look, I see light, we are close to the exit!
	
	prefs->command_view = g_key_file_get_string (key_file, PROGRAM_NAME,
			"command_view", &error);
	if (error != NULL)
	{
		print_warning ("%s", error->message);
		prefs->command_view = g_strdup (command_view_);
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

	print_info ("load user preferences: OK");
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
	g_key_file_set_string (key_file, PROGRAM_NAME, "command_view",
			prefs->command_view);
	g_key_file_set_string (key_file, PROGRAM_NAME, "font", prefs->font_str);

	/* set the keys that must be taken from the widgets */
	GdkWindowState flag = gdk_window_get_state (gtk_widget_get_window (
				GTK_WIDGET (latexila.main_window)));
	gboolean window_maximised = flag & GDK_WINDOW_STATE_MAXIMIZED;
	g_key_file_set_boolean (key_file, PROGRAM_NAME, "window_maximised",
			window_maximised);

	gint window_width, window_height;
	gtk_window_get_size (latexila.main_window, &window_width, &window_height);

	/* generate bugs with the panes positions
	// if the window is maximised, store sizes that are a bit smaller, else
	// making window non-maximised will have no effect
	if (window_maximised)
	{
		window_width -= window_width / 10;
		window_height -= window_height / 10;
	}
	*/

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
	prefs->window_width = window_width_;
	prefs->window_height = window_height_;
	prefs->window_maximised = window_maximised_;
	prefs->main_hpaned_pos = main_hpaned_pos_;
	prefs->vpaned_pos = vpaned_pos_;
	prefs->log_hpaned_pos = log_hpaned_pos_;
	prefs->command_view = g_strdup (command_view_);
	prefs->font_str = g_strdup (font_);

	set_current_font_prefs (prefs);
}

static gchar *
get_rc_file (void)
{
	gchar *rc_file = g_build_filename (g_get_home_dir (), ".config", "latexila",
			"latexilarc", NULL);
	return rc_file;
}
