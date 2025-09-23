#ifndef __H_SETTINGS_SCREEN_H__
#define __H_SETTINGS_SCREEN_H__

#include "lvgl.h"

typedef enum {
    POMO_DARK_THEME,
    POMO_LIGHT_THEME
} pomodoro_theme_e;

pomodoro_theme_e ui_get_theme(void);
void show_settings_screen(lv_obj_t *parent);


#endif/* __H_SETTINGS_SCREEN_H__ */
