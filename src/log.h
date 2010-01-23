/*
 * This file is part of LaTeXila.
 *
 * Copyright © 2010 Sébastien Wilmet
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

#ifndef LOG_H
#define LOG_H

#define NO_LINE			-1
#define COLOR_RED 		"#C00000"
#define COLOR_ORANGE	"#FF7200"
#define COLOR_BROWN		"#663106"
#define COLOR_GREEN		"#009900"
#define WEIGHT_NORMAL	400
#define WEIGHT_BOLD		800
#define INFO_MESSAGE	"*****"

void init_log_zone (GtkPaned *log_hpaned, GtkWidget *log_toolbar);
void add_action (const gchar *title, const gchar *command);
void cb_go_previous_latex_error (void);
void cb_go_previous_latex_warning (void);
void cb_go_previous_latex_badbox (void);
void cb_go_next_latex_error (void);
void cb_go_next_latex_warning (void);
void cb_go_next_latex_badbox (void);
void set_history_sensitivity (gboolean sensitive);
void output_view_columns_autosize (void);

void print_output_title (const gchar *title);
void print_output_info (const gchar *info);
void print_output_stats (gint nb_errors, gint nb_warnings, gint nb_badboxes);
void print_output_exit (const gint exit_code, const gchar *message);
void print_output_message (const gchar *filename, const gint line_number,
		const gchar *message, gint message_type);
void print_output_normal (const gchar *message);

enum history_action
{
	COL_ACTION_TITLE,
	COL_ACTION_OUTPUT_STORE,
	N_COLS_ACTION
};

enum output_message_type
{
	MESSAGE_TYPE_OTHER,
	MESSAGE_TYPE_BADBOX,
	MESSAGE_TYPE_WARNING,
	MESSAGE_TYPE_ERROR
};

enum output_line
{
	COL_OUTPUT_BASENAME,
	COL_OUTPUT_FILENAME,
	COL_OUTPUT_LINE_NUMBER,
	COL_OUTPUT_MESSAGE,
	COL_OUTPUT_MESSAGE_TYPE,
	COL_OUTPUT_COLOR,
	COL_OUTPUT_COLOR_SET,
	COL_OUTPUT_BG_COLOR,
	COL_OUTPUT_BG_COLOR_SET,
	COL_OUTPUT_WEIGHT,
	N_COLS_OUTPUT
};

typedef struct
{
	gint nb_errors;
	gint nb_warnings;
	gint nb_badboxes;
	gint *errors;
	gint *warnings;
	gint *badboxes;
} messages_index_t;

#endif /* LOG_H */
