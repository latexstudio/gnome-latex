/*
 * This file is part of LaTeXila.
 *
 * Copyright © 2010 Sébastien Wilmet
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

#include "main.h"
#include "utils.h"

void
flush_queue (void)
{
	while (g_main_context_pending (NULL))
		g_main_context_iteration (NULL, TRUE);
}

void
scroll_to_cursor (void)
{
	if (latexila.active_doc == NULL)
		return;

	gtk_text_view_scroll_to_mark (
			GTK_TEXT_VIEW (latexila.active_doc->source_view),
			gtk_text_buffer_get_insert (
				GTK_TEXT_BUFFER (latexila.active_doc->source_buffer)),
			0.25, FALSE, 0, 0);
}
