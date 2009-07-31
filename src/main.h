#ifndef MAIN_H
#define MAIN_H

#define _(STRING) gettext(STRING)

#define PROGRAM_NAME "LaTeXila"
#define PROGRAM_VERSION "0.0.1"

static void menu_add_widget (GtkUIManager *ui_manager, GtkWidget *widget, GtkContainer *box);
static void about_dialog (void);

#endif // MAIN_H
