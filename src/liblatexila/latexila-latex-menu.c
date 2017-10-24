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
 * SECTION:latex-menu
 * @title: LatexilaLatexMenu
 * @short_description: LaTeX and Math menus
 */

#include "latexila-latex-menu.h"
#include "latexila-utils.h"

/* Temporarily public, will be made private when all GActions for the LaTeX and
 * Math menus are implemented.
 */
void
latexila_latex_menu_insert_text (TeplApplicationWindow *tepl_window,
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
