/*
 * This file is part of GNOME LaTeX.
 *
 * Copyright (C) 2014 - Sébastien Wilmet <swilmet@gnome.org>
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

#ifndef LATEXILA_POST_PROCESSOR_LATEX_H
#define LATEXILA_POST_PROCESSOR_LATEX_H

#include <glib-object.h>
#include "latexila-post-processor.h"
#include "latexila-types.h"

G_BEGIN_DECLS

#define LATEXILA_TYPE_POST_PROCESSOR_LATEX             (latexila_post_processor_latex_get_type ())
#define LATEXILA_POST_PROCESSOR_LATEX(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), LATEXILA_TYPE_POST_PROCESSOR_LATEX, LatexilaPostProcessorLatex))
#define LATEXILA_POST_PROCESSOR_LATEX_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), LATEXILA_TYPE_POST_PROCESSOR_LATEX, LatexilaPostProcessorLatexClass))
#define LATEXILA_IS_POST_PROCESSOR_LATEX(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LATEXILA_TYPE_POST_PROCESSOR_LATEX))
#define LATEXILA_IS_POST_PROCESSOR_LATEX_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), LATEXILA_TYPE_POST_PROCESSOR_LATEX))
#define LATEXILA_POST_PROCESSOR_LATEX_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), LATEXILA_TYPE_POST_PROCESSOR_LATEX, LatexilaPostProcessorLatexClass))

typedef struct _LatexilaPostProcessorLatexClass   LatexilaPostProcessorLatexClass;
typedef struct _LatexilaPostProcessorLatexPrivate LatexilaPostProcessorLatexPrivate;

struct _LatexilaPostProcessorLatex
{
  LatexilaPostProcessor parent;

  LatexilaPostProcessorLatexPrivate *priv;
};

struct _LatexilaPostProcessorLatexClass
{
  LatexilaPostProcessorClass parent_class;
};

GType                   latexila_post_processor_latex_get_type          (void);

LatexilaPostProcessor * latexila_post_processor_latex_new               (void);

gint                    latexila_post_processor_latex_get_errors_count  (LatexilaPostProcessorLatex *pp);

G_END_DECLS

#endif /* LATEXILA_POST_PROCESSOR_LATEX_H */
