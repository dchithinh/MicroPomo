#ifndef __H_MAIN_SCREEN_H__
#define __H_MAIN_SCREEN_H__

#include "lvgl.h"

typedef enum {
    POMO_DARK_THEME,
    POMO_LIGHT_THEME
} pomodoro_theme_e;

void ui_main_screen(lv_obj_t *parent);

#endif /* __H_MAIN_SCREEN_H__ */
