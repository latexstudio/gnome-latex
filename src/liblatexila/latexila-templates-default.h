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

#ifndef LATEXILA_TEMPLATES_DEFAULT_H
#define LATEXILA_TEMPLATES_DEFAULT_H

#include <gtk/gtk.h>
#include "latexila-types.h"

G_BEGIN_DECLS

#define LATEXILA_TYPE_TEMPLATES_DEFAULT latexila_templates_default_get_type ()
G_DECLARE_FINAL_TYPE (LatexilaTemplatesDefault, latexila_templates_default, LATEXILA, TEMPLATES_DEFAULT, GtkListStore)

LatexilaTemplatesDefault *
              latexila_templates_default_get_instance           (void);

gchar *       latexila_templates_default_get_contents           (LatexilaTemplatesDefault *templates,
                                                                 GtkTreePath              *path);

G_END_DECLS

#endif /* LATEXILA_TEMPLATES_DEFAULT_H */
