#include "settings_screen.h"
#include "lvgl.h"


static lv_obj_t *settings_screen;
static lv_obj_t *work_time_sb, *short_break_sb, *long_break_sb, *cycle_sb;



static void ui_setting_screen_set_bg_by_theme(lv_obj_t *parent);

pomodoro_theme_e ui_get_theme(void) {
    //Only support Dark theme for now
    return POMO_DARK_THEME;
}

// Callback for when a setting changes
static void settings_changed(lv_event_t *e)
{
    // Example: Save or apply new settings here
    // int work_time = lv_spinbox_get_value(work_time_sb);
    // int short_break = lv_spinbox_get_value(short_break_sb);
    // int long_break = lv_spinbox_get_value(long_break_sb);
    // int cycle = lv_spinbox_get_value(cycle_sb);
}

lv_obj_t *setting_screen_create_spinbox(lv_obj_t *parent, const char *label_text, int min, int max, int step, int initial_value)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, label_text);

    // Create spinbox
    lv_obj_t *spinbox = lv_spinbox_create(parent);
    lv_spinbox_set_range(spinbox, min, max);
    lv_spinbox_set_step(spinbox, step);
    lv_spinbox_set_value(spinbox, initial_value);

    // Format: two digits, no decimals
    lv_spinbox_set_digit_format(spinbox, 2, 0); // 2 digits, 0 decimals

    // // Optional: add postfix (e.g., "min") for clarity
    // lv_spinbox_set_postfix(spinbox, " min");

    // lv_obj_add_event_cb(spinbox, settings_changed, LV_EVENT_VALUE_CHANGED, NULL);

    return spinbox;
}

void show_settings_screen(lv_obj_t *parent)
{
    settings_screen = lv_obj_create(parent);
    ui_setting_screen_set_bg_by_theme(settings_screen);

    lv_obj_t *timer_section = lv_obj_create(settings_screen);
    const static int col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    const static int row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};

    lv_obj_set_grid_dsc_array(timer_section, col_dsc, row_dsc);
    lv_obj_set_size(timer_section, LV_PCT(100), 150);

    lv_obj_t *section_title = lv_label_create(timer_section);
    lv_obj_set_grid_cell(section_title, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_END, 0, 1);
    lv_label_set_text(section_title, "TIMER");
    lv_obj_set_style_text_font(section_title, &lv_font_montserrat_22, 0);
    
    lv_obj_t *time_label = lv_label_create(timer_section);
    lv_obj_set_grid_cell(time_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 1, 1);
    lv_label_set_text(time_label, "Time (minutes)");


    lv_obj_t *work_label = lv_label_create(timer_section);
    lv_obj_set_grid_cell(work_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_label_set_text(work_label, "Pomodoro");
    lv_obj_t *pomo_sb = setting_screen_create_spinbox(timer_section, "", 1, 180, 1, 25);
    lv_obj_set_grid_cell(pomo_sb, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 3, 1);

    lv_obj_t *short_break_label = lv_label_create(timer_section);
    lv_obj_set_grid_cell(short_break_label, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_label_set_text(short_break_label, "Short Break");
    lv_obj_t *short_sb = setting_screen_create_spinbox(timer_section, "", 1, 60, 1, 5);
    lv_obj_set_grid_cell(short_sb, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 3, 1);

    lv_obj_t *long_break_label = lv_label_create(timer_section);
    lv_obj_set_grid_cell(long_break_label, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_label_set_text(long_break_label, "Long Break");
    lv_obj_t *long_sb = setting_screen_create_spinbox(timer_section, "", 1, 120, 1, 15);
    lv_obj_set_grid_cell(long_sb, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_START, 3, 1);
}

static void ui_setting_screen_set_bg_by_theme(lv_obj_t *parent)
{
    if (ui_get_theme() == POMO_DARK_THEME) {
        lv_obj_remove_style_all(parent);
        lv_obj_set_size(parent, LV_PCT(100), LV_PCT(100));
        // Set background color for the parent/screen
        lv_obj_set_style_bg_color(parent, lv_color_hex(0x343247), 0);
        lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);
        lv_obj_set_style_pad_all(parent, 0, 0);
        lv_obj_set_style_margin_all(parent, 0, 0);
        lv_obj_set_style_border_width(parent, 0, 0);

    }
    else {
        lv_obj_set_style_bg_color(parent, lv_color_hex(0xffffff), 0);
    }
}

int settings_get_work_time()      { return lv_spinbox_get_value(work_time_sb); }
int settings_get_short_break()    { return lv_spinbox_get_value(short_break_sb); }
int settings_get_long_break()     { return lv_spinbox_get_value(long_break_sb); }
int settings_get_cycle_count()    { return lv_spinbox_get_value(cycle_sb); }