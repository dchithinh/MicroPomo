#ifndef __H_SETTINGS_SCREEN_H__
#define __H_SETTINGS_SCREEN_H__

typedef enum {
    POMO_DARK_THEME,
    POMO_LIGHT_THEME
} pomodoro_theme_e;

pomodoro_theme_e ui_get_theme(void);

#endif/* __H_SETTINGS_SCREEN_H__ */
