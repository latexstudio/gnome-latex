#ifndef CALLBACKS_H
#define CALLBACKS_H

void cb_new (void);
void cb_open (void);
void cb_save (void);
void cb_save_as (void);
void cb_close (void);
void cb_close_tab (GtkWidget *widget, GtkWidget *child);
void cb_quit (void);
void cb_undo (void);
void cb_redo (void);
void cb_latex (void);
void cb_pdflatex (void);
void cb_view_dvi (void);
void cb_view_pdf (void);
void cb_view_ps (void);
void cb_dvi_to_pdf (void);
void cb_dvi_to_ps (void);
void cb_action_list_changed (GtkTreeSelection *selection,
		gpointer user_data);
void cb_about_dialog (void);
void cb_text_changed (GtkWidget *widget, gpointer user_data);
void cb_cursor_moved (GtkTextBuffer *text_buffer, GtkTextIter *location,
		GtkTextMark *mark, gpointer user_data);
void cb_page_change (GtkNotebook *notebook, GtkNotebookPage *page,
		guint page_num, gpointer user_data);
gboolean cb_delete_event (GtkWidget *widget, GdkEvent *event,
		gpointer user_data);
void cb_line_numbers (GtkToggleAction *action, gpointer user_data);
void cb_recent_item_activated (GtkRecentAction *action, gpointer user_data);

#endif /* CALLBACKS_H */
