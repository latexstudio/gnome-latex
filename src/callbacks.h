#ifndef CALLBACKS_H
#define CALLBACKS_H

void cb_open (GtkAction *action, widgets_t *widgets);
void cb_save (GtkAction *action, widgets_t *widgets);
void cb_save_as (GtkAction *action, widgets_t *widgets);
void cb_about_dialog (GtkAction *action, widgets_t *widgets);
void cb_quit (void);
void cb_text_changed (GtkWidget *widget, gpointer user_data);

#endif /* CALLBACKS_H */
