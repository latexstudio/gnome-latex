/*
 * This file is part of LaTeXila.
 *
 * Copyright (C) 2017 - Sébastien Wilmet <swilmet@gnome.org>
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

/**
 * SECTION:latex-commands
 * @title: LaTeX commands
 * @short_description: LaTeX menu, Math menu and Edit toolbar
 */

#include "latexila-latex-commands.h"
#include "latexila-utils.h"
#include "latexila-view.h"

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
  TeplView *view;
  gchar *indent;
  gchar *text_before;
  gchar *text_after;

  view = tepl_tab_group_get_active_view (TEPL_TAB_GROUP (tepl_window));
  g_return_if_fail (view != NULL);

  indent = latexila_view_get_indentation_style (GTK_SOURCE_VIEW (view));

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
  TeplView *view;
  gchar *indent;
  gchar *text_before;
  gchar *text_after;

  view = tepl_tab_group_get_active_view (TEPL_TAB_GROUP (tepl_window));
  g_return_if_fail (view != NULL);

  indent = latexila_view_get_indentation_style (GTK_SOURCE_VIEW (view));

  text_before = g_strdup_printf ("\\begin{table}\n"
                                 "%s\\caption{",
                                 indent);

  text_after = g_strdup_printf ("}\n"
                                "%s\\label{tab:}\n"
                                "\n"
                                "%s\\begin{center}\n"
                                "%s%s\\begin{tabular}{cc}\n"
                                "%s%s%s & \\\\\n"
                                "%s%s%s & \\\\\n"
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

  latexila_latex_commands_insert_text (tepl_window, text_before, text_after, NULL);

  g_free (indent);
  g_free (text_before);
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

/**
 * latexila_latex_commands_add_actions:
 * @gtk_window: a #GtkApplicationWindow.
 *
 * Adds the #GAction's related to the LaTeX and Math menus.
 */
void
latexila_latex_commands_add_actions (GtkApplicationWindow *gtk_window)
{
  TeplApplicationWindow *tepl_window;

  const GActionEntry entries[] = {
    { "latex-command-with-braces", latex_command_with_braces_cb, "s" },
    { "latex-command-env-simple", latex_command_env_simple_cb, "s" },
    { "latex-command-env-figure", latex_command_env_figure_cb },
    { "latex-command-env-table", latex_command_env_table_cb },
    { "latex-command-char-style", latex_command_char_style_cb, "s" },
  };

  g_return_if_fail (GTK_IS_APPLICATION_WINDOW (gtk_window));

  tepl_window = tepl_application_window_get_from_gtk_application_window (gtk_window);

  amtk_action_map_add_action_entries_check_dups (G_ACTION_MAP (gtk_window),
                                                 entries,
                                                 G_N_ELEMENTS (entries),
                                                 tepl_window);
}
