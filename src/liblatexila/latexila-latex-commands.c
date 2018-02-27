/*
 * This file is part of GNOME LaTeX.
 *
 * Copyright (C) 2017 - Sébastien Wilmet <swilmet@gnome.org>
 *
 * GNOME LaTeX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNOME LaTeX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNOME LaTeX.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * SECTION:latex-commands
 * @title: LaTeX commands
 * @short_description: LaTeX menu, Math menu and Edit toolbar
 */

#include "config.h"
#include "latexila-latex-commands.h"
#include <glib/gi18n.h>
#include "latexila-utils.h"
#include "latexila-view.h"

/**
 * latexila_latex_commands_add_action_infos:
 * @gtk_app: the #GtkApplication instance.
 *
 * Creates the #AmtkActionInfo's related to the LaTeX and Math menus, and add
 * them to the #AmtkActionInfoStore as returned by
 * tepl_application_get_app_action_info_store().
 */
void
latexila_latex_commands_add_action_infos (GtkApplication *gtk_app)
{
	TeplApplication *tepl_app;
	AmtkActionInfoStore *store;

	const AmtkActionInfoEntry entries[] =
	{
		/* action, icon, label, accel, tooltip */

		{ "win.latex-command-env-figure", "image-x-generic", "\\begin{_figure}", NULL,
		  N_ ("Figure - \\begin{figure}") },
	};

	g_return_if_fail (GTK_IS_APPLICATION (gtk_app));

	tepl_app = tepl_application_get_from_gtk_application (gtk_app);
	store = tepl_application_get_app_action_info_store (tepl_app);

	amtk_action_info_store_add_entries (store,
					    entries,
					    G_N_ELEMENTS (entries),
					    GETTEXT_PACKAGE);
}

/* Util functions */

/* Temporarily public, will be made private when all GActions for the LaTeX and
 * Math menus are implemented.
 */
void
latexila_latex_commands_insert_text (TeplApplicationWindow *tepl_window,
				     const gchar           *text_before,
				     const gchar           *text_after,
				     const gchar           *text_if_no_selection)
{
	TeplView *view;
	GtkTextBuffer *buffer;
	GtkTextIter selection_start;
	GtkTextIter selection_end;
	gboolean has_selection;
	gchar *text_before_with_indent;
	gchar *text_after_with_indent;

	g_return_if_fail (TEPL_IS_APPLICATION_WINDOW (tepl_window));
	g_return_if_fail (text_before != NULL);
	g_return_if_fail (text_after != NULL);

	view = tepl_tab_group_get_active_view (TEPL_TAB_GROUP (tepl_window));
	g_return_if_fail (view != NULL);

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

	has_selection = gtk_text_buffer_get_selection_bounds (buffer,
							      &selection_start,
							      &selection_end);

	/* Take into account the current indentation. */
	{
		gchar *current_indent;
		gchar *newline_replacement;

		current_indent = tepl_iter_get_line_indentation (&selection_start);
		newline_replacement = g_strdup_printf ("\n%s", current_indent);

		text_before_with_indent = latexila_utils_str_replace (text_before, "\n", newline_replacement);
		text_after_with_indent = latexila_utils_str_replace (text_after, "\n", newline_replacement);

		g_free (current_indent);
		g_free (newline_replacement);
	}

	gtk_text_buffer_begin_user_action (buffer);

	/* Insert around the selected text.
	 * Move the cursor to the end.
	 */
	if (has_selection)
	{
		GtkTextMark *end_mark;
		GtkTextIter end_iter;

		end_mark = gtk_text_buffer_create_mark (buffer, NULL, &selection_end, FALSE);

		gtk_text_buffer_insert (buffer, &selection_start, text_before_with_indent, -1);

		gtk_text_buffer_get_iter_at_mark (buffer, &end_iter, end_mark);
		gtk_text_buffer_delete_mark (buffer, end_mark);

		gtk_text_buffer_insert (buffer, &end_iter, text_after_with_indent, -1);
		gtk_text_buffer_place_cursor (buffer, &end_iter);
	}
	/* No selection */
	else if (text_if_no_selection != NULL)
	{
		gtk_text_buffer_insert_at_cursor (buffer, text_if_no_selection, -1);
	}
	/* No selection, move the cursor between the two inserted chunks. */
	else
	{
		GtkTextIter between_iter;
		GtkTextMark *between_mark;

		gtk_text_buffer_insert_at_cursor (buffer, text_before_with_indent, -1);

		gtk_text_buffer_get_iter_at_mark (buffer,
						  &between_iter,
						  gtk_text_buffer_get_insert (buffer));
		between_mark = gtk_text_buffer_create_mark (buffer, NULL, &between_iter, TRUE);

		gtk_text_buffer_insert_at_cursor (buffer, text_after_with_indent, -1);

		gtk_text_buffer_get_iter_at_mark (buffer, &between_iter, between_mark);
		gtk_text_buffer_delete_mark (buffer, between_mark);
		gtk_text_buffer_place_cursor (buffer, &between_iter);
	}

	gtk_text_buffer_end_user_action (buffer);

	gtk_widget_grab_focus (GTK_WIDGET (view));

	g_free (text_before_with_indent);
	g_free (text_after_with_indent);
}

static gchar *
get_indentation (TeplApplicationWindow *tepl_window)
{
	TeplView *view;

	view = tepl_tab_group_get_active_view (TEPL_TAB_GROUP (tepl_window));
	g_return_val_if_fail (view != NULL, NULL);

	return latexila_view_get_indentation_style (GTK_SOURCE_VIEW (view));
}

/* When it doesn't make sense to have the selected text between @text_before and
 * @text_after when calling latexila_latex_commands_insert_text(), call this
 * function first.
 */
static void
deselect_text (TeplApplicationWindow *tepl_window)
{
	TeplBuffer *tepl_buffer;
	GtkTextBuffer *gtk_buffer;
	GtkTextIter selection_start;
	GtkTextIter selection_end;

	tepl_buffer = tepl_tab_group_get_active_buffer (TEPL_TAB_GROUP (tepl_window));
	g_return_if_fail (tepl_buffer != NULL);

	gtk_buffer = GTK_TEXT_BUFFER (tepl_buffer);

	gtk_text_buffer_get_selection_bounds (gtk_buffer,
					      &selection_start,
					      &selection_end);

	/* Always place cursor at beginning of selection, to have more predictable
	 * results, instead of placing the cursor at the insert mark.
	 */
	gtk_text_buffer_place_cursor (gtk_buffer, &selection_start);
}

/* GActions implementation */

static void
latex_command_simple_cb (GSimpleAction *action,
			 GVariant      *parameter,
			 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	const gchar *command;
	gchar *text_before;

	command = g_variant_get_string (parameter, NULL);
	text_before = g_strdup_printf ("\\%s", command);

	latexila_latex_commands_insert_text (tepl_window, text_before, "", NULL);

	g_free (text_before);
}

static void
latex_command_with_braces_cb (GSimpleAction *action,
			      GVariant      *parameter,
			      gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	const gchar *command;
	gchar *text_before;

	command = g_variant_get_string (parameter, NULL);
	text_before = g_strdup_printf ("\\%s{", command);

	latexila_latex_commands_insert_text (tepl_window, text_before, "}", NULL);

	g_free (text_before);
}

static void
latex_command_with_space_cb (GSimpleAction *action,
			     GVariant      *parameter,
			     gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	const gchar *command;
	gchar *text_before;

	command = g_variant_get_string (parameter, NULL);
	text_before = g_strdup_printf ("\\%s ", command);

	latexila_latex_commands_insert_text (tepl_window, text_before, "", NULL);

	g_free (text_before);
}

static void
latex_command_with_newline_cb (GSimpleAction *action,
			       GVariant      *parameter,
			       gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	const gchar *command;
	gchar *text_before;

	command = g_variant_get_string (parameter, NULL);
	text_before = g_strdup_printf ("\\%s\n", command);

	latexila_latex_commands_insert_text (tepl_window, text_before, "", NULL);

	g_free (text_before);
}

static void
latex_command_env_simple_cb (GSimpleAction *action,
			     GVariant      *parameter,
			     gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	const gchar *environment;
	gchar *text_before;
	gchar *text_after;

	environment = g_variant_get_string (parameter, NULL);
	text_before = g_strdup_printf ("\\begin{%s}\n", environment);
	text_after = g_strdup_printf ("\n\\end{%s}", environment);

	latexila_latex_commands_insert_text (tepl_window, text_before, text_after, NULL);

	g_free (text_before);
	g_free (text_after);
}

static void
latex_command_env_figure_cb (GSimpleAction *action,
			     GVariant      *parameter,
			     gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	gchar *indent;
	gchar *text_before;
	gchar *text_after;

	indent = get_indentation (tepl_window);

	text_before = g_strdup_printf ("\\begin{figure}\n"
				       "%s\\begin{center}\n"
				       "%s%s\\includegraphics{",
				       indent,
				       indent, indent);

	text_after = g_strdup_printf ("}\n"
				      "%s%s\\caption{}\n"
				      "%s%s\\label{fig:}\n"
				      "%s\\end{center}\n"
				      "\\end{figure}",
				      indent, indent,
				      indent, indent,
				      indent);

	deselect_text (tepl_window);
	latexila_latex_commands_insert_text (tepl_window, text_before, text_after, NULL);

	g_free (indent);
	g_free (text_before);
	g_free (text_after);
}

static void
latex_command_env_table_cb (GSimpleAction *action,
			    GVariant      *parameter,
			    gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	gchar *indent;
	gchar *text_before;
	gchar *text_after;

	indent = get_indentation (tepl_window);

	text_before = g_strdup_printf ("\\begin{table}\n"
				       "%s\\caption{",
				       indent);

	text_after = g_strdup_printf ("}\n"
				      "%s\\label{tab:}\n"
				      "\n"
				      "%s\\begin{center}\n"
				      "%s%s\\begin{tabular}{cc}\n"
				      "%s%s%s a & b \\\\\n"
				      "%s%s%s c & d \\\\\n"
				      "%s%s\\end{tabular}\n"
				      "%s\\end{center}\n"
				      "\\end{table}",
				      indent,
				      indent,
				      indent, indent,
				      indent, indent, indent,
				      indent, indent, indent,
				      indent, indent,
				      indent);

	deselect_text (tepl_window);
	latexila_latex_commands_insert_text (tepl_window, text_before, text_after, NULL);

	g_free (indent);
	g_free (text_before);
	g_free (text_after);
}

static void
latex_command_list_env_simple_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	const gchar *list_env;
	gchar *indent;
	gchar *text_before;
	gchar *text_after;

	list_env = g_variant_get_string (parameter, NULL);

	indent = get_indentation (tepl_window);

	text_before = g_strdup_printf ("\\begin{%s}\n"
				       "%s\\item ",
				       list_env,
				       indent);

	text_after = g_strdup_printf ("\n\\end{%s}", list_env);

	deselect_text (tepl_window);
	latexila_latex_commands_insert_text (tepl_window, text_before, text_after, NULL);

	g_free (indent);
	g_free (text_before);
	g_free (text_after);
}

static void
latex_command_list_env_description_cb (GSimpleAction *action,
				       GVariant      *parameter,
				       gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	gchar *indent;
	gchar *text_before;
	gchar *text_after;

	indent = get_indentation (tepl_window);

	text_before = g_strdup_printf ("\\begin{description}\n"
				       "%s\\item[",
				       indent);

	text_after = g_strdup_printf ("] \n\\end{description}");

	deselect_text (tepl_window);
	latexila_latex_commands_insert_text (tepl_window, text_before, text_after, NULL);

	g_free (indent);
	g_free (text_before);
	g_free (text_after);
}

static void
latex_command_list_env_list_cb (GSimpleAction *action,
				GVariant      *parameter,
				gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	gchar *indent;
	gchar *text_after;

	indent = get_indentation (tepl_window);
	text_after = g_strdup_printf ("}{}\n%s\\item \n\\end{list}", indent);

	deselect_text (tepl_window);
	latexila_latex_commands_insert_text (tepl_window, "\\begin{list}{", text_after, NULL);

	g_free (indent);
	g_free (text_after);
}

static void
latex_command_char_style_cb (GSimpleAction *action,
			     GVariant      *parameter,
			     gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	const gchar *style;
	TeplBuffer *buffer;
	TeplSelectionType selection_type;
	gchar *text_before;
	gchar *text_after;
	gchar *text_if_no_selection;

	style = g_variant_get_string (parameter, NULL);

	buffer = tepl_tab_group_get_active_buffer (TEPL_TAB_GROUP (tepl_window));
	g_return_if_fail (buffer != NULL);

	selection_type = tepl_buffer_get_selection_type (buffer);

	if (selection_type == TEPL_SELECTION_TYPE_MULTIPLE_LINES)
	{
		text_before = g_strdup_printf ("\\begin{%s}\n", style);
		text_after = g_strdup_printf ("\n\\end{%s}", style);
		text_if_no_selection = NULL;
	}
	else
	{
		text_before = g_strdup_printf ("{\\%s ", style);
		text_after = g_strdup_printf ("}");
		text_if_no_selection = g_strdup_printf ("\\%s ", style);
	}

	latexila_latex_commands_insert_text (tepl_window,
					     text_before,
					     text_after,
					     text_if_no_selection);

	g_free (text_before);
	g_free (text_after);
	g_free (text_if_no_selection);
}

static void
latex_command_tabular_tabular_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	gchar *indent;
	gchar *text_after;

	indent = get_indentation (tepl_window);
	text_after = g_strdup_printf ("}\n"
				      "%s a & b \\\\\n"
				      "%s c & d \\\\\n"
				      "\\end{tabular}",
				      indent,
				      indent);

	deselect_text (tepl_window);
	latexila_latex_commands_insert_text (tepl_window,
					     "\\begin{tabular}{cc",
					     text_after,
					     NULL);

	g_free (indent);
	g_free (text_after);
}

static void
latex_command_tabular_multicolumn_cb (GSimpleAction *action,
				      GVariant      *parameter,
				      gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\multicolumn{}{}{", "}", NULL);
}

static void
latex_command_tabular_cline_cb (GSimpleAction *action,
				GVariant      *parameter,
				gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\cline{", "-}", NULL);
}

static void
latex_command_presentation_frame_cb (GSimpleAction *action,
				     GVariant      *parameter,
				     gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	gchar *indent;
	gchar *text_before;

	indent = get_indentation (tepl_window);
	text_before = g_strdup_printf ("\\begin{frame}\n"
				       "%s\\frametitle{}\n"
				       "%s\\framesubtitle{}\n",
				       indent,
				       indent);

	latexila_latex_commands_insert_text (tepl_window,
					     text_before,
					     "\n\\end{frame}",
					     NULL);

	g_free (indent);
	g_free (text_before);
}

static void
latex_command_presentation_block_cb (GSimpleAction *action,
				     GVariant      *parameter,
				     gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window,
					     "\\begin{block}{}\n",
					     "\n\\end{block}",
					     NULL);
}

static void
latex_command_presentation_columns_cb (GSimpleAction *action,
				       GVariant      *parameter,
				       gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	gchar *indent;
	gchar *text_before;
	gchar *text_after;

	indent = get_indentation (tepl_window);

	text_before = g_strdup_printf ("\\begin{columns}\n"
				       "%s\\begin{column}{.5\\textwidth}\n",
				       indent);

	text_after = g_strdup_printf ("\n"
				      "%s\\end{column}\n"
				      "%s\\begin{column}{.5\\textwidth}\n\n"
				      "%s\\end{column}\n"
				      "\\end{columns}",
				      indent,
				      indent,
				      indent);

	latexila_latex_commands_insert_text (tepl_window, text_before, text_after, NULL);

	g_free (indent);
	g_free (text_before);
	g_free (text_after);
}

static void
latex_command_spacing_new_line_cb (GSimpleAction *action,
				   GVariant      *parameter,
				   gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\\\\n", "", NULL);
}

static void
latex_command_ams_packages_cb (GSimpleAction *action,
			       GVariant      *parameter,
			       gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window,
					     "\\usepackage{amsmath}\n"
					     "\\usepackage{amsfonts}\n"
					     "\\usepackage{amssymb}",
					     "",
					     NULL);
}

static void
math_command_env_normal_cb (GSimpleAction *action,
			    GVariant      *parameter,
			    gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "$ ", " $", NULL);
}

static void
math_command_env_centered_cb (GSimpleAction *action,
			      GVariant      *parameter,
			      gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\[ ", " \\]", NULL);
}

static void
math_command_env_array_cb (GSimpleAction *action,
			   GVariant      *parameter,
			   gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window,
					     "\\begin{align*}\n",
					     "\n\\end{align*}",
					     NULL);
}

static void
math_command_misc_superscript_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "^{", "}", NULL);
}

static void
math_command_misc_subscript_cb (GSimpleAction *action,
				GVariant      *parameter,
				gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "_{", "}", NULL);
}

static void
math_command_misc_frac_cb (GSimpleAction *action,
			   GVariant      *parameter,
			   gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\frac{", "}{}", NULL);
}

static void
math_command_misc_nth_root_cb (GSimpleAction *action,
			       GVariant      *parameter,
			       gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\sqrt[]{", "}", NULL);
}

static void
math_command_spaces_small_cb (GSimpleAction *action,
			      GVariant      *parameter,
			      gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\, ", "", NULL);
}

static void
math_command_spaces_medium_cb (GSimpleAction *action,
			       GVariant      *parameter,
			       gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\: ", "", NULL);
}

static void
math_command_spaces_large_cb (GSimpleAction *action,
			      GVariant      *parameter,
			      gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\; ", "", NULL);
}

static void
math_command_delimiter_left1_cb (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\left( ", "", NULL);
}

static void
math_command_delimiter_left2_cb (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\left[ ", "", NULL);
}

static void
math_command_delimiter_left3_cb (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\left\\lbrace ", "", NULL);
}

static void
math_command_delimiter_left4_cb (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\left\\langle ", "", NULL);
}

static void
math_command_delimiter_left5_cb (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\left) ", "", NULL);
}

static void
math_command_delimiter_left6_cb (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\left] ", "", NULL);
}

static void
math_command_delimiter_left7_cb (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\left\\rbrace ", "", NULL);
}

static void
math_command_delimiter_left8_cb (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\left\\rangle ", "", NULL);
}

static void
math_command_delimiter_left9_cb (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\left. ", "", NULL);
}

static void
math_command_delimiter_right1_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\right) ", "", NULL);
}

static void
math_command_delimiter_right2_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\right] ", "", NULL);
}

static void
math_command_delimiter_right3_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\right\\rbrace ", "", NULL);
}

static void
math_command_delimiter_right4_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\right\\rangle ", "", NULL);
}

static void
math_command_delimiter_right5_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\right( ", "", NULL);
}

static void
math_command_delimiter_right6_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\right[ ", "", NULL);
}

static void
math_command_delimiter_right7_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\right\\lbrace ", "", NULL);
}

static void
math_command_delimiter_right8_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\right\\langle ", "", NULL);
}

static void
math_command_delimiter_right9_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	latexila_latex_commands_insert_text (tepl_window, "\\right. ", "", NULL);
}

/**
 * latexila_latex_commands_add_actions:
 * @gtk_window: a #GtkApplicationWindow.
 *
 * Creates the #GAction's related to the LaTeX and Math menus, and add them to
 * @gtk_window.
 */
void
latexila_latex_commands_add_actions (GtkApplicationWindow *gtk_window)
{
	TeplApplicationWindow *tepl_window;

	const GActionEntry entries[] = {
		{ "latex-command-simple", latex_command_simple_cb, "s" },
		{ "latex-command-with-braces", latex_command_with_braces_cb, "s" },
		{ "latex-command-with-space", latex_command_with_space_cb, "s" },
		{ "latex-command-with-newline", latex_command_with_newline_cb, "s" },
		{ "latex-command-env-simple", latex_command_env_simple_cb, "s" },
		{ "latex-command-env-figure", latex_command_env_figure_cb },
		{ "latex-command-env-table", latex_command_env_table_cb },
		{ "latex-command-list-env-simple", latex_command_list_env_simple_cb, "s" },
		{ "latex-command-list-env-description", latex_command_list_env_description_cb },
		{ "latex-command-list-env-list", latex_command_list_env_list_cb },
		{ "latex-command-char-style", latex_command_char_style_cb, "s" },
		{ "latex-command-tabular-tabular", latex_command_tabular_tabular_cb },
		{ "latex-command-tabular-multicolumn", latex_command_tabular_multicolumn_cb },
		{ "latex-command-tabular-cline", latex_command_tabular_cline_cb },
		{ "latex-command-presentation-frame", latex_command_presentation_frame_cb },
		{ "latex-command-presentation-block", latex_command_presentation_block_cb },
		{ "latex-command-presentation-columns", latex_command_presentation_columns_cb },
		{ "latex-command-spacing-new-line", latex_command_spacing_new_line_cb },
		{ "latex-command-ams-packages", latex_command_ams_packages_cb },
		{ "math-command-env-normal", math_command_env_normal_cb },
		{ "math-command-env-centered", math_command_env_centered_cb },
		{ "math-command-env-array", math_command_env_array_cb },
		{ "math-command-misc-superscript", math_command_misc_superscript_cb },
		{ "math-command-misc-subscript", math_command_misc_subscript_cb },
		{ "math-command-misc-frac", math_command_misc_frac_cb },
		{ "math-command-misc-nth-root", math_command_misc_nth_root_cb },
		{ "math-command-spaces-small", math_command_spaces_small_cb },
		{ "math-command-spaces-medium", math_command_spaces_medium_cb },
		{ "math-command-spaces-large", math_command_spaces_large_cb },
		{ "math-command-delimiter-left1", math_command_delimiter_left1_cb },
		{ "math-command-delimiter-left2", math_command_delimiter_left2_cb },
		{ "math-command-delimiter-left3", math_command_delimiter_left3_cb },
		{ "math-command-delimiter-left4", math_command_delimiter_left4_cb },
		{ "math-command-delimiter-left5", math_command_delimiter_left5_cb },
		{ "math-command-delimiter-left6", math_command_delimiter_left6_cb },
		{ "math-command-delimiter-left7", math_command_delimiter_left7_cb },
		{ "math-command-delimiter-left8", math_command_delimiter_left8_cb },
		{ "math-command-delimiter-left9", math_command_delimiter_left9_cb },
		{ "math-command-delimiter-right1", math_command_delimiter_right1_cb },
		{ "math-command-delimiter-right2", math_command_delimiter_right2_cb },
		{ "math-command-delimiter-right3", math_command_delimiter_right3_cb },
		{ "math-command-delimiter-right4", math_command_delimiter_right4_cb },
		{ "math-command-delimiter-right5", math_command_delimiter_right5_cb },
		{ "math-command-delimiter-right6", math_command_delimiter_right6_cb },
		{ "math-command-delimiter-right7", math_command_delimiter_right7_cb },
		{ "math-command-delimiter-right8", math_command_delimiter_right8_cb },
		{ "math-command-delimiter-right9", math_command_delimiter_right9_cb },
	};

	g_return_if_fail (GTK_IS_APPLICATION_WINDOW (gtk_window));

	tepl_window = tepl_application_window_get_from_gtk_application_window (gtk_window);

	amtk_action_map_add_action_entries_check_dups (G_ACTION_MAP (gtk_window),
						       entries,
						       G_N_ELEMENTS (entries),
						       tepl_window);
}
