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

#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H

void init_file_browser (GtkWidget *vbox);
void cb_file_browser_refresh (GtkButton *button, gpointer user_data);

enum
{
	COL_FILE_BROWSER_PIXBUF,
	COL_FILE_BROWSER_FILE,
	N_COLS_FILE_BROWSER
};

#endif /* FILE_BROWSER_H */
