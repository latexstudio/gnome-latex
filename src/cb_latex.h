#ifndef CB_LATEX_H
#define CB_LATEX_H

void cb_sectioning_part (void);
void cb_sectioning_chapter (void);
void cb_sectioning_section (void);
void cb_sectioning_subsection (void);
void cb_sectioning_subsubsection (void);
void cb_sectioning_paragraph (void);
void cb_sectioning_subparagraph (void);

void cb_ref_label (void);
void cb_ref_ref (void);
void cb_ref_pageref (void);
void cb_ref_index (void);
void cb_ref_footnote (void);
void cb_ref_cite (void);

void cb_env_center (void);
void cb_env_left (void);
void cb_env_right (void);
void cb_env_minipage (void);
void cb_env_quote (void);
void cb_env_quotation (void);
void cb_env_verse (void);

void cb_list_env_itemize (void);
void cb_list_env_enumerate (void);
void cb_list_env_description (void);
void cb_list_env_item (void);

void cb_size_tiny (void);
void cb_size_scriptsize (void);
void cb_size_footnotesize (void);
void cb_size_small (void);
void cb_size_normalsize (void);
void cb_size_large (void);
void cb_size_Large (void);
void cb_size_LARGE (void);
void cb_size_huge (void);
void cb_size_Huge (void);

void cb_text_bold (void);
void cb_text_italic (void);
void cb_text_typewriter (void);
void cb_text_underline (void);
void cb_text_slanted (void);
void cb_text_small_caps (void);
void cb_text_emph (void);
void cb_text_strong (void);
void cb_text_font_family_roman (void);
void cb_text_font_family_sans_serif (void);
void cb_text_font_family_monospace (void);
void cb_text_font_series_medium (void);
void cb_text_font_series_bold (void);
void cb_text_font_shape_upright (void);
void cb_text_font_shape_italic (void);
void cb_text_font_shape_slanted (void);
void cb_text_font_shape_small_caps (void);

#endif /* CB_LATEX_H */
