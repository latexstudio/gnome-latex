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

#ifndef CONFIG_H
#define CONFIG_H

#define PROGRAM_NAME "LaTeXila"
#define PROGRAM_VERSION "@latexila_VERSION@"

#cmakedefine LATEXILA_NLS_ENABLED
#cmakedefine LATEXILA_NLS_PACKAGE "@LATEXILA_NLS_PACKAGE@"
#cmakedefine LATEXILA_NLS_LOCALEDIR "@LATEXILA_NLS_LOCALEDIR@"

#define COMMAND_LATEX "@COMMAND_LATEX@"
#define COMMAND_PDFLATEX "@COMMAND_PDFLATEX@"
#define COMMAND_DVIPDF "@COMMAND_DVIPDF@"
#define COMMAND_DVIPS "@COMMAND_DVIPS@"

#define DATA_DIR "${DATA_DIR}"

#define GTKSOURCEVIEW_VERSION "@GTKSOURCEVIEW_VERSION@"

#endif /* CONFIG_H */
