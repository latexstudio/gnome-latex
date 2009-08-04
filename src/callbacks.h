#ifndef CALLBACKS_H
#define CALLBACKS_H

void cb_new (void);
void cb_open (void);
void cb_save (void);
void cb_save_as (void);
void cb_close (void);
void cb_quit (void);
void cb_about_dialog (void);
void cb_text_changed (GtkWidget *widget, gpointer user_data);
void cb_page_change (GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data);
gboolean cb_delete_event (GtkWidget *widget, GdkEvent *event, gpointer data);

#endif /* CALLBACKS_H */
