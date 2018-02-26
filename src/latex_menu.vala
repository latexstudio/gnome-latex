/*
 * This file is part of GNOME LaTeX.
 *
 * Copyright © 2010-2011, 2017 Sébastien Wilmet
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

using Gtk;

public class LatexMenu : Gtk.ActionGroup
{
    private const Gtk.ActionEntry[] latex_action_entries =
    {
        // LaTeX

        { "Latex", null, "_LaTeX" },

        // LaTeX: Sectioning

        { "Sectioning", "sectioning", N_("_Sectioning") },
        { "SectioningPart", null, "\\_part", null,
            N_("Part") },
        { "SectioningChapter", null, "\\_chapter", null,
            N_("Chapter") },
        { "SectioningSection", null, "\\_section", null,
            N_("Section") },
        { "SectioningSubsection", null, "\\s_ubsection", null,
            N_("Sub-section") },
        { "SectioningSubsubsection", null, "\\su_bsubsection", null,
            N_("Sub-sub-section") },
        { "SectioningParagraph", null, "\\p_aragraph", null,
            N_("Paragraph") },
        { "SectioningSubparagraph", null, "\\subpa_ragraph", null,
            N_("Sub-paragraph") },

        // LaTeX: References

        { "References", "references", N_("_References") },
        { "ReferencesLabel", null, "\\_label", null,
            N_("Label") },
        { "ReferencesRef", null, "\\_ref", null,
            N_("Reference to a label") },
        { "ReferencesPageref", null, "\\_pageref", null,
            N_("Page reference to a label") },
        { "ReferencesIndex", null, "\\_index", null,
            N_("Add a word to the index") },
        { "ReferencesFootnote", null, "\\_footnote", null,
            N_("Footnote") },
        { "ReferencesCite", null, "\\_cite", null,
            N_("Reference to a bibliography item") },

        // LaTeX: Environments

        { "Environments", "format-justify-center", "_Environments" },
        { "EnvCenter", "format-justify-center", "\\begin{_center}", null,
            N_("Center - \\begin{center}") },
        { "EnvLeft", "format-justify-left", "\\begin{flush_left}", null,
            N_("Align Left - \\begin{flushleft}") },
        { "EnvRight", "format-justify-right", "\\begin{flush_right}", null,
            N_("Align Right - \\begin{flushright}") },
        { "EnvTable", "table", "\\begin{_table}", null,
            N_("Table - \\begin{table}") },
        { "EnvQuote", null, "\\begin{_quote}", null,
            N_("Quote - \\begin{quote}") },
        { "EnvQuotation", null, "\\begin{qu_otation}", null,
            N_("Quotation - \\begin{quotation}") },
        { "EnvVerse", null, "\\begin{_verse}", null,
            N_("Verse - \\begin{verse}") },
        { "EnvVerbatim", null, "\\begin{ver_batim}", null,
            N_("Verbatim - \\begin{verbatim}") },
        { "EnvMinipage", null, "\\begin{_minipage}", null,
            N_("Minipage - \\begin{minipage}") },
        { "EnvTitlepage", null, "\\begin{titlepage}", null,
            N_("Title page - \\begin{titlepage}") },

        // LaTeX: list environments

        { "ListEnvironments", "list-itemize", N_("_List Environments") },
        { "ListEnvItemize", "list-itemize", "\\begin{_itemize}", null,
            N_("Bulleted List - \\begin{itemize}") },
        { "ListEnvEnumerate", "list-enumerate", "\\begin{_enumerate}", null,
            N_("Enumeration - \\begin{enumerate}") },
        { "ListEnvDescription", "list-description", "\\begin{_description}", null,
            N_("Description - \\begin{description}") },
        { "ListEnvList", null, "\\begin{_list}", null,
            N_("Custom list - \\begin{list}") },
        { "ListEnvItem", "list-item", "\\i_tem", "<Alt><Shift>H",
            N_("List item - \\item") },

        // LaTeX: character sizes

        { "CharacterSize", "character-size", N_("_Characters Sizes") },
        { "CharacterSizeTiny", null, "_tiny", null, "tiny" },
        { "CharacterSizeScriptsize", null, "_scriptsize", null, "scriptsize" },
        { "CharacterSizeFootnotesize", null, "_footnotesize", null, "footnotesize" },
        { "CharacterSizeSmall", null, "s_mall", null, "small" },
        { "CharacterSizeNormalsize", null, "_normalsize", null, "normalsize" },
        { "CharacterSizelarge", null, "_large", null, "large" },
        { "CharacterSizeLarge", null, "L_arge", null, "Large" },
        { "CharacterSizeLARGE", null, "LA_RGE", null, "LARGE" },
        { "CharacterSizehuge", null, "_huge", null, "huge" },
        { "CharacterSizeHuge", null, "H_uge", null, "Huge" },

        // LaTeX: font styles

        { "FontStyles", "bold", N_("_Font Styles") },
        { "Bold", "bold", "\\text_bf", "<Control>B",
            N_("Bold - \\textbf") },
        { "Italic", "italic", "\\text_it", "<Control>I",
            N_("Italic - \\textit") },
        { "Typewriter", "typewriter", "\\text_tt", "<Alt><Shift>T",
            N_("Typewriter - \\texttt") },
        { "Slanted", "slanted", "\\text_sl", "<Alt><Shift>S",
            N_("Slanted - \\textsl") },
        { "SmallCaps", "small_caps", "\\texts_c", "<Alt><Shift>C",
            N_("Small Capitals - \\textsc") },
        { "SansSerif", "sans_serif", "\\texts_f", null,
            N_("Sans Serif - \\textsf") },
        { "Emph", null, "\\_emph", "<Control>E",
            N_("Emphasized - \\emph") },
        { "Underline", "underline", "\\_underline", "<Control>U",
            N_("Underline - \\underline") },

        { "FontFamily", null, N_("_Font Family") },
        { "FontFamilyRoman", "roman", "\\_rmfamily", null,
            N_("Roman - \\rmfamily") },
        { "FontFamilySansSerif", "sans_serif", "\\_sffamily", null,
            N_("Sans Serif - \\sffamily") },
        { "FontFamilyMonospace", "typewriter", "\\_ttfamily", null,
            N_("Monospace - \\ttfamily") },

        { "FontSeries", null, N_("F_ont Series") },
        { "FontSeriesMedium", "roman", "\\_mdseries", null,
            N_("Medium - \\mdseries") },
        { "FontSeriesBold", "bold", "\\_bfseries", null,
            N_("Bold - \\bfseries") },

        { "FontShape", null, N_("Fo_nt Shape") },
        { "FontShapeUpright", "roman", "\\_upshape", null,
            N_("Upright - \\upshape") },
        { "FontShapeItalic", "italic", "\\_itshape", null,
            N_("Italic - \\itshape") },
        { "FontShapeSlanted", "slanted", "\\_slshape", null,
            N_("Slanted - \\slshape") },
        { "FontShapeSmallCaps", "small_caps", "\\s_cshape", null,
            N_("Small Capitals - \\scshape") },

        // LaTeX: Tabular

        { "Tabular", "table", N_("_Tabular") },
        { "TabularTabbing", null, "\\begin{ta_bbing}", null,
            N_("Tabbing - \\begin{tabbing}") },
        { "TabularTabular", null, "\\begin{_tabular}", null,
            N_("Tabular - \\begin{tabular}") },
        { "TabularMulticolumn", null, "\\_multicolumn", null,
            N_("Multicolumn - \\multicolumn") },
        { "TabularHline", null, "\\_hline", null,
            N_("Horizontal line - \\hline") },
        { "TabularVline", null, "\\_vline", null,
            N_("Vertical line - \\vline") },
        { "TabularCline", null, "\\_cline", null,
            N_("Horizontal line (columns specified) - \\cline") },

        // LaTeX: Presentation

        { "Presentation", "x-office-presentation", "_Presentation" },
        { "PresentationFrame", null, "\\begin{frame}", null,
            N_("Frame - \\begin{frame}") },
        { "PresentationBlock", null, "\\begin{block}", null,
            N_("Block - \\begin{block}") },
        { "PresentationColumns", null, "\\begin{columns}", null,
            N_("Two columns - \\begin{columns}") },

        // LaTeX: Spacing

        { "Spacing", null, N_("_Spacing") },
        { "SpacingNewLine", null, N_("New _Line"), null,
            N_("New Line - \\\\") },
        { "SpacingNewPage", null, "\\new_page", null,
            N_("New page - \\newpage") },
        { "SpacingLineBreak", null, "\\l_inebreak", null,
            N_("Line break - \\linebreak") },
        { "SpacingPageBreak", null, "\\p_agebreak", null,
            N_("Page break - \\pagebreak") },
        { "SpacingBigSkip", null, "\\_bigskip", null,
            N_("Big skip - \\bigskip") },
        { "SpacingMedSkip", null, "\\_medskip", null,
            N_("Medium skip - \\medskip") },
        { "SpacingHSpace", null, "\\_hspace", null,
            N_("Horizontal space - \\hspace") },
        { "SpacingVSpace", null, "\\_vspace", null,
            N_("Vertical space - \\vspace") },
        { "SpacingNoIndent", null, "\\_noindent", null,
            N_("No paragraph indentation - \\noindent") },

        // LaTeX: International accents

        { "Accents", null, N_("International _Accents") },
        { "Accent0", "accent0", "\\'", null, N_("Acute accent - \\'") },
        { "Accent1", "accent1", "\\`", null, N_("Grave accent - \\`") },
        { "Accent2", "accent2", "\\^", null, N_("Circumflex accent - \\^") },
        { "Accent3", "accent3", "\\\"", null, N_("Trema - \\\"") },
        { "Accent4", "accent4", "\\~", null, N_("Tilde - \\~") },
        { "Accent5", "accent5", "\\=", null, N_("Macron - \\=") },
        { "Accent6", "accent6", "\\.", null, N_("Dot above - \\.") },
        { "Accent7", "accent7", "\\v", null, N_("Caron - \\v") },
        { "Accent8", "accent8", "\\u", null, N_("Breve - \\u") },
        { "Accent9", "accent9", "\\H", null,
            N_("Double acute accent - \\H") },
        { "Accent10", "accent10", "\\c", null, N_("Cedilla - \\c") },
        { "Accent11", "accent11", "\\k", null, N_("Ogonek - \\k") },
        { "Accent12", "accent12", "\\d", null, N_("Dot below - \\d") },
        { "Accent13", "accent13", "\\b", null, N_("Macron below - \\b") },
        { "Accent14", "accent14", "\\r", null, N_("Ring - \\r") },
        { "Accent15", "accent15", "\\t", null, N_("Tie - \\t") },

        // LaTeX: Misc

        { "LatexMisc", null, N_("_Misc") },
        { "LatexDocumentClass", null, "\\_documentclass", null,
            N_("Document class - \\documentclass") },
        { "LatexUsepackage", null, "\\_usepackage", null,
            N_("Use package - \\usepackage") },
        { "LatexAMS", null, N_("_AMS packages"), null,
            N_("AMS packages") },
        { "LatexAuthor", null, "\\au_thor", null, N_("Author - \\author") },
        { "LatexTitle", null, "\\t_itle", null, N_("Title - \\title") },
        { "LatexBeginDocument", null, "\\begin{d_ocument}", null,
            N_("Content of the document - \\begin{document}") },
        { "LatexMakeTitle", null, "\\_maketitle", null,
            N_("Make title - \\maketitle") },
        { "LatexTableOfContents", null, "\\tableof_contents", null,
            N_("Table of contents - \\tableofcontents") },
        { "LatexAbstract", null, "\\begin{abst_ract}", null,
            N_("Abstract - \\begin{abstract}") },
        { "LatexIncludeGraphics", null, "\\include_graphics", null,
            N_("Include an image (graphicx package) - \\includegraphics") },
        { "LatexInput", null, "\\_input", null,
            N_("Include a file - \\input") },

        // Math

        { "Math", null, N_("_Math") },

        // Math Environments

        { "MathEnvironments", null, N_("_Math Environments") },
        { "MathEnvNormal", null, N_("_Mathematical Environment - $...$"),
            "<Alt><Shift>M", N_("Mathematical Environment - $...$") },
        { "MathEnvCentered", null, N_("_Centered Formula - \\[...\\]"),
            "<Alt><Shift>E", N_("Centered Formula - \\[...\\]") },
        { "MathEnvNumbered", null,
            N_("_Numbered Equation - \\begin{equation}"), null,
            N_("Numbered Equation - \\begin{equation}") },
        { "MathEnvArray", null, N_("_Array of Equations - \\begin{align*}"), null,
            N_("Array of Equations - \\begin{align*}") },
        { "MathEnvNumberedArray", null,
            N_("Numbered Array of _Equations - \\begin{align}"), null,
            N_("Numbered Array of Equations - \\begin{align}") },

        // Math misc

        { "MathSuperscript", "math-superscript", N_("_Superscript - ^{}"), null,
            N_("Superscript - ^{}") },
        { "MathSubscript", "math-subscript", N_("Su_bscript - __{}"), null,
            N_("Subscript - _{}") },
        { "MathFrac", "math-frac", N_("_Fraction - \\frac{}{}"), "<Alt><Shift>F",
            N_("Fraction - \\frac{}{}") },
        { "MathSquareRoot", "math-square-root", N_("Square _Root - \\sqrt{}"), null,
            N_("Square Root - \\sqrt{}") },
        { "MathNthRoot", "math-nth-root", N_("_N-th Root - \\sqrt[]{}"), null,
            N_("N-th Root - \\sqrt[]{}") },

        // Math functions

        { "MathFunctions", null, N_("Math _Functions") },
        { "MathFuncArccos", null, "\\arccos", null, null },
        { "MathFuncArcsin", null, "\\arcsin", null, null },
        { "MathFuncArctan", null, "\\arctan", null, null },
        { "MathFuncCos", null, "\\cos", null, null },
        { "MathFuncCosh", null, "\\cosh", null, null },
        { "MathFuncCot", null, "\\cot", null, null },
        { "MathFuncCoth", null, "\\coth", null, null },
        { "MathFuncCsc", null, "\\csc", null, null },
        { "MathFuncDeg", null, "\\deg", null, null },
        { "MathFuncDet", null, "\\det", null, null },
        { "MathFuncDim", null, "\\dim", null, null },
        { "MathFuncExp", null, "\\exp", null, null },
        { "MathFuncGcd", null, "\\gcd", null, null },
        { "MathFuncHom", null, "\\hom", null, null },
        { "MathFuncInf", null, "\\inf", null, null },
        { "MathFuncKer", null, "\\ker", null, null },
        { "MathFuncLg", null, "\\lg", null, null },
        { "MathFuncLim", null, "\\lim", null, null },
        { "MathFuncLiminf", null, "\\liminf", null, null },
        { "MathFuncLimsup", null, "\\limsup", null, null },
        { "MathFuncLn", null, "\\ln", null, null },
        { "MathFuncLog", null, "\\log", null, null },
        { "MathFuncMax", null, "\\max", null, null },
        { "MathFuncMin", null, "\\min", null, null },
        { "MathFuncSec", null, "\\sec", null, null },
        { "MathFuncSin", null, "\\sin", null, null },
        { "MathFuncSinh", null, "\\sinh", null, null },
        { "MathFuncSup", null, "\\sup", null, null },
        { "MathFuncTan", null, "\\tan", null, null },
        { "MathFuncTanh", null, "\\tanh", null, null },

        // Math Font Styles

        { "MathFontStyles", null, N_("Math Font _Styles") },
        { "MathFSrm", "roman", "\\math_rm", null,
            N_("Roman - \\mathrm") },
        { "MathFSit", "italic", "\\math_it", null,
            N_("Italic - \\mathit") },
        { "MathFSbf", "bold", "\\math_bf", null,
            N_("Bold - \\mathbf") },
        { "MathFSsf", "sans_serif", "\\math_sf", null,
            N_("Sans Serif - \\mathsf") },
        { "MathFStt", "typewriter", "\\math_tt", null,
            N_("Typewriter - \\mathtt") },
        { "MathFScal", "mathcal", "\\math_cal", null,
            N_("Calligraphic - \\mathcal") },
        { "MathFSbb", "blackboard", "\\_mathbb", null,
            N_("Blackboard (uppercase only)  - \\mathbb (amsfonts package)") },
        { "MathFSfrak", "mathfrak", "\\math_frak", null,
            N_("Euler Fraktur - \\mathfrak (amsfonts package)") },

        // Math Accents

        { "MathAccents", null, N_("Math _Accents") },
        { "MathAccentAcute", "mathaccent0", "\\_acute", null, null },
        { "MathAccentGrave", "mathaccent1", "\\_grave", null, null },
        { "MathAccentTilde", "mathaccent2", "\\_tilde", null, null },
        { "MathAccentBar", "mathaccent3", "\\_bar", null, null },
        { "MathAccentVec", "mathaccent4", "\\_vec", null, null },
        { "MathAccentHat", "mathaccent5", "\\_hat", null, null },
        { "MathAccentCheck", "mathaccent6", "\\_check", null, null },
        { "MathAccentBreve", "mathaccent7", "\\b_reve", null, null },
        { "MathAccentDot", "mathaccent8", "\\_dot", null, null },
        { "MathAccentDdot", "mathaccent9", "\\dd_ot", null, null },
        { "MathAccentRing", "mathaccent10", "\\_mathring", null, null },

        // Math Spaces

        { "MathSpaces", null, N_("Math _Spaces") },
        { "MathSpaceSmall", null, N_("_Small"), null, N_("Small - \\,") },
        { "MathSpaceMedium", null, N_("_Medium"), null, N_("Medium - \\:") },
        { "MathSpaceLarge", null, N_("_Large"), null, N_("Large - \\;") },
        { "MathSpaceQuad", null, "\\_quad", null, null },
        { "MathSpaceQquad", null, "\\qqu_ad", null, null },

        // Math: Left Delimiters

        { "MathLeftDelimiters", "delimiters-left", N_("_Left Delimiters") },
        { "MathLeftDelimiter1", null, N_("left ("), null, null },
        { "MathLeftDelimiter2", null, N_("left ["), null, null },
        { "MathLeftDelimiter3", null, N_("left { "), null, null },
        { "MathLeftDelimiter4", null, N_("left <"), null, null },
        { "MathLeftDelimiter5", null, N_("left )"), null, null },
        { "MathLeftDelimiter6", null, N_("left ]"), null, null },
        { "MathLeftDelimiter7", null, N_("left  }"), null, null },
        { "MathLeftDelimiter8", null, N_("left >"), null, null },
        { "MathLeftDelimiter9", null, N_("left ."), null, null },

        // Math: Right Delimiters

        { "MathRightDelimiters", "delimiters-right", N_("Right _Delimiters") },
        { "MathRightDelimiter1", null, N_("right )"), null,
            null, on_math_right_delimiter_1 },
        { "MathRightDelimiter2", null, N_("right ]"), null,
            null, on_math_right_delimiter_2 },
        { "MathRightDelimiter3", null, N_("right  }"), null,
            null, on_math_right_delimiter_3 },
        { "MathRightDelimiter4", null, N_("right >"), null,
            null, on_math_right_delimiter_4 },
        { "MathRightDelimiter5", null, N_("right ("), null,
            null, on_math_right_delimiter_5 },
        { "MathRightDelimiter6", null, N_("right ["), null,
            null, on_math_right_delimiter_6 },
        { "MathRightDelimiter7", null, N_("right { "), null,
            null, on_math_right_delimiter_7 },
        { "MathRightDelimiter8", null, N_("right <"), null,
            null, on_math_right_delimiter_8 },
        { "MathRightDelimiter9", null, N_("right ."), null,
            null, on_math_right_delimiter_9 }
    };

    private unowned MainWindow main_window;

    public LatexMenu (MainWindow main_window)
    {
        GLib.Object (name: "LatexActionGroup");
        set_translation_domain (Config.GETTEXT_PACKAGE);

        this.main_window = main_window;

        // menus under toolitems
        Gtk.Action sectioning = get_menu_tool_action ("SectioningToolItem",
            _("Sectioning"), "sectioning");

        Gtk.Action sizes = get_menu_tool_action ("CharacterSizeToolItem",
            _("Characters Sizes"), "character-size");

        Gtk.Action references = get_menu_tool_action ("ReferencesToolItem",
            _("References"), "references");

        Gtk.Action presentation_env = get_menu_tool_action ("PresentationToolItem",
            _("Presentation Environments"), "x-office-presentation");

        Gtk.Action math_env = get_menu_tool_action ("MathEnvironmentsToolItem",
            _("Math Environments"), "math");

        add_actions (latex_action_entries, this);
        add_action (sectioning);
        add_action (sizes);
        add_action (references);
        add_action (presentation_env);
        add_action (math_env);

        /* GActions */

        Latexila.latex_commands_add_actions (main_window);

        // LaTeX: Sectioning
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::part",
            this, "SectioningPart");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::chapter",
            this, "SectioningChapter");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::section",
            this, "SectioningSection");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::subsection",
            this, "SectioningSubsection");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::subsubsection",
            this, "SectioningSubsubsection");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::paragraph",
            this, "SectioningParagraph");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::subparagraph",
            this, "SectioningSubparagraph");

        // LaTeX: References
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::label",
            this, "ReferencesLabel");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::ref",
            this, "ReferencesRef");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::pageref",
            this, "ReferencesPageref");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::index",
            this, "ReferencesIndex");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::footnote",
            this, "ReferencesFootnote");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::cite",
            this, "ReferencesCite");

        // LaTeX: Environments
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-env-simple::center",
            this, "EnvCenter");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-env-simple::flushleft",
            this, "EnvLeft");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-env-simple::flushright",
            this, "EnvRight");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-env-figure",
            this, "EnvFigure");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-env-table",
            this, "EnvTable");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-env-simple::quote",
            this, "EnvQuote");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-env-simple::quotation",
            this, "EnvQuotation");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-env-simple::verse",
            this, "EnvVerse");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-env-simple::verbatim",
            this, "EnvVerbatim");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-env-simple::minipage",
            this, "EnvMinipage");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-env-simple::titlepage",
            this, "EnvTitlepage");

        // LaTeX: list environments
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-list-env-simple::itemize",
            this, "ListEnvItemize");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-list-env-simple::enumerate",
            this, "ListEnvEnumerate");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-list-env-description",
            this, "ListEnvDescription");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-list-env-list",
            this, "ListEnvList");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::item",
            this, "ListEnvItem");

        // LaTeX: character sizes
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-char-style::tiny",
            this, "CharacterSizeTiny");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-char-style::scriptsize",
            this, "CharacterSizeScriptsize");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-char-style::footnotesize",
            this, "CharacterSizeFootnotesize");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-char-style::small",
            this, "CharacterSizeSmall");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-char-style::normalsize",
            this, "CharacterSizeNormalsize");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-char-style::large",
            this, "CharacterSizelarge");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-char-style::Large",
            this, "CharacterSizeLarge");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-char-style::LARGE",
            this, "CharacterSizeLARGE");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-char-style::huge",
            this, "CharacterSizehuge");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-char-style::Huge",
            this, "CharacterSizeHuge");

        // LaTeX: font styles
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::textbf",
            this, "Bold");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::textit",
            this, "Italic");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::texttt",
            this, "Typewriter");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::textsl",
            this, "Slanted");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::textsc",
            this, "SmallCaps");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::textsf",
            this, "SansSerif");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::emph",
            this, "Emph");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::underline",
            this, "Underline");

        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-char-style::rmfamily",
            this, "FontFamilyRoman");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-char-style::sffamily",
            this, "FontFamilySansSerif");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-char-style::ttfamily",
            this, "FontFamilyMonospace");

        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-char-style::mdseries",
            this, "FontSeriesMedium");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-char-style::bfseries",
            this, "FontSeriesBold");

        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-char-style::upshape",
            this, "FontShapeUpright");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-char-style::itshape",
            this, "FontShapeItalic");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-char-style::slshape",
            this, "FontShapeSlanted");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-char-style::scshape",
            this, "FontShapeSmallCaps");

        // LaTeX: Tabular
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-env-simple::tabbing",
            this, "TabularTabbing");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-tabular-tabular",
            this, "TabularTabular");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-tabular-multicolumn",
            this, "TabularMulticolumn");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::hline",
            this, "TabularHline");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::vline",
            this, "TabularVline");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-tabular-cline",
            this, "TabularCline");

        // LaTeX: Presentation
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-presentation-frame",
            this, "PresentationFrame");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-presentation-block",
            this, "PresentationBlock");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-presentation-columns",
            this, "PresentationColumns");

        // LaTeX: Spacing
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-spacing-new-line",
            this, "SpacingNewLine");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-newline::newpage",
            this, "SpacingNewPage");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-newline::linebreak",
            this, "SpacingLineBreak");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-newline::pagebreak",
            this, "SpacingPageBreak");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::bigskip",
            this, "SpacingBigSkip");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::medskip",
            this, "SpacingMedSkip");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::hspace",
            this, "SpacingHSpace");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::vspace",
            this, "SpacingVSpace");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::noindent",
            this, "SpacingNoIndent");

        // LaTeX: International accents
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces('\\'')",
            this, "Accent0");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces('`')",
            this, "Accent1");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces('^')",
            this, "Accent2");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces('\"')",
            this, "Accent3");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces('~')",
            this, "Accent4");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces('=')",
            this, "Accent5");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces('.')",
            this, "Accent6");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces('v')",
            this, "Accent7");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces('u')",
            this, "Accent8");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces('H')",
            this, "Accent9");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces('c')",
            this, "Accent10");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces('k')",
            this, "Accent11");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces('d')",
            this, "Accent12");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces('b')",
            this, "Accent13");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces('r')",
            this, "Accent14");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces('t')",
            this, "Accent15");

        // LaTeX: Misc
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::documentclass",
            this, "LatexDocumentClass");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::usepackage",
            this, "LatexUsepackage");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-ams-packages",
            this, "LatexAMS");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::author",
            this, "LatexAuthor");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::title",
            this, "LatexTitle");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-env-simple::document",
            this, "LatexBeginDocument");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-simple::maketitle",
            this, "LatexMakeTitle");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-simple::tableofcontents",
            this, "LatexTableOfContents");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-env-simple::abstract",
            this, "LatexAbstract");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::includegraphics",
            this, "LatexIncludeGraphics");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::input",
            this, "LatexInput");

        // Math Environments
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "math-command-env-normal",
            this, "MathEnvNormal");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "math-command-env-centered",
            this, "MathEnvCentered");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-env-simple::equation",
            this, "MathEnvNumbered");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "math-command-env-array",
            this, "MathEnvArray");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-env-simple::align",
            this, "MathEnvNumberedArray");

        // Math misc
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "math-command-misc-superscript",
            this, "MathSuperscript");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "math-command-misc-subscript",
            this, "MathSubscript");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "math-command-misc-frac",
            this, "MathFrac");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::sqrt",
            this, "MathSquareRoot");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "math-command-misc-nth-root",
            this, "MathNthRoot");

        // Math functions
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::arccos",
            this, "MathFuncArccos");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::arcsin",
            this, "MathFuncArcsin");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::arctan",
            this, "MathFuncArctan");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::cos",
            this, "MathFuncCos");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::cosh",
            this, "MathFuncCosh");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::cot",
            this, "MathFuncCot");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::coth",
            this, "MathFuncCoth");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::csc",
            this, "MathFuncCsc");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::deg",
            this, "MathFuncDeg");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::det",
            this, "MathFuncDet");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::dim",
            this, "MathFuncDim");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::exp",
            this, "MathFuncExp");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::gcd",
            this, "MathFuncGcd");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::hom",
            this, "MathFuncHom");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::inf",
            this, "MathFuncInf");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::ker",
            this, "MathFuncKer");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::lg",
            this, "MathFuncLg");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::lim",
            this, "MathFuncLim");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::liminf",
            this, "MathFuncLiminf");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::limsup",
            this, "MathFuncLimsup");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::ln",
            this, "MathFuncLn");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::log",
            this, "MathFuncLog");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::max",
            this, "MathFuncMax");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::min",
            this, "MathFuncMin");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::sec",
            this, "MathFuncSec");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::sin",
            this, "MathFuncSin");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::sinh",
            this, "MathFuncSinh");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::sup",
            this, "MathFuncSup");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::tan",
            this, "MathFuncTan");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::tanh",
            this, "MathFuncTanh");

        // Math Font Styles
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::mathrm",
            this, "MathFSrm");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::mathit",
            this, "MathFSit");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::mathbf",
            this, "MathFSbf");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::mathsf",
            this, "MathFSsf");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::mathtt",
            this, "MathFStt");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::mathcal",
            this, "MathFScal");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::mathbb",
            this, "MathFSbb");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::mathfrak",
            this, "MathFSfrak");

        // Math Accents
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::acute",
            this, "MathAccentAcute");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::grave",
            this, "MathAccentGrave");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::tilde",
            this, "MathAccentTilde");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::bar",
            this, "MathAccentBar");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::vec",
            this, "MathAccentVec");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::hat",
            this, "MathAccentHat");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::check",
            this, "MathAccentCheck");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::breve",
            this, "MathAccentBreve");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::dot",
            this, "MathAccentDot");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::ddot",
            this, "MathAccentDdot");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-braces::mathring",
            this, "MathAccentRing");

        // Math Spaces
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "math-command-spaces-small",
            this, "MathSpaceSmall");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "math-command-spaces-medium",
            this, "MathSpaceMedium");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "math-command-spaces-large",
            this, "MathSpaceLarge");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::quad",
            this, "MathSpaceQuad");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "latex-command-with-space::qquad",
            this, "MathSpaceQquad");

        // Math Left Delimiters
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "math-command-delimiter-left1",
            this, "MathLeftDelimiter1");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "math-command-delimiter-left2",
            this, "MathLeftDelimiter2");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "math-command-delimiter-left3",
            this, "MathLeftDelimiter3");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "math-command-delimiter-left4",
            this, "MathLeftDelimiter4");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "math-command-delimiter-left5",
            this, "MathLeftDelimiter5");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "math-command-delimiter-left6",
            this, "MathLeftDelimiter6");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "math-command-delimiter-left7",
            this, "MathLeftDelimiter7");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "math-command-delimiter-left8",
            this, "MathLeftDelimiter8");
        Amtk.utils_bind_g_action_to_gtk_action (main_window, "math-command-delimiter-left9",
            this, "MathLeftDelimiter9");
    }

    private Gtk.Action get_menu_tool_action (string name, string? label, string? icon_name)
    {
        Gtk.Action action = new MenuToolAction (name, label, label, icon_name);
        Activatable menu_tool_button = (Activatable) new MenuToolButton (null, null);
        menu_tool_button.set_related_action (action);
        return action;
    }

    private void text_buffer_insert (string text_before, string text_after,
        string? text_if_no_selection = null)
    {
        Tepl.ApplicationWindow tepl_window =
            Tepl.ApplicationWindow.get_from_gtk_application_window (main_window);

        Latexila.latex_commands_insert_text (tepl_window, text_before, text_after,
            text_if_no_selection);
    }

    /* Right Delimiters */

    public void on_math_right_delimiter_1 ()
    {
        text_buffer_insert ("\\right) ", "");
    }

    public void on_math_right_delimiter_2 ()
    {
        text_buffer_insert ("\\right] ", "");
    }

    public void on_math_right_delimiter_3 ()
    {
        text_buffer_insert ("\\right\\rbrace ", "");
    }

    public void on_math_right_delimiter_4 ()
    {
        text_buffer_insert ("\\right\\rangle ", "");
    }

    public void on_math_right_delimiter_5 ()
    {
        text_buffer_insert ("\\right( ", "");
    }

    public void on_math_right_delimiter_6 ()
    {
        text_buffer_insert ("\\right[ ", "");
    }

    public void on_math_right_delimiter_7 ()
    {
        text_buffer_insert ("\\right\\lbrace ", "");
    }

    public void on_math_right_delimiter_8 ()
    {
        text_buffer_insert ("\\right\\langle ", "");
    }

    public void on_math_right_delimiter_9 ()
    {
        text_buffer_insert ("\\right. ", "");
    }
}
