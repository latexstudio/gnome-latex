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

#ifndef LATEXILA_LATEX_COMMANDS_H
#define LATEXILA_LATEX_COMMANDS_H

#include <tepl/tepl.h>

G_BEGIN_DECLS

void      latexila_latex_commands_add_action_infos  (GtkApplication *gtk_app);

void      latexila_latex_commands_add_actions       (GtkApplicationWindow *gtk_window);

void      latexila_latex_commands_insert_text       (TeplApplicationWindow *tepl_window,
                                                     const gchar           *text_before,
                                                     const gchar           *text_after,
                                                     const gchar           *text_if_no_selection);

G_END_DECLS

#endif /* LATEXILA_LATEX_COMMANDS_H */
