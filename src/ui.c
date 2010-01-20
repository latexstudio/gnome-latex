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
#include "callbacks.h"
#include "cb_latex.h"
#include "tool_menu_action.h"
#include "prefs.h"
#include "templates.h"

static void register_my_stock_icons (void);

static struct {
	gchar *filename;
	gchar *stock_id;
} stock_icons[] = {
	{DATA_DIR "/images/icons/compile_latex.png", "compile_latex"},
	{DATA_DIR "/images/icons/compile_pdflatex.png", "compile_pdflatex"},
	{DATA_DIR "/images/icons/view_dvi.png", "view_dvi"},
	{DATA_DIR "/images/icons/view_pdf.png", "view_pdf"},
	{DATA_DIR "/images/icons/view_ps.png", "view_ps"},
	{DATA_DIR "/images/icons/dvi_to_pdf.png", "dvi_to_pdf"},
	{DATA_DIR "/images/icons/dvi_to_ps.png", "dvi_to_ps"},
	{DATA_DIR "/images/icons/textbf.png", "bold"},
	{DATA_DIR "/images/icons/textit.png", "italic"},
	{DATA_DIR "/images/icons/texttt.png", "typewriter"},
	{DATA_DIR "/images/icons/underline.png", "underline"},
	{DATA_DIR "/images/icons/sectioning-part.png", "sectioning-part"},
	{DATA_DIR "/images/icons/sectioning-chapter.png", "sectioning-chapter"},
	{DATA_DIR "/images/icons/sectioning-section.png", "sectioning-section"},
	{DATA_DIR "/images/icons/sectioning-subsection.png", "sectioning-subsection"},
	{DATA_DIR "/images/icons/sectioning-subsubsection.png", "sectioning-subsubsection"},
	{DATA_DIR "/images/icons/sectioning-paragraph.png", "sectioning-paragraph"},
	{DATA_DIR "/images/icons/character-size.png", "character-size"},
	{DATA_DIR "/images/icons/list-itemize.png", "list-itemize"},
	{DATA_DIR "/images/icons/list-enumerate.png", "list-enumerate"},
	{DATA_DIR "/images/icons/list-description.png", "list-description"},
	{DATA_DIR "/images/icons/list-item.png", "list-item"},
	{DATA_DIR "/images/icons/references.png", "references"},
	{DATA_DIR "/images/icons/math.png", "math"},
	{DATA_DIR "/images/icons/math-centered.png", "math-centered"},
	{DATA_DIR "/images/icons/math-numbered.png", "math-numbered"},
	{DATA_DIR "/images/icons/math-array.png", "math-array"},
	{DATA_DIR "/images/icons/math-numbered-array.png", "math-numbered-array"},
	{DATA_DIR "/images/icons/math-superscript.png", "math-superscript"},
	{DATA_DIR "/images/icons/math-subscript.png", "math-subscript"},
	{DATA_DIR "/images/icons/math-frac.png", "math-frac"},
	{DATA_DIR "/images/icons/math-square-root.png", "math-square-root"},
	{DATA_DIR "/images/icons/math-nth-root.png", "math-nth-root"},
	{DATA_DIR "/images/icons/delimiters-left.png", "delimiters-left"},
	{DATA_DIR "/images/icons/delimiters-right.png", "delimiters-right"},
};

static const char *ui =
"<ui>"
"  <menubar name='MainMenu'>"
"    <menu action='File'>"
"      <menuitem action='FileNew' />"
"      <menuitem action='FileOpen' />"
"      <menuitem action='FileOpenRecent' />"
"      <separator />"
"      <menuitem action='FileSave' />"
"      <menuitem action='FileSaveAs' />"
"      <separator />"
"      <menuitem action='FileCreateTemplate' />"
"      <menuitem action='FileDeleteTemplate' />"
"      <separator />"
"      <menuitem action='FileClose' />"
"      <menuitem action='FileQuit' />"
"    </menu>"

"    <menu name='Edit' action='Edit'>"
"      <menuitem name='Undo' action='EditUndo' />"
"      <menuitem name='Redo' action='EditRedo' />"
"      <separator />"
"      <menuitem action='EditCut' />"
"      <menuitem action='EditCopy' />"
"      <menuitem action='EditPaste' />"
"      <menuitem action='EditDelete' />"
"      <separator />"
"      <menuitem action='EditSelectAll' />"
"      <menuitem action='EditPreferences' />"
"    </menu>"

"    <menu action='View'>"
"      <menuitem action='ViewSidePane' />"
"      <menuitem action='ViewEditToolbar' />"
"      <separator />"
"      <menuitem action='ViewZoomIn' />"
"      <menuitem action='ViewZoomOut' />"
"      <menuitem action='ViewZoomReset' />"
"    </menu>"

"    <menu action='Search'>"
"      <menuitem action='SearchFind' />"
"      <menuitem action='SearchReplace' />"
"      <separator />"
"      <menuitem action='SearchGoToLine' />"
"    </menu>"

"    <menu action='Build'>"
"      <menuitem action='compile_latex' />"
"      <menuitem action='viewDVI' />"
"      <separator />"
"      <menuitem action='compile_pdflatex' />"
"      <menuitem action='viewPDF' />"
"      <separator />"
"      <menuitem action='DVItoPDF' />"
"      <menuitem action='DVItoPS' />"
"      <menuitem action='viewPS' />"
"      <separator />"
"      <menuitem action='compile_bibtex' />"
"      <menuitem action='compile_makeindex' />"
"      <separator />"
"      <menuitem action='clean_up_build_files' />"
"      <menuitem action='stop_execution' />"
"    </menu>"

"    <menu action='Latex'>"
"      <menu action='Sectioning'>"
"        <menuitem action='SectioningPart' />"
"        <menuitem action='SectioningChapter' />"
"        <separator />"
"        <menuitem action='SectioningSection' />"
"        <menuitem action='SectioningSubsection' />"
"        <menuitem action='SectioningSubsubsection' />"
"        <menuitem action='SectioningParagraph' />"
"        <menuitem action='SectioningSubparagraph' />"
"      </menu>"

"      <menu action='References'>"
"        <menuitem action='ReferencesLabel' />"
"        <menuitem action='ReferencesRef' />"
"        <menuitem action='ReferencesPageref' />"
"        <separator />"
"        <menuitem action='ReferencesIndex' />"
"        <menuitem action='ReferencesFootnote' />"
"        <separator />"
"        <menuitem action='ReferencesCite' />"
"      </menu>"

"      <menu action='Environments'>"
"        <menuitem action='EnvironmentCenter' />"
"        <menuitem action='EnvironmentLeft' />"
"        <menuitem action='EnvironmentRight' />"
"        <separator />"
"        <menuitem action='EnvironmentMinipage' />"
"        <separator />"
"        <menuitem action='EnvironmentQuote' />"
"        <menuitem action='EnvironmentQuotation' />"
"        <menuitem action='EnvironmentVerse' />"
"      </menu>"

"      <menu action='ListEnvironments'>"
"        <menuitem action='ListEnvItemize' />"
"        <menuitem action='ListEnvEnumerate' />"
"        <menuitem action='ListEnvDescription' />"
"        <separator />"
"        <menuitem action='ListEnvItem' />"
"      </menu>"

"      <menu action='CharacterSize'>"
"        <menuitem action='CharacterSizeTiny' />"
"        <menuitem action='CharacterSizeScriptsize' />"
"        <menuitem action='CharacterSizeFootnotesize' />"
"        <menuitem action='CharacterSizeSmall' />"
"        <menuitem action='CharacterSizeNormalsize' />"
"        <menuitem action='CharacterSizelarge' />"
"        <menuitem action='CharacterSizeLarge' />"
"        <menuitem action='CharacterSizeLARGE' />"
"        <menuitem action='CharacterSizehuge' />"
"        <menuitem action='CharacterSizeHuge' />"
"      </menu>"

"      <menu action='FontStyles'>"
"        <menuitem action='Bold' />"
"        <menuitem action='Italic' />"
"        <menuitem action='Typewriter' />"
"        <menuitem action='Slanted' />"
"        <menuitem action='SmallCaps' />"
"        <separator />"
"        <menuitem action='Emph' />"
"        <separator />"
"        <menuitem action='Underline' />"
"        <separator />"
"        <menu action='FontFamily'>"
"          <menuitem action='FontFamilyRoman' />"
"          <menuitem action='FontFamilySansSerif' />"
"          <menuitem action='FontFamilyMonospace' />"
"        </menu>"
"        <menu action='FontSeries'>"
"          <menuitem action='FontSeriesMedium' />"
"          <menuitem action='FontSeriesBold' />"
"        </menu>"
"        <menu action='FontShape'>"
"          <menuitem action='FontShapeUpright' />"
"          <menuitem action='FontShapeItalic' />"
"          <menuitem action='FontShapeSlanted' />"
"          <menuitem action='FontShapeSmallCaps' />"
"        </menu>"
"      </menu>"

"      <menu action='Math'>"
"        <menu action='MathEnvironments'>"
"          <menuitem action='MathEnvNormal' />"
"          <menuitem action='MathEnvCentered' />"
"          <menuitem action='MathEnvNumbered' />"
"          <menuitem action='MathEnvArray' />"
"          <menuitem action='MathEnvNumberedArray' />"
"        </menu>"
"        <separator />"
"        <menuitem action='MathSuperscript' />"
"        <menuitem action='MathSubscript' />"
"        <separator />"
"        <menuitem action='MathFrac' />"
"        <menuitem action='MathSquareRoot' />"
"        <menuitem action='MathNthRoot' />"
"        <separator />"
"        <menu action='MathLeftDelimiters'>"
"          <menuitem action='MathLeftDelimiter1' />"
"          <menuitem action='MathLeftDelimiter2' />"
"          <menuitem action='MathLeftDelimiter3' />"
"          <menuitem action='MathLeftDelimiter4' />"
"          <menuitem action='MathLeftDelimiter5' />"
"          <menuitem action='MathLeftDelimiter6' />"
"          <menuitem action='MathLeftDelimiter7' />"
"          <menuitem action='MathLeftDelimiter8' />"
"          <menuitem action='MathLeftDelimiter9' />"
"        </menu>"
"        <menu action='MathRightDelimiters'>"
"          <menuitem action='MathRightDelimiter1' />"
"          <menuitem action='MathRightDelimiter2' />"
"          <menuitem action='MathRightDelimiter3' />"
"          <menuitem action='MathRightDelimiter4' />"
"          <menuitem action='MathRightDelimiter5' />"
"          <menuitem action='MathRightDelimiter6' />"
"          <menuitem action='MathRightDelimiter7' />"
"          <menuitem action='MathRightDelimiter8' />"
"          <menuitem action='MathRightDelimiter9' />"
"        </menu>"
"      </menu>"
"    </menu>"

"    <menu action='Tools'>"
"      <menuitem action='ToolsComment' />"
"      <menuitem action='ToolsUncomment' />"
"      <separator />"
"      <menuitem action='ToolsIndent' />"
"      <menuitem action='ToolsUnindent' />"
"    </menu>"

"    <menu action='Documents'>"
"      <menuitem action='DocumentsSaveAll' />"
"      <menuitem action='DocumentsCloseAll' />"
"      <separator />"
"      <menuitem action='DocumentsPrevious' />"
"      <menuitem action='DocumentsNext' />"
"    </menu>"

"    <menu action='Help'>"
"      <menuitem action='HelpLatexReference' />"
"      <menuitem action='HelpAbout' />"
"    </menu>"
"  </menubar>"

"  <toolbar name='MainToolbar'>"
"    <toolitem action='FileNew' />"
"    <toolitem action='FileOpen' />"
"    <!-- TODO add FileOpenRecent with the arrow only -->"
"    <toolitem action='FileSave' />"
"    <separator />"
"    <toolitem action='EditUndo' />"
"    <toolitem action='EditRedo' />"
"    <separator />"
"    <toolitem action='EditCut' />"
"    <toolitem action='EditCopy' />"
"    <toolitem action='EditPaste' />"
"    <separator />"
"    <toolitem action='SearchFind' />"
"    <toolitem action='SearchReplace' />"
"    <separator />"
"    <toolitem action='compile_latex' />"
"    <toolitem action='viewDVI' />"
"    <separator />"
"    <toolitem action='compile_pdflatex' />"
"    <toolitem action='viewPDF' />"
"    <separator />"
"    <toolitem action='stop_execution' />"
"  </toolbar>"

"  <toolbar name='EditToolbar'>"
"    <toolitem action='SectioningToolItem'>"
"      <menu action='Sectioning'>"
"        <menuitem action='SectioningPart' />"
"        <menuitem action='SectioningChapter' />"
"        <separator />"
"        <menuitem action='SectioningSection' />"
"        <menuitem action='SectioningSubsection' />"
"        <menuitem action='SectioningSubsubsection' />"
"        <menuitem action='SectioningParagraph' />"
"        <menuitem action='SectioningSubparagraph' />"
"      </menu>"
"    </toolitem>"

"    <toolitem action='ReferencesToolItem'>"
"      <menu action='References'>"
"        <menuitem action='ReferencesLabel' />"
"        <menuitem action='ReferencesRef' />"
"        <menuitem action='ReferencesPageref' />"
"        <menuitem action='ReferencesIndex' />"
"        <menuitem action='ReferencesFootnote' />"
"        <menuitem action='ReferencesCite' />"
"      </menu>"
"    </toolitem>"

"    <toolitem action='CharacterSizeToolItem'>"
"      <menu action='CharacterSize'>"
"        <menuitem action='CharacterSizeTiny' />"
"        <menuitem action='CharacterSizeScriptsize' />"
"        <menuitem action='CharacterSizeFootnotesize' />"
"        <menuitem action='CharacterSizeSmall' />"
"        <menuitem action='CharacterSizeNormalsize' />"
"        <menuitem action='CharacterSizelarge' />"
"        <menuitem action='CharacterSizeLarge' />"
"        <menuitem action='CharacterSizeLARGE' />"
"        <menuitem action='CharacterSizehuge' />"
"        <menuitem action='CharacterSizeHuge' />"
"      </menu>"
"    </toolitem>"

"    <separator />"

"    <toolitem action='Bold' />"
"    <toolitem action='Italic' />"
"    <toolitem action='Typewriter' />"
"    <toolitem action='Underline' />"

"    <separator />"

"    <toolitem action='EnvironmentCenter' />"

"    <separator />"

"    <toolitem action='ListEnvItemize' />"
"    <toolitem action='ListEnvEnumerate' />"
"    <toolitem action='ListEnvDescription' />"
"    <toolitem action='ListEnvItem' />"

"    <separator />"

"    <toolitem action='MathEnvironmentsToolItem'>"
"      <menu action='MathEnvironments'>"
"        <menuitem action='MathEnvNormal' />"
"        <menuitem action='MathEnvCentered' />"
"        <menuitem action='MathEnvNumbered' />"
"        <menuitem action='MathEnvArray' />"
"        <menuitem action='MathEnvNumberedArray' />"
"      </menu>"
"    </toolitem>"
"    <separator />"
"    <toolitem action='MathSuperscript' />"
"    <toolitem action='MathSubscript' />"
"    <separator />"
"    <toolitem action='MathFrac' />"
"    <toolitem action='MathSquareRoot' />"
"    <separator />"
"    <toolitem action='MathLeftDelimitersToolItem'>"
"      <menu action='MathLeftDelimiters'>"
"        <menuitem action='MathLeftDelimiter1' />"
"        <menuitem action='MathLeftDelimiter2' />"
"        <menuitem action='MathLeftDelimiter3' />"
"        <menuitem action='MathLeftDelimiter4' />"
"        <menuitem action='MathLeftDelimiter5' />"
"        <menuitem action='MathLeftDelimiter6' />"
"        <menuitem action='MathLeftDelimiter7' />"
"        <menuitem action='MathLeftDelimiter8' />"
"        <menuitem action='MathLeftDelimiter9' />"
"      </menu>"
"    </toolitem>"
"    <toolitem action='MathRightDelimitersToolItem'>"
"      <menu action='MathRightDelimiters'>"
"        <menuitem action='MathRightDelimiter1' />"
"        <menuitem action='MathRightDelimiter2' />"
"        <menuitem action='MathRightDelimiter3' />"
"        <menuitem action='MathRightDelimiter4' />"
"        <menuitem action='MathRightDelimiter5' />"
"        <menuitem action='MathRightDelimiter6' />"
"        <menuitem action='MathRightDelimiter7' />"
"        <menuitem action='MathRightDelimiter8' />"
"        <menuitem action='MathRightDelimiter9' />"
"      </menu>"
"    </toolitem>"
"  </toolbar>"
"</ui>";

// all the actions (for the menu and the toolbar)
// {name, stock_id, label, accelerator, tooltip, callback}
// the names come from the XML file
static GtkActionEntry entries[] = {
	{"File", NULL, N_("_File"), NULL, NULL, NULL},
	{"FileNew", GTK_STOCK_NEW, N_("_New"), "<Control>N",
		N_("New file"), G_CALLBACK (cb_new)},
	{"FileOpen", GTK_STOCK_OPEN, N_("_Open..."), "<Control>O",
		N_("Open a file"), G_CALLBACK (cb_open)},
	{"FileSave", GTK_STOCK_SAVE, N_("_Save..."), "<Control>S",
		N_("Save the current file"), G_CALLBACK (cb_save)},
	{"FileSaveAs", GTK_STOCK_SAVE_AS, N_("Save _As..."), "<Shift><Control>S",
		N_("Save the current file with a different name"), G_CALLBACK (cb_save_as)},
	{"FileCreateTemplate", NULL, N_("Create _Template From Document..."), NULL,
		N_("Create a new template from the current document"), G_CALLBACK (cb_create_template)},
	{"FileDeleteTemplate", NULL, N_("_Delete Template..."), NULL,
		N_("Delete personnal template(s)"), G_CALLBACK (cb_delete_template)},
	{"FileClose", GTK_STOCK_CLOSE, N_("_Close"), "<Control>W",
		N_("Close the current file"), G_CALLBACK (cb_close)},
	{"FileQuit", GTK_STOCK_QUIT, N_("_Quit"), "<Control>Q",
		N_("Quit the program"), G_CALLBACK (cb_quit)},
	
	{"Edit", NULL, N_("_Edit"), NULL, NULL, NULL},
	{"EditUndo", GTK_STOCK_UNDO, N_("_Undo"), "<Control>Z",
		N_("Undo the last action"), G_CALLBACK (cb_undo)},
	{"EditRedo", GTK_STOCK_REDO, N_("_Redo"), "<Shift><Control>Z",
		N_("Redo the last undone action"), G_CALLBACK (cb_redo)},
	{"EditCut", GTK_STOCK_CUT, N_("Cu_t"), "<Control>X",
		N_("Cut the selection"), G_CALLBACK (cb_cut)},
	{"EditCopy", GTK_STOCK_COPY, N_("_Copy"), "<Control>C",
		N_("Copy the selection"), G_CALLBACK (cb_copy)},
	{"EditPaste", GTK_STOCK_PASTE, N_("_Paste"), "<Control>V",
		N_("Paste the clipboard"), G_CALLBACK (cb_paste)},
	{"EditDelete", GTK_STOCK_DELETE, N_("_Delete"), NULL,
		N_("Delete the selected text"), G_CALLBACK (cb_delete)},
	{"EditSelectAll", GTK_STOCK_SELECT_ALL, N_("Select _All"), "<Control>A",
		N_("Select the entire document"), G_CALLBACK (cb_select_all)},
	{"EditPreferences", GTK_STOCK_PREFERENCES, N_("Pr_eferences"), NULL,
		N_("Configure the application"), G_CALLBACK (cb_preferences)},

	{"View", NULL, N_("_View"), NULL, NULL, NULL},
	{"ViewZoomIn", GTK_STOCK_ZOOM_IN, N_("Zoom _In"), "<Control>plus",
		N_("Enlarge the font"), G_CALLBACK (cb_zoom_in)},
	{"ViewZoomOut", GTK_STOCK_ZOOM_OUT, N_("Zoom _Out"), "<Control>minus",
		N_("Shrink the font"), G_CALLBACK (cb_zoom_out)},
	{"ViewZoomReset", GTK_STOCK_ZOOM_100, N_("_Reset Zoom"), "<Control>0",
		N_("Reset the size of the font"), G_CALLBACK (cb_zoom_reset)},

	{"Search", NULL, N_("_Search"), NULL, NULL, NULL},
	{"SearchFind", GTK_STOCK_FIND, N_("_Find..."), "<Control>F",
		N_("Search for text"), G_CALLBACK (cb_find)},
	{"SearchReplace", GTK_STOCK_FIND_AND_REPLACE, N_("_Replace..."), "<Control>H",
		N_("Search for and replace text"), G_CALLBACK (cb_replace)},
	{"SearchGoToLine", GTK_STOCK_JUMP_TO, N_("_Go to Line..."), "<Control>G",
		N_("Go to a specific line"), G_CALLBACK (cb_go_to_line)},
	
	{"Build", NULL, N_("_Build"), NULL, NULL, NULL},
	{"compile_latex", "compile_latex", N_("_Compile (latex)"), "<Release>F5",
		N_("Produce the document in DVI format"), G_CALLBACK (cb_latex)},
	{"viewDVI", "view_dvi", N_("_View DVI"), "<Release>F6",
		N_("View the DVI file"), G_CALLBACK (cb_view_dvi)},
	{"compile_pdflatex", "compile_pdflatex", N_("C_ompile (pdflatex)"), "<Release>F7",
		N_("Produce the document in PDF format"), G_CALLBACK (cb_pdflatex)},
	{"viewPDF", "view_pdf", N_("V_iew PDF"), "<Release>F8",
		N_("View the PDF file"), G_CALLBACK (cb_view_pdf)},
	{"DVItoPDF", "dvi_to_pdf", N_("_DVI to PDF"), NULL,
		N_("Convert the DVI document to the PDF format"), G_CALLBACK (cb_dvi_to_pdf)},
	{"DVItoPS", "dvi_to_ps", N_("DVI to _PS"), NULL,
		N_("Convert the DVI document to the PostScript format"), G_CALLBACK (cb_dvi_to_ps)},
	{"viewPS", "view_ps", N_("Vi_ew PS"), NULL,
		N_("View the PostScript file"), G_CALLBACK (cb_view_ps)},
	{"compile_bibtex", NULL, "_BibTeX", NULL,
		N_("Run BibTeX on the current document"), G_CALLBACK (cb_bibtex)},
	{"compile_makeindex", NULL, "_MakeIndex", NULL,
		N_("Run MakeIndex on the current document"), G_CALLBACK (cb_makeindex)},
	{"clean_up_build_files", GTK_STOCK_DELETE, N_("Cleanup Build _Files"), NULL,
		N_("Clean-up build files (*.aux, *.log, *.out, *.toc, etc)"),
		G_CALLBACK (cb_clean_up_build_files)},
	{"stop_execution", GTK_STOCK_STOP, N_("_Stop Execution"), "<Release>F9",
		N_("Stop Execution"), G_CALLBACK (cb_stop_execution)},

	{"Tools", NULL, N_("_Tools"), NULL, NULL, NULL},
	{"ToolsComment", NULL, N_("_Comment"), "<Control>D",
		N_("Comment the selected lines (add the character \"%\")"),
		G_CALLBACK (cb_tools_comment)},
	{"ToolsUncomment", NULL, N_("_Uncomment"), "<Shift><Control>D",
		N_("Uncomment the selected lines (remove the character \"%\")"),
		G_CALLBACK (cb_tools_uncomment)},
	{"ToolsIndent", GTK_STOCK_INDENT, N_("_Indent"), "<Control>I",
		N_("Indent the selected lines"), G_CALLBACK (cb_tools_indent)},
	{"ToolsUnindent", GTK_STOCK_UNINDENT, N_("U_nindent"), "<Shift><Control>I",
		N_("Unindent the selected lines"), G_CALLBACK (cb_tools_unindent)},

	{"Documents", NULL, N_("_Documents"), NULL, NULL, NULL},
	{"DocumentsSaveAll", GTK_STOCK_SAVE, N_("_Save All"), "<Shift><Control>L",
		N_("Save all open files"), G_CALLBACK (cb_documents_save_all)},
	{"DocumentsCloseAll", GTK_STOCK_CLOSE, N_("_Close All"), "<Shift><Control>W",
		N_("Close all open files"), G_CALLBACK (cb_documents_close_all)},
	{"DocumentsPrevious", GTK_STOCK_GO_BACK, N_("_Previous Document"), "<Control><Alt>Page_Up",
		N_("Activate previous document"), G_CALLBACK (cb_documents_previous)},
	{"DocumentsNext", GTK_STOCK_GO_FORWARD, N_("_Next Document"), "<Control><Alt>Page_Down",
		N_("Activate next document"), G_CALLBACK (cb_documents_next)},
	
	{"Help", NULL, N_("_Help"), NULL, NULL, NULL},
	{"HelpLatexReference", GTK_STOCK_HELP, N_("_LaTeX Reference"), "<Release>F1",
		N_("The Kile LaTeX Reference"), G_CALLBACK (cb_help_latex_reference)},
	{"HelpAbout", GTK_STOCK_ABOUT, N_("_About"), NULL,
		N_("About LaTeXila"), G_CALLBACK (cb_about_dialog)}
};

// {name, stock_id, label, accelerator, tooltip, callback}
static GtkActionEntry latex_entries[] = {
	{"Latex", NULL, "_LaTeX", NULL, NULL, NULL},

	{"Sectioning", "sectioning-section", N_("_Sectioning"), NULL, NULL, NULL},
	{"SectioningPart", "sectioning-part", N_("_part"), NULL,
		N_("part"), G_CALLBACK (cb_sectioning_part)},
	{"SectioningChapter", "sectioning-chapter", N_("_chapter"), NULL,
		N_("chapter"), G_CALLBACK (cb_sectioning_chapter)},
	{"SectioningSection", "sectioning-section", N_("_section"), NULL,
		N_("section"), G_CALLBACK (cb_sectioning_section)},
	{"SectioningSubsection", "sectioning-subsection", N_("s_ubsection"), NULL,
		N_("subsection"), G_CALLBACK (cb_sectioning_subsection)},
	{"SectioningSubsubsection", "sectioning-subsubsection", N_("su_bsubsection"), NULL,
		N_("subsubsection"), G_CALLBACK (cb_sectioning_subsubsection)},
	{"SectioningParagraph", "sectioning-paragraph", N_("p_aragraph"), NULL,
		N_("paragraph"), G_CALLBACK (cb_sectioning_paragraph)},
	{"SectioningSubparagraph", "sectioning-paragraph", N_("subpa_ragraph"), NULL,
		N_("subparagraph"), G_CALLBACK (cb_sectioning_subparagraph)},

	{"References", "references", N_("_References"), NULL, NULL, NULL},
	{"ReferencesLabel", NULL, "_label", NULL, "label", G_CALLBACK (cb_ref_label)},
	{"ReferencesRef", NULL, "_ref", NULL, "ref", G_CALLBACK (cb_ref_ref)},
	{"ReferencesPageref", NULL, "_pageref", NULL, "pageref", G_CALLBACK (cb_ref_pageref)},
	{"ReferencesIndex", NULL, "_index", NULL, "index", G_CALLBACK (cb_ref_index)},
	{"ReferencesFootnote", NULL, "_footnote", NULL, "footnote", G_CALLBACK (cb_ref_footnote)},
	{"ReferencesCite", NULL, "_cite", NULL, "cite", G_CALLBACK (cb_ref_cite)},

	{"Environments", GTK_STOCK_JUSTIFY_CENTER, N_("_Environments"), NULL, NULL, NULL},
	{"EnvironmentCenter", GTK_STOCK_JUSTIFY_CENTER, N_("_Center - \\begin{center}"), NULL,
		N_("Center - \\begin{center}"), G_CALLBACK (cb_env_center)},
	{"EnvironmentLeft", GTK_STOCK_JUSTIFY_LEFT, N_("Align _Left - \\begin{flushleft}"), NULL,
		N_("Align Left - \\begin{flushleft}"), G_CALLBACK (cb_env_left)},
	{"EnvironmentRight", GTK_STOCK_JUSTIFY_RIGHT, N_("Align _Right - \\begin{flushright}"), NULL,
		N_("Align Right - \\begin{flushright}"), G_CALLBACK (cb_env_right)},
	{"EnvironmentMinipage", NULL, N_("_Minipage - \\begin{minipage}"), NULL,
		N_("Minipage - \\begin{minipage}"), G_CALLBACK (cb_env_minipage)},
	{"EnvironmentQuote", NULL, N_("_Quote - \\begin{quote}"), NULL,
		N_("Quote - \\begin{quote}"), G_CALLBACK (cb_env_quote)},
	{"EnvironmentQuotation", NULL, N_("Qu_otation - \\begin{quotation}"), NULL,
		N_("Quotation - \\begin{quotation}"), G_CALLBACK (cb_env_quotation)},
	{"EnvironmentVerse", NULL, N_("_Verse - \\begin{verse}"), NULL,
		N_("Verse - \\begin{verse}"), G_CALLBACK (cb_env_verse)},

	{"ListEnvironments", "list-enumerate", N_("_List Environments"), NULL, NULL, NULL},
	{"ListEnvItemize", "list-itemize", N_("_Bulleted List - \\begin{itemize}"), NULL,
		N_("Bulleted List - \\begin{itemize}"), G_CALLBACK (cb_list_env_itemize)},
	{"ListEnvEnumerate", "list-enumerate", N_("_Enumeration - \\begin{enumerate}"), NULL,
		N_("Enumeration - \\begin{enumerate}"), G_CALLBACK (cb_list_env_enumerate)},
	{"ListEnvDescription", "list-description", N_("_Description - \\begin{description}"), NULL,
		N_("Description - \\begin{description}"), G_CALLBACK (cb_list_env_description)},
	{"ListEnvItem", "list-item", "\\_item", NULL,
		"\\item", G_CALLBACK (cb_list_env_item)},
	

	{"CharacterSize", "character-size", N_("_Characters Sizes"), NULL, NULL, NULL},
	{"CharacterSizeTiny", NULL, "_tiny", NULL,
		"\\tiny", G_CALLBACK (cb_size_tiny)},
	{"CharacterSizeScriptsize", NULL, "_scriptsize", NULL,
		"\\scriptsize", G_CALLBACK (cb_size_scriptsize)},
	{"CharacterSizeFootnotesize", NULL, "_footnotesize", NULL,
		"\\footnotesize", G_CALLBACK (cb_size_footnotesize)},
	{"CharacterSizeSmall", NULL, "s_mall", NULL,
		"\\small", G_CALLBACK (cb_size_small)},
	{"CharacterSizeNormalsize", NULL, "_normalsize", NULL,
		"\\normalsize", G_CALLBACK (cb_size_normalsize)},
	{"CharacterSizelarge", NULL, "_large", NULL,
		"\\large", G_CALLBACK (cb_size_large)},
	{"CharacterSizeLarge", NULL, "L_arge", NULL,
		"\\Large", G_CALLBACK (cb_size_Large)},
	{"CharacterSizeLARGE", NULL, "LA_RGE", NULL,
		"\\LARGE", G_CALLBACK (cb_size_LARGE)},
	{"CharacterSizehuge", NULL, "_huge", NULL,
		"\\huge", G_CALLBACK (cb_size_huge)},
	{"CharacterSizeHuge", NULL, "H_uge", NULL,
		"\\Huge", G_CALLBACK (cb_size_Huge)},

	{"FontStyles", "bold", N_("_Font Styles"), NULL, NULL, NULL},
	{"Bold", "bold", N_("_Bold - \\textbf{}"), NULL,
		N_("Bold - \\textbf{}"), G_CALLBACK (cb_text_bold)},
	{"Italic", "italic", N_("_Italic - \\textit{}"), NULL,
		N_("Italic - \\textit{}"), G_CALLBACK (cb_text_italic)},
	{"Typewriter", "typewriter", N_("_Typewriter - \\texttt{}"), NULL,
		N_("Typewriter - \\texttt{}"), G_CALLBACK (cb_text_typewriter)},
	{"Underline", "underline", N_("_Underline - \\underline{}"), NULL,
		N_("Underline - \\underline{}"), G_CALLBACK (cb_text_underline)},
	{"Slanted", NULL, N_("_Slanted - \\textsl{}"), NULL,
		N_("Slanted - \\textsl{}"), G_CALLBACK (cb_text_slanted)},
	{"SmallCaps", NULL, N_("Small _Capitals - \\textsc{}"), NULL,
		N_("Small Capitals - \\textsc{}"), G_CALLBACK (cb_text_small_caps)},
	{"Emph", NULL, N_("_Emphasized - \\emph{}"), NULL,
		N_("Emphasized - \\emph{}"), G_CALLBACK (cb_text_emph)},
	{"FontFamily", NULL, N_("_Font Family"), NULL, NULL, NULL},
	{"FontFamilyRoman", NULL, N_("_Roman - \\rmfamily"), NULL,
		N_("Roman - \\rmfamily"), G_CALLBACK (cb_text_font_family_roman)},
	{"FontFamilySansSerif", NULL, N_("_Sans Serif - \\sffamily"), NULL,
		N_("Sans Serif - \\sffamily"), G_CALLBACK (cb_text_font_family_sans_serif)},
	{"FontFamilyMonospace", NULL, N_("_Monospace - \\ttfamily"), NULL,
		N_("Monospace - \\ttfamily"), G_CALLBACK (cb_text_font_family_monospace)},
	{"FontSeries", NULL, N_("F_ont Series"), NULL, NULL, NULL},
	{"FontSeriesMedium", NULL, N_("_Medium - \\mdseries"), NULL,
		N_("Medium - \\mdseries"), G_CALLBACK (cb_text_font_series_medium)},
	{"FontSeriesBold", NULL, N_("_Bold - \\bfseries"), NULL,
		N_("Bold - \\bfseries"), G_CALLBACK (cb_text_font_series_bold)},
	{"FontShape", NULL, N_("Fo_nt Shape"), NULL, NULL, NULL},
	{"FontShapeUpright", NULL, N_("_Upright - \\upshape"), NULL,
		N_("Upright - \\upshape"), G_CALLBACK (cb_text_font_shape_upright)},
	{"FontShapeItalic", NULL, N_("_Italic - \\itshape"), NULL,
		N_("Italic - \\itshape"), G_CALLBACK (cb_text_font_shape_italic)},
	{"FontShapeSlanted", NULL, N_("_Slanted - \\slshape"), NULL,
		N_("Slanted - \\slshape"), G_CALLBACK (cb_text_font_shape_slanted)},
	{"FontShapeSmallCaps", NULL, N_("Small _Capitals - \\scshape"), NULL,
		N_("Small Capitals - \\scshape"), G_CALLBACK (cb_text_font_shape_small_caps)},

	{"Math", "math", N_("_Math"), NULL, NULL, NULL},
	{"MathEnvironments", NULL, N_("_Math Environments"), NULL, NULL, NULL},
	{"MathEnvNormal", "math", N_("_Mathematical Environment - $...$"), NULL,
		N_("Mathematical Environment - $...$"), G_CALLBACK (cb_math_env_normal)},
	{"MathEnvCentered", "math-centered", N_("_Centered Formula - $$...$$"), NULL,
		N_("Centered Formula - $$...$$"), G_CALLBACK (cb_math_env_centered)},
	{"MathEnvNumbered", "math-numbered", N_("_Numbered Equation - \\begin{equation}"), NULL,
		N_("Numbered Equation - \\begin{equation}"), G_CALLBACK (cb_math_env_numbered)},
	{"MathEnvArray", "math-array", N_("_Array of Equations - \\begin{align*}"), NULL,
		N_("Array of Equations - \\begin{align*}"), G_CALLBACK (cb_math_env_array)},
	{"MathEnvNumberedArray", "math-numbered-array", N_("Numbered Array of _Equations - \\begin{align}"), NULL,
		N_("Numbered Array of Equations - \\begin{align}"), G_CALLBACK (cb_math_env_numbered_array)},
	{"MathSuperscript", "math-superscript", N_("_Superscript - ^{}"), NULL,
		N_("Superscript - ^{}"), G_CALLBACK (cb_math_superscript)},
	{"MathSubscript", "math-subscript", N_("Su_bscript - __{}"), NULL,
		N_("Subscript - _{}"), G_CALLBACK (cb_math_subscript)},
	{"MathFrac", "math-frac", N_("_Fraction - \\frac{}{}"), NULL,
		N_("Fraction - \\frac{}{}"), G_CALLBACK (cb_math_frac)},
	{"MathSquareRoot", "math-square-root", N_("Square _Root - \\sqrt{}"), NULL,
		N_("Square Root - \\sqrt{}"), G_CALLBACK (cb_math_square_root)},
	{"MathNthRoot", "math-nth-root", N_("_N-th Root - \\sqrt[]{}"), NULL,
		N_("N-th Root - \\sqrt[]{}"), G_CALLBACK (cb_math_nth_root)},
	{"MathLeftDelimiters", "delimiters-left", N_("_Left Delimiters"), NULL, NULL, NULL},
	{"MathLeftDelimiter1", NULL, N_("left ("), NULL,
		NULL, G_CALLBACK (cb_math_left_delimiter_1)},
	{"MathLeftDelimiter2", NULL, N_("left ["), NULL,
		NULL, G_CALLBACK (cb_math_left_delimiter_2)},
	{"MathLeftDelimiter3", NULL, N_("left {"), NULL,
		NULL, G_CALLBACK (cb_math_left_delimiter_3)},
	{"MathLeftDelimiter4", NULL, N_("left <"), NULL,
		NULL, G_CALLBACK (cb_math_left_delimiter_4)},
	{"MathLeftDelimiter5", NULL, N_("left )"), NULL,
		NULL, G_CALLBACK (cb_math_left_delimiter_5)},
	{"MathLeftDelimiter6", NULL, N_("left ]"), NULL,
		NULL, G_CALLBACK (cb_math_left_delimiter_6)},
	{"MathLeftDelimiter7", NULL, N_("left }"), NULL,
		NULL, G_CALLBACK (cb_math_left_delimiter_7)},
	{"MathLeftDelimiter8", NULL, N_("left >"), NULL,
		NULL, G_CALLBACK (cb_math_left_delimiter_8)},
	{"MathLeftDelimiter9", NULL, N_("left ."), NULL,
		NULL, G_CALLBACK (cb_math_left_delimiter_9)},
	{"MathRightDelimiters", "delimiters-right", N_("Right _Delimiters"), NULL, NULL, NULL},
	{"MathRightDelimiter1", NULL, N_("right )"), NULL,
		NULL, G_CALLBACK (cb_math_right_delimiter_1)},
	{"MathRightDelimiter2", NULL, N_("right ]"), NULL,
		NULL, G_CALLBACK (cb_math_right_delimiter_2)},
	{"MathRightDelimiter3", NULL, N_("right }"), NULL,
		NULL, G_CALLBACK (cb_math_right_delimiter_3)},
	{"MathRightDelimiter4", NULL, N_("right >"), NULL,
		NULL, G_CALLBACK (cb_math_right_delimiter_4)},
	{"MathRightDelimiter5", NULL, N_("right ("), NULL,
		NULL, G_CALLBACK (cb_math_right_delimiter_5)},
	{"MathRightDelimiter6", NULL, N_("right ["), NULL,
		NULL, G_CALLBACK (cb_math_right_delimiter_6)},
	{"MathRightDelimiter7", NULL, N_("right {"), NULL,
		NULL, G_CALLBACK (cb_math_right_delimiter_7)},
	{"MathRightDelimiter8", NULL, N_("right <"), NULL,
		NULL, G_CALLBACK (cb_math_right_delimiter_8)},
	{"MathRightDelimiter9", NULL, N_("right ."), NULL,
		NULL, G_CALLBACK (cb_math_right_delimiter_9)},
};

// {name, stock_id, label, accelerator, tooltip, callback}
static GtkToggleActionEntry toggle_entries[] = {
	{"ViewSidePane", NULL, N_("_Side pane"), NULL,
		N_("Show or hide the side pane"), G_CALLBACK (cb_show_side_pane)},
	{"ViewEditToolbar", NULL, N_("_Edit Toolbar"), NULL,
		N_("Show or hide the edit toolbar"), G_CALLBACK (cb_show_edit_toolbar)},
};

static guint n_stock_icons = G_N_ELEMENTS (stock_icons);
static guint nb_entries = G_N_ELEMENTS (entries);
static guint nb_latex_entries = G_N_ELEMENTS (latex_entries);
static guint nb_toggle_entries = G_N_ELEMENTS (toggle_entries);

static void
register_my_stock_icons (void)
{
	GtkIconFactory *icon_factory = gtk_icon_factory_new ();

	for (gint i = 0; i < n_stock_icons; i++)
	{
		GtkIconSet *icon_set = gtk_icon_set_new ();
		GtkIconSource *icon_source = gtk_icon_source_new ();
		gtk_icon_source_set_filename (icon_source, stock_icons[i].filename);
		gtk_icon_set_add_source (icon_set, icon_source);
		gtk_icon_source_free (icon_source);
		gtk_icon_factory_add (icon_factory, stock_icons[i].stock_id, icon_set);
		gtk_icon_set_unref (icon_set);
	}

	gtk_icon_factory_add_default (icon_factory);
	g_object_unref (icon_factory);
}

void
init_ui (GtkWidget *box)
{
	GError *error = NULL;

	register_my_stock_icons ();

	// recent document
	GtkAction *recent = gtk_recent_action_new ("FileOpenRecent",
			_("Open _Recent"), _("Open recently used files"), NULL);
	g_signal_connect (G_OBJECT (recent), "item-activated",
			G_CALLBACK (cb_recent_item_activated), NULL);

	GtkRecentFilter *filter = gtk_recent_filter_new ();
	gtk_recent_filter_add_application (filter, "latexila");
	gtk_recent_chooser_add_filter (GTK_RECENT_CHOOSER (recent), filter);
	gtk_recent_chooser_set_sort_type (GTK_RECENT_CHOOSER (recent),
			GTK_RECENT_SORT_MRU);

	// menus under toolitems
	GtkAction *sectioning = tool_menu_action_new ("SectioningToolItem",
			_("Sectioning"), _("Sectioning"), "sectioning-section");
	GtkToolItem *sectioning_menu_tool_button = gtk_menu_tool_button_new (NULL, NULL);
	gtk_activatable_set_related_action (
			GTK_ACTIVATABLE (sectioning_menu_tool_button), sectioning);

	GtkAction *sizes = tool_menu_action_new ("CharacterSizeToolItem",
			_("Characters Sizes"), _("Characters Sizes"), "character-size");
	GtkToolItem *sizes_menu_tool_button = gtk_menu_tool_button_new (NULL, NULL);
	gtk_activatable_set_related_action (
			GTK_ACTIVATABLE (sizes_menu_tool_button), sizes);

	GtkAction *references = tool_menu_action_new ("ReferencesToolItem",
			_("References"), _("References"), "references");
	GtkToolItem *references_menu_tool_button = gtk_menu_tool_button_new (NULL, NULL);
	gtk_activatable_set_related_action (
			GTK_ACTIVATABLE (references_menu_tool_button), references);

	GtkAction *math_env = tool_menu_action_new ("MathEnvironmentsToolItem",
			_("Math Environments"), _("Math Environments"), "math");
	GtkToolItem *math_env_menu_tool_button = gtk_menu_tool_button_new (NULL, NULL);
	gtk_activatable_set_related_action (
			GTK_ACTIVATABLE (math_env_menu_tool_button), math_env);

	GtkAction *math_left_del = tool_menu_action_new ("MathLeftDelimitersToolItem",
			_("Left Delimiters"), _("Left Delimiters"), "delimiters-left");
	GtkToolItem *math_left_del_menu_tool_button = gtk_menu_tool_button_new (NULL, NULL);
	gtk_activatable_set_related_action (
			GTK_ACTIVATABLE (math_left_del_menu_tool_button), math_left_del);

	GtkAction *math_right_del = tool_menu_action_new ("MathRightDelimitersToolItem",
			_("Right Delimiters"), _("Right Delimiters"), "delimiters-right");
	GtkToolItem *math_right_del_menu_tool_button = gtk_menu_tool_button_new (NULL, NULL);
	gtk_activatable_set_related_action (
			GTK_ACTIVATABLE (math_right_del_menu_tool_button), math_right_del);

	// create the action group and the ui manager
	GtkActionGroup *action_group = gtk_action_group_new ("menuActionGroup");
#ifdef LATEXILA_NLS_ENABLED
	gtk_action_group_set_translation_domain (action_group, LATEXILA_NLS_PACKAGE);
#endif
	gtk_action_group_add_action (action_group, sectioning);
	gtk_action_group_add_action (action_group, sizes);
	gtk_action_group_add_action (action_group, references);
	gtk_action_group_add_action (action_group, math_env);
	gtk_action_group_add_action (action_group, math_left_del);
	gtk_action_group_add_action (action_group, math_right_del);
	gtk_action_group_add_actions (action_group, entries, nb_entries, NULL);
	gtk_action_group_add_actions (action_group, latex_entries, nb_latex_entries,
			NULL);
	gtk_action_group_add_toggle_actions (action_group, toggle_entries,
			nb_toggle_entries, NULL);
	gtk_action_group_add_action (action_group, recent);
	GtkUIManager *ui_manager = gtk_ui_manager_new ();
	gtk_ui_manager_insert_action_group (ui_manager, action_group, 0);

	// load the xml file
	gtk_ui_manager_add_ui_from_string (ui_manager, ui, -1, &error);
	if (error != NULL)
	{
		print_error ("building menubar and toolbar failed: %s", error->message);
		g_error_free (error);
		exit (EXIT_FAILURE);
	}

	// get and put the menubar and the toolbars to the main vbox
	GtkWidget *menubar = gtk_ui_manager_get_widget (ui_manager, "/MainMenu");
	gtk_box_pack_start (GTK_BOX (box), menubar, FALSE, FALSE, 0);

	GtkWidget *toolbar = gtk_ui_manager_get_widget (ui_manager, "/MainToolbar");
	GtkWidget *edit_toolbar = gtk_ui_manager_get_widget (ui_manager, "/EditToolbar");
	latexila.edit_toolbar = edit_toolbar;

	// toolbars with icons only
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_ICONS);
	gtk_toolbar_set_style (GTK_TOOLBAR (edit_toolbar), GTK_TOOLBAR_ICONS);

	if (latexila.prefs.toolbars_horizontal)
	{
		GtkWidget *hbox_toolbars = gtk_hbox_new (FALSE, 10);
		gtk_box_pack_start (GTK_BOX (box), hbox_toolbars, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (hbox_toolbars), toolbar, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (hbox_toolbars), edit_toolbar, TRUE, TRUE, 0);
	}
	else
	{
		gtk_box_pack_start (GTK_BOX (box), toolbar, FALSE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box), edit_toolbar, FALSE, FALSE, 0);
	}

	// accelerators
	gtk_window_add_accel_group (latexila.main_window, 
			gtk_ui_manager_get_accel_group (ui_manager));

	// get actions
	latexila.actions.undo = gtk_action_group_get_action (action_group,
			"EditUndo");
	latexila.actions.redo = gtk_action_group_get_action (action_group,
			"EditRedo");
	latexila.actions.compile_latex = gtk_action_group_get_action (action_group,
			"compile_latex");
	latexila.actions.compile_pdflatex = gtk_action_group_get_action (action_group,
			"compile_pdflatex");
	latexila.actions.dvi_to_pdf = gtk_action_group_get_action (action_group,
			"DVItoPDF");
	latexila.actions.dvi_to_ps = gtk_action_group_get_action (action_group,
			"DVItoPS");
	latexila.actions.view_dvi = gtk_action_group_get_action (action_group,
			"viewDVI");
	latexila.actions.view_pdf = gtk_action_group_get_action (action_group,
			"viewPDF");
	latexila.actions.view_ps = gtk_action_group_get_action (action_group,
			"viewPS");
	latexila.actions.bibtex = gtk_action_group_get_action (action_group,
			"compile_bibtex");
	latexila.actions.makeindex = gtk_action_group_get_action (action_group,
			"compile_makeindex");
	latexila.actions.stop_execution = gtk_action_group_get_action (action_group,
			"stop_execution");

	GtkToggleAction *show_side_pane = GTK_TOGGLE_ACTION (
			gtk_action_group_get_action (action_group, "ViewSidePane"));
	gtk_toggle_action_set_active (show_side_pane,
			latexila.prefs.show_side_pane);

	GtkToggleAction *show_edit_toolbar = GTK_TOGGLE_ACTION (
			gtk_action_group_get_action (action_group, "ViewEditToolbar"));
	gtk_toggle_action_set_active (show_edit_toolbar,
			latexila.prefs.show_edit_toolbar);

	gtk_action_set_sensitive (latexila.actions.stop_execution, FALSE);
}
