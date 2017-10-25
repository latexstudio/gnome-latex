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
 * SECTION:view
 * @title: LatexilaView
 * @short_description: Additional #TeplView functions
 *
 * Additional #TeplView functions.
 */

#include "latexila-view.h"

/**
 * latexila_view_get_indentation_style:
 * @view: a #GtkSourceView.
 *
 * Returns one indentation level, as a string.
 *
 * Attention, this function assumes that the #GtkSourceView:indent-width is -1
 * or equal to the #GtkSourceView:tab-width, which is always the case in
 * LaTeXila (there is no #GSettings for the #GtkSourceView:indent-width).
 *
 * Returns: one indentation level, as a string. Free with g_free() when no
 * longer needed.
 */
gchar *
latexila_view_get_indentation_style (GtkSourceView *view)
{
  guint tab_width;
  gint indent_width;

  g_return_val_if_fail (GTK_SOURCE_IS_VIEW (view), NULL);

  tab_width = gtk_source_view_get_tab_width (view);
  indent_width = gtk_source_view_get_indent_width (view);
  g_return_val_if_fail (indent_width == -1 || indent_width == (gint)tab_width, NULL);

  if (gtk_source_view_get_insert_spaces_instead_of_tabs (view))
    return g_strnfill (tab_width, ' ');

  return g_strdup ("\t");
}
