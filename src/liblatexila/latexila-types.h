/*
 * This file is part of GNOME LaTeX.
 *
 * Copyright (C) 2014-2015 - Sébastien Wilmet <swilmet@gnome.org>
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

/* Every type is defined here. It avoids problems with header #includes cycles.
 * For example when type-a.h needs the TypeB declaration, and type-b.h needs the
 * TypeA declaration.
 */

#ifndef LATEXILA_TYPES_H
#define LATEXILA_TYPES_H

#include <glib.h>

G_BEGIN_DECLS

typedef struct _LatexilaBuildJob                LatexilaBuildJob;
typedef struct _LatexilaBuildTool               LatexilaBuildTool;
typedef struct _LatexilaBuildTools              LatexilaBuildTools;
typedef struct _LatexilaBuildToolsDefault       LatexilaBuildToolsDefault;
typedef struct _LatexilaBuildToolsPersonal      LatexilaBuildToolsPersonal;
typedef struct _LatexilaBuildView               LatexilaBuildView;
typedef struct _LatexilaPostProcessor           LatexilaPostProcessor;
typedef struct _LatexilaPostProcessorAllOutput  LatexilaPostProcessorAllOutput;
typedef struct _LatexilaPostProcessorLatex      LatexilaPostProcessorLatex;
typedef struct _LatexilaPostProcessorLatexmk    LatexilaPostProcessorLatexmk;
typedef struct _LatexilaSynctex                 LatexilaSynctex;
typedef struct _LatexilaTemplatesDefault        LatexilaTemplatesDefault;
typedef struct _LatexilaTemplatesPersonal       LatexilaTemplatesPersonal;

G_END_DECLS

#endif /* LATEXILA_TYPES_H */
