/*
 * This file is part of LaTeXila.
 *
 * Copyright © 2009, 2010 Sébastien Wilmet
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

#ifndef EXTERNAL_COMMANDS_H
#define EXTERNAL_COMMANDS_H

void run_compilation (gchar *command, gchar *title);
void view_current_document (gchar *title, gchar *doc_extension);
void view_document (gchar *title, gchar *filename);
void convert_document (gchar *title, gchar *doc_extension, gchar *command);
void run_bibtex (void);
void run_makeindex (void);
void view_in_web_browser (gchar *title, gchar *filename);
void stop_execution (void);

enum output
{
	OUTPUT_GO_FETCHING,
	OUTPUT_IS_FETCHING,
	OUTPUT_STOP_REQUEST
};

#endif /* EXTERNAL_COMMANDS_H */
