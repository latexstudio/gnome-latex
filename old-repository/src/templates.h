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

#ifndef TEMPLATES_H
#define TEMPLATES_H

void cb_new (void);
void cb_create_template (void);
void cb_delete_template (void);
void init_templates (void);

enum templates
{
	COL_TEMPLATE_PIXBUF,
	COL_TEMPLATE_ICON_ID,
	COL_TEMPLATE_NAME,
	COL_TEMPLATE_CONTENTS,
	N_COLS_TEMPLATE
};

#endif /* TEMPLATES_H */
