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

#ifndef LATEXILA_TEMPLATES_PERSONAL_H
#define LATEXILA_TEMPLATES_PERSONAL_H

#include <gtk/gtk.h>
#include "latexila-types.h"

G_BEGIN_DECLS

#define LATEXILA_TYPE_TEMPLATES_PERSONAL latexila_templates_personal_get_type ()
G_DECLARE_FINAL_TYPE (LatexilaTemplatesPersonal, latexila_templates_personal, LATEXILA, TEMPLATES_PERSONAL, GtkListStore)

LatexilaTemplatesPersonal *
              latexila_templates_personal_get_instance          (void);

gchar *       latexila_templates_personal_get_contents          (LatexilaTemplatesPersonal *templates,
                                                                 GtkTreePath               *path);

gboolean      latexila_templates_personal_create                (LatexilaTemplatesPersonal *templates,
                                                                 const gchar               *name,
                                                                 const gchar               *config_icon_name,
                                                                 const gchar               *contents,
                                                                 GError                   **error);

gboolean      latexila_templates_personal_delete                (LatexilaTemplatesPersonal  *templates,
                                                                 GtkTreeIter                *iter,
                                                                 GError                    **error);

gboolean      latexila_templates_personal_move_up               (LatexilaTemplatesPersonal  *templates,
                                                                 GtkTreeIter                *iter,
                                                                 GError                    **error);

gboolean      latexila_templates_personal_move_down             (LatexilaTemplatesPersonal  *templates,
                                                                 GtkTreeIter                *iter,
                                                                 GError                    **error);

G_END_DECLS

#endif /* LATEXILA_TEMPLATES_PERSONAL_H */
