/*
 * This file is part of GNOME LaTeX.
 *
 * Copyright (C) 2015 - Sébastien Wilmet <swilmet@gnome.org>
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

#ifndef LATEXILA_TEMPLATES_COMMON_H
#define LATEXILA_TEMPLATES_COMMON_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

enum _LatexilaTemplatesColumn
{
  LATEXILA_TEMPLATES_COLUMN_PIXBUF_ICON_NAME,

  /* The string stored in the rc file (article, report, ...). For
   * backward-compatibility reasons, this is not the same as PIXBUF_ICON_NAME.
   */
  LATEXILA_TEMPLATES_COLUMN_CONFIG_ICON_NAME,

  LATEXILA_TEMPLATES_COLUMN_NAME,

  /* The file where is stored the contents. For a default template this is an
   * XML file, for a personal template this is a .tex file. A NULL file is
   * valid for a default template, it means an empty template.
   */
  LATEXILA_TEMPLATES_COLUMN_FILE,

  LATEXILA_TEMPLATES_N_COLUMNS
};

void            latexila_templates_init_store       (GtkListStore *store);

void            latexila_templates_add_template     (GtkListStore *store,
                                                     const gchar  *name,
                                                     const gchar  *config_icon_name,
                                                     GFile        *file);

GtkTreeView *   latexila_templates_get_view         (GtkListStore *store);

G_END_DECLS

#endif /* LATEXILA_TEMPLATES_COMMON_H */
