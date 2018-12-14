/*
 * This file is part of LaTeXila.
 *
 * Copyright © 2009 Sébastien Wilmet
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

#ifndef SYMBOLS_H
#define SYMBOLS_H

void init_symbols (void);

// for the symbol tables
struct symbol
{
	gchar *filename;
	gchar *latex_command;
	gchar *package_required;
};

// for the symbol lists
enum symbols
{
	COLUMN_SYMBOL_PIXBUF,
	COLUMN_SYMBOL_COMMAND,
	COLUMN_SYMBOL_TOOLTIP,
	N_COLUMNS_SYMBOL
};

// for the categories
enum category_symbols
{
	COLUMN_CAT_ICON,
	COLUMN_CAT_NAME,
	COLUMN_CAT_NUM,
	N_COLUMNS_CAT
};

#endif /* SYMBOLS_H */
