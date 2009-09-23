#ifndef PREFS_H
#define PREFS_H

void load_preferences (preferences_t *prefs);
void save_preferences (preferences_t *prefs);
void set_current_font_prefs (preferences_t *prefs);
void cb_preferences (void);

#endif /* PREFS_H */
