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

#ifndef LATEX_OUTPUT_FILTER_H
#define LATEX_OUTPUT_FILTER_H

#include "log.h"

void latex_output_filter (const gchar *line);
void latex_output_filter_init (void);
void latex_output_filter_free (void);
void latex_output_filter_set_path (const gchar *dir);
void latex_output_filter_print_stats (void);

enum filter_status
{
	START,
	BADBOX,
	WARNING,
	ERROR,
	ERROR_SEARCH_LINE,
	FILENAME,
	FILENAME_HEURISTIC
};

typedef struct
{
	gchar *filename;
	gboolean reliable;
} file_in_stack_t;

enum message_type
{
	TYPE_BADBOX,
	TYPE_WARNING,
	TYPE_ERROR,
	TYPE_OTHER
};

typedef struct
{
	enum output_message_type message_type;
	gchar *message;
	gint line;
} message_t;

#define BUFFER_SIZE 1024

#endif /* LATEX_OUTPUT_FILTER_H */
