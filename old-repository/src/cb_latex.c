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

#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>

#include "main.h"
#include "config.h"
#include "print.h"
#include "cb_latex.h"

static void text_buffer_insert (gchar *text_before, gchar *text_after,
		gchar *text_if_no_selection);

static void
text_buffer_insert (gchar *text_before, gchar *text_after,
		gchar *text_if_no_selection)
{
	if (latexila.active_doc == NULL)
		return;

	// we do not use the insert and selection_bound marks because the we don't
	// know the order. With gtk_text_buffer_get_selection_bounds, we are certain
	// that "start" points to the start of the selection, where we must insert
	// "text_before".

	GtkTextBuffer *buffer = GTK_TEXT_BUFFER (latexila.active_doc->source_buffer);
	GtkTextIter start, end;
	gboolean text_selected = gtk_text_buffer_get_selection_bounds (buffer,
			&start, &end);

	gtk_text_buffer_begin_user_action (buffer);

	// insert around the selected text
	// move the cursor to the end
	if (text_selected)
	{
		GtkTextMark *mark_end = gtk_text_buffer_create_mark (buffer, NULL,
				&end, FALSE);
		gtk_text_buffer_insert (buffer, &start, text_before, -1);
		gtk_text_buffer_get_iter_at_mark (buffer, &end, mark_end);
		gtk_text_buffer_insert (buffer, &end, text_after, -1);

		gtk_text_buffer_get_iter_at_mark (buffer, &end, mark_end);
		gtk_text_buffer_select_range (buffer, &end, &end);
	}

	// no selection
	else if (text_if_no_selection != NULL)
		gtk_text_buffer_insert_at_cursor (buffer, text_if_no_selection, -1);

	// no selection
	// move the cursor between the 2 texts inserted
	else
	{
		gtk_text_buffer_insert_at_cursor (buffer, text_before, -1);

		GtkTextIter between;
		gtk_text_buffer_get_iter_at_mark (buffer, &between,
				gtk_text_buffer_get_insert (buffer));
		GtkTextMark *mark = gtk_text_buffer_create_mark (buffer, NULL,
				&between, TRUE);

		gtk_text_buffer_insert_at_cursor (buffer, text_after, -1);

		gtk_text_buffer_get_iter_at_mark (buffer, &between, mark);
		gtk_text_buffer_select_range (buffer, &between, &between);
	}

	gtk_text_buffer_end_user_action (buffer);
}

/* sectioning */

void
cb_sectioning_part (void)
{
	text_buffer_insert ("\\part{", "}", NULL);
}

void
cb_sectioning_chapter (void)
{
	text_buffer_insert ("\\chapter{", "}", NULL);
}

void
cb_sectioning_section (void)
{
	text_buffer_insert ("\\section{", "}", NULL);
}

void
cb_sectioning_subsection (void)
{
	text_buffer_insert ("\\subsection{", "}", NULL);
}

void
cb_sectioning_subsubsection (void)
{
	text_buffer_insert ("\\subsubsection{", "}", NULL);
}

void
cb_sectioning_paragraph (void)
{
	text_buffer_insert ("\\paragraph{", "}", NULL);
}

void
cb_sectioning_subparagraph (void)
{
	text_buffer_insert ("\\subparagraph{", "}", NULL);
}

/* References */

void
cb_ref_label (void)
{
	text_buffer_insert ("\\label{", "} ", NULL);
}

void
cb_ref_ref (void)
{
	text_buffer_insert ("\\ref{", "} ", NULL);
}

void
cb_ref_pageref (void)
{
	text_buffer_insert ("\\pageref{", "} ", NULL);
}

void
cb_ref_index (void)
{
	text_buffer_insert ("\\index{", "} ", NULL);
}

void
cb_ref_footnote (void)
{
	text_buffer_insert ("\\footnote{", "} ", NULL);
}

void
cb_ref_cite (void)
{
	text_buffer_insert ("\\cite{", "} ", NULL);
}

/* environments */

void
cb_env_center (void)
{
	text_buffer_insert ("\\begin{center}\n", "\n\\end{center}", NULL);
}

void
cb_env_left (void)
{
	text_buffer_insert ("\\begin{flushleft}\n", "\n\\end{flushleft}", NULL);
}

void
cb_env_right (void)
{
	text_buffer_insert ("\\begin{flushright}\n", "\n\\end{flushright}", NULL);
}

void
cb_env_minipage (void)
{
	text_buffer_insert ("\\begin{minipage}\n", "\n\\end{minipage}", NULL);
}

void
cb_env_quote (void)
{
	text_buffer_insert ("\\begin{quote}\n", "\n\\end{quote}", NULL);
}

void
cb_env_quotation (void)
{
	text_buffer_insert ("\\begin{quotation}\n", "\n\\end{quotation}", NULL);
}

void
cb_env_verse (void)
{
	text_buffer_insert ("\\begin{verse}\n", "\n\\end{verse}", NULL);
}

/* List Environments */

void
cb_list_env_itemize (void)
{
	text_buffer_insert ("\\begin{itemize}\n  \\item ", "\n\\end{itemize}",
			NULL);
}

void
cb_list_env_enumerate (void)
{
	text_buffer_insert ("\\begin{enumerate}\n  \\item ", "\n\\end{enumerate}",
			NULL);
}

void
cb_list_env_description (void)
{
	text_buffer_insert ("\\begin{description}\n  \\item[",
			"] \n\\end{description}", NULL);
}

void
cb_list_env_item (void)
{
	text_buffer_insert ("\\item ", "", NULL);
}


/* Characters sizes */

void
cb_size_tiny (void)
{
	text_buffer_insert ("{\\tiny ", "}", "\\tiny ");
}

void
cb_size_scriptsize (void)
{
	text_buffer_insert ("{\\scriptsize ", "}", "\\scriptsize ");
}

void
cb_size_footnotesize (void)
{
	text_buffer_insert ("{\\footnotesize ", "}", "\\footnotesize ");
}

void
cb_size_small (void)
{
	text_buffer_insert ("{\\small ", "}", "\\small ");
}

void
cb_size_normalsize (void)
{
	text_buffer_insert ("{\\normalsize ", "}", "\\normalsize ");
}

void
cb_size_large (void)
{
	text_buffer_insert ("{\\large ", "}", "\\large ");
}

void
cb_size_Large (void)
{
	text_buffer_insert ("{\\Large ", "}", "\\Large ");
}

void
cb_size_LARGE (void)
{
	text_buffer_insert ("{\\LARGE ", "}", "\\LARGE ");
}

void
cb_size_huge (void)
{
	text_buffer_insert ("{\\huge ", "}", "\\huge ");
}

void
cb_size_Huge (void)
{
	text_buffer_insert ("{\\Huge ", "}", "\\Huge ");
}

/* Font styles */

void
cb_text_bold (void)
{
	text_buffer_insert ("\\textbf{", "}", NULL);
}

void
cb_text_italic (void)
{
	text_buffer_insert ("\\textit{", "}", NULL);
}

void
cb_text_typewriter (void)
{
	text_buffer_insert ("\\texttt{", "}", NULL);
}

void
cb_text_underline (void)
{
	text_buffer_insert ("\\underline{", "}", NULL);
}

void
cb_text_slanted (void)
{
	text_buffer_insert ("\\textsl{", "}", NULL);
}

void
cb_text_small_caps (void)
{
	text_buffer_insert ("\\textsc{", "}", NULL);
}

void
cb_text_emph (void)
{
	text_buffer_insert ("\\emph{", "}", NULL);
}

void
cb_text_font_family_roman (void)
{
	text_buffer_insert ("{\\rmfamily ", "}", "\\rmfamily ");
}

void
cb_text_font_family_sans_serif (void)
{
	text_buffer_insert ("{\\sffamily ", "}", "\\sffamily ");
}

void
cb_text_font_family_monospace (void)
{
	text_buffer_insert ("{\\ttfamily ", "}", "\\ttfamily ");
}

void
cb_text_font_series_medium (void)
{
	text_buffer_insert ("{\\mdseries ", "}", "\\mdseries ");
}

void
cb_text_font_series_bold (void)
{
	text_buffer_insert ("{\\bfseries ", "}", "\\bfseries ");
}

void
cb_text_font_shape_upright (void)
{
	text_buffer_insert ("{\\upshape ", "}", "\\upshape ");
}

void
cb_text_font_shape_italic (void)
{
	text_buffer_insert ("{\\itshape ", "}", "\\itshape ");
}

void
cb_text_font_shape_slanted (void)
{
	text_buffer_insert ("{\\slshape ", "}", "\\slshape ");
}

void
cb_text_font_shape_small_caps (void)
{
	text_buffer_insert ("{\\scshape ", "}", "\\scshape ");
}

void
cb_math_env_normal (void)
{
	text_buffer_insert ("$ ", " $", NULL);
}

void
cb_math_env_centered (void)
{
	text_buffer_insert ("$$ ", " $$", NULL);
}

void
cb_math_env_numbered (void)
{
	text_buffer_insert ("\\begin{equation}\n", "\n\\end{equation}", NULL);
}

void
cb_math_env_array (void)
{
	text_buffer_insert ("\\begin{align*}\n", "\n\\end{align*}", NULL);
}

void
cb_math_env_numbered_array (void)
{
	text_buffer_insert ("\\begin{align}\n", "\n\\end{align}", NULL);
}

void
cb_math_superscript (void)
{
	text_buffer_insert ("^{", "}", NULL);
}

void
cb_math_subscript (void)
{
	text_buffer_insert ("_{", "}", NULL);
}

void
cb_math_frac (void)
{
	text_buffer_insert ("\\frac{", "}{}", NULL);
}

void
cb_math_square_root (void)
{
	text_buffer_insert ("\\sqrt{", "}", NULL);
}

void
cb_math_nth_root (void)
{
	text_buffer_insert ("\\sqrt[]{", "}", NULL);
}

void
cb_math_left_delimiter_1 (void)
{
	text_buffer_insert ("\\left( ", "", NULL);
}

void
cb_math_left_delimiter_2 (void)
{
	text_buffer_insert ("\\left[ ", "", NULL);
}

void
cb_math_left_delimiter_3 (void)
{
	text_buffer_insert ("\\left\\lbrace ", "", NULL);
}

void
cb_math_left_delimiter_4 (void)
{
	text_buffer_insert ("\\left\\langle ", "", NULL);
}

void
cb_math_left_delimiter_5 (void)
{
	text_buffer_insert ("\\left) ", "", NULL);
}

void
cb_math_left_delimiter_6 (void)
{
	text_buffer_insert ("\\left] ", "", NULL);
}

void
cb_math_left_delimiter_7 (void)
{
	text_buffer_insert ("\\left\\rbrace ", "", NULL);
}

void
cb_math_left_delimiter_8 (void)
{
	text_buffer_insert ("\\left\\rangle ", "", NULL);
}

void
cb_math_left_delimiter_9 (void)
{
	text_buffer_insert ("\\left. ", "", NULL);
}

void
cb_math_right_delimiter_1 (void)
{
	text_buffer_insert ("\\right( ", "", NULL);
}

void
cb_math_right_delimiter_2 (void)
{
	text_buffer_insert ("\\right[ ", "", NULL);
}

void
cb_math_right_delimiter_3 (void)
{
	text_buffer_insert ("\\right\\rbrace ", "", NULL);
}

void
cb_math_right_delimiter_4 (void)
{
	text_buffer_insert ("\\right\\rangle ", "", NULL);
}

void
cb_math_right_delimiter_5 (void)
{
	text_buffer_insert ("\\right) ", "", NULL);
}

void
cb_math_right_delimiter_6 (void)
{
	text_buffer_insert ("\\right] ", "", NULL);
}

void
cb_math_right_delimiter_7 (void)
{
	text_buffer_insert ("\\right\\lbrace ", "", NULL);
}

void
cb_math_right_delimiter_8 (void)
{
	text_buffer_insert ("\\right\\langle ", "", NULL);
}

void
cb_math_right_delimiter_9 (void)
{
	text_buffer_insert ("\\right. ", "", NULL);
}
