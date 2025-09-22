#include <stdio.h>

#include "lvgl.h"
#include "event.h"
#include "timer.h"
#include "pomodoro.h"
#include "main_screen.h"


static lv_obj_t *label_mode;
static lv_obj_t *label_timer;
static lv_obj_t *label_cycle;
static lv_obj_t *progress;
static lv_obj_t *label_pause;

static lv_obj_t *btn_start;
static lv_obj_t *btn_reset;

static lv_style_t progress_main_style;
static lv_style_t progress_indic_style;
static lv_style_t font_style;


static bool timer_running = false;
static int remaining_sec = POMODORO_DEF_WORK_MIN * 60;

/* Forward declarations */
static void update_timer_label(uint32_t remaining_ms);
static void start_event_cb(lv_event_t *e);
static void reset_event_cb(lv_event_t *e);
static void pomodoro_state_changed(PomodoroState_e state);
static void timer_tick_cb(lv_timer_t * timer);
static void ui_tick_cb(uint32_t remaining);
static void ui_update_by_state(PomodoroState_e state);

/* --- UI Functions --- */


static pomodoro_theme_e ui_get_theme(void) {
    //Only support Dark theme for now
    return POMO_DARK_THEME;
}

static void ui_theme_init(void) {

    lv_style_init(&progress_main_style);
    lv_style_init(&progress_indic_style);
    lv_style_init(&font_style);

    if (ui_get_theme() == POMO_DARK_THEME) {
        lv_style_set_width(&progress_main_style, 10);
        lv_style_set_arc_color(&progress_main_style, lv_color_hex(0x000000));

        lv_style_set_width(&progress_indic_style, 10);
        lv_style_set_arc_color(&progress_indic_style, lv_color_hex(0x00ff88));

        lv_style_set_text_font(&font_style, &lv_font_montserrat_18);
        lv_style_set_text_color(&font_style, lv_color_hex(0xffffff));
    }
    else {
        //TODO: adjust colors for light theme
        lv_style_set_width(&progress_main_style, 20);
        lv_style_set_arc_color(&progress_main_style, lv_color_hex(0x00ff00));

        lv_style_set_width(&progress_indic_style, 20);
        lv_style_set_arc_color(&progress_indic_style, lv_color_hex(0xff0000));
    }
}

void ui_main_screen(lv_obj_t *parent)
{
    // Initialize systems
    event_init();
    timer_init();
    ui_theme_init();
    
    // Register callbacks
    pomodoro_set_state_callback(pomodoro_state_changed);
    pomodoro_set_tick_callback(ui_tick_cb);

    // Create periodic timer check using LVGL
    lv_timer_create(timer_tick_cb, 1000, NULL);  // Check every 1000ms
    
    /* Grid: 6 rows, 1 column */
    static int col_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int row_dsc[] = {
        LV_GRID_FR(1),     // Mode
        LV_GRID_FR(5),     // Timer + bar
        LV_GRID_FR(1),     // Buttons
        LV_GRID_FR(1),     // Cycle status
        LV_GRID_FR(1),     // Spacer
        LV_GRID_TEMPLATE_LAST
    };

    // Remove all default styles from parent
    lv_obj_remove_style_all(parent);
    
    // Set background color for the parent/screen
    lv_obj_set_style_bg_color(parent, lv_color_hex(0x343247), 0);
    lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(parent, 0, 0);
    lv_obj_set_style_margin_all(parent, 0, 0);
    lv_obj_set_style_border_width(parent, 0, 0);
    
    // Remove scrollbars from parent
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *cont = lv_obj_create(parent);
    
    // Remove all default styles from container
    lv_obj_remove_style_all(cont);
    
    // Set container properties
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x343247), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);
    lv_obj_set_style_margin_all(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);

    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    /* Add some margin top/bottom */
    lv_obj_set_style_pad_top(cont, 12, 0);
    lv_obj_set_style_pad_bottom(cont, 12, 0);
    lv_obj_set_style_pad_row(cont, 8, 0);   // spacing between rows

    /* Mode */
    label_mode = lv_label_create(cont);
    lv_label_set_text(label_mode, "Focus");
    lv_obj_set_grid_cell(label_mode,
                         LV_GRID_ALIGN_CENTER, 0, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_add_style(label_mode, &font_style, 0);
    lv_obj_set_style_text_font(label_mode, &lv_font_montserrat_48, 0);

    /* Timer row */
    lv_obj_t *timer_cont = lv_obj_create(cont);
    lv_obj_remove_style_all(timer_cont);
    lv_obj_clear_flag(timer_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_grid_cell(timer_cont, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);


    /* Circular progress indicator */
    progress = lv_arc_create(timer_cont);
    lv_obj_set_size(progress, 200, 200); //TODO: adjust size based on screen
    lv_obj_center(progress);

    lv_obj_remove_style(progress, NULL, LV_PART_KNOB);   // Remove the knob
    lv_obj_clear_flag(progress, LV_OBJ_FLAG_CLICKABLE);  // Make it non-interactive

    lv_arc_set_range(progress, 0, remaining_sec);
    lv_arc_set_value(progress, remaining_sec);
    lv_arc_set_bg_angles(progress, 0, 360);

    // Set rotation to start from top
    lv_arc_set_rotation(progress, 270);  // Rotate so 0 degrees is at 12 o'clock
    lv_arc_set_mode(progress, LV_ARC_MODE_REVERSE);

    lv_obj_add_style(progress, &progress_main_style, LV_PART_MAIN);
    lv_obj_add_style(progress, &progress_indic_style, LV_PART_INDICATOR);

    /* Timer label - positioned in center of circle */
    label_timer = lv_label_create(progress);  // Create as child of arc for centering
    lv_label_set_text(label_timer, "25:00");
    lv_obj_set_style_text_font(label_timer, &lv_font_montserrat_28, 0);
    lv_obj_center(label_timer);  // Center within the arc

    label_pause = lv_label_create(timer_cont);
    lv_label_set_text(label_pause, "Paused>");
    lv_obj_add_style(label_pause, &font_style, 0);
    lv_obj_align_to(label_pause, label_timer, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_add_flag(label_pause, LV_OBJ_FLAG_HIDDEN);
    

    /* Buttons */
    lv_obj_t *btn_row = lv_obj_create(cont);
    lv_obj_remove_style_all(btn_row);
    lv_obj_clear_flag(btn_row, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(btn_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_row,
                          LV_FLEX_ALIGN_SPACE_EVENLY,
                          LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_grid_cell(btn_row,
                         LV_GRID_ALIGN_STRETCH, 0, 1,
                         LV_GRID_ALIGN_STRETCH, 2, 1);

    btn_start = lv_btn_create(btn_row);
    lv_obj_set_size(btn_start, 90, 40);
    lv_obj_add_event_cb(btn_start, start_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *label_start = lv_label_create(btn_start);
    lv_label_set_text(label_start, "Start");
    lv_obj_center(label_start);

    btn_reset = lv_btn_create(btn_row);
    lv_obj_set_size(btn_reset, 90, 40);
    lv_obj_add_event_cb(btn_reset, reset_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *label_reset = lv_label_create(btn_reset);
    lv_label_set_text(label_reset, "Reset");
    lv_obj_center(label_reset);

    /* Cycle status */
    label_cycle = lv_label_create(cont);
    lv_label_set_text(label_cycle, "Cycle: 0 / 4");
    lv_obj_set_grid_cell(label_cycle,
                         LV_GRID_ALIGN_CENTER, 0, 1,
                         LV_GRID_ALIGN_CENTER, 3, 1);

    /* Update initial timer display */
    update_timer_label(remaining_sec * 1000);

    // Initialize to IDLE state
    pomodoro_state_changed(POMODORO_IDLE);
}

/* --- Callbacks --- */

static void update_timer_label(uint32_t remaining_ms)
{
    uint32_t total_seconds = remaining_ms / 1000;
    uint32_t minutes = total_seconds / 60;
    uint32_t seconds = total_seconds % 60;

    static char buf[8];
    lv_snprintf(buf, sizeof(buf), "%02d:%02d", minutes, seconds);
    lv_label_set_text(label_timer, buf);

    // Update progress bar using seconds
    lv_arc_set_value(progress, total_seconds);
}

static void ui_update_by_state(PomodoroState_e state)
{
    if(state == POMODORO_IDLE) {
        lv_obj_add_flag(btn_reset, LV_OBJ_FLAG_HIDDEN); // Hide Reset
        lv_obj_set_align(btn_start, LV_ALIGN_CENTER);   // Center Start button
    }
    else if (state == POMODORO_PAUSED_WORK || state == POMODORO_PAUSED_BREAK) {
        lv_obj_clear_flag(label_pause, LV_OBJ_FLAG_HIDDEN); // Show "Paused" label
    }
    else {
        lv_obj_clear_flag(btn_reset, LV_OBJ_FLAG_HIDDEN); // Show Reset
        lv_obj_set_align(btn_start, LV_ALIGN_LEFT_MID);   // Align Start to left
        lv_obj_set_align(btn_reset, LV_ALIGN_RIGHT_MID);  // Align Reset to right
        lv_obj_add_flag(label_pause, LV_OBJ_FLAG_HIDDEN);
    }
}

static void pomodoro_state_changed(PomodoroState_e state)
{
    ui_update_by_state(state);
    // Update mode label based on state
    switch (state) {
        case POMODORO_IDLE:
            lv_label_set_text(label_mode, "Ready");
            lv_label_set_text(lv_obj_get_child(btn_start, 0), "Start");
            break;
            
        case POMODORO_WORK:
            lv_label_set_text(label_mode, "Focus");
            lv_label_set_text(lv_obj_get_child(btn_start, 0), "Pause");
            break;
            
        case POMODORO_SHORT_BREAK:
            lv_label_set_text(label_mode, "Short Break");
            lv_label_set_text(lv_obj_get_child(btn_start, 0), "Pause");
            break;
            
        case POMODORO_LONG_BREAK:
            lv_label_set_text(label_mode, "Long Break");
            lv_label_set_text(lv_obj_get_child(btn_start, 0), "Pause");
            break;
            
        case POMODORO_PAUSED_WORK:
            lv_label_set_text(lv_obj_get_child(btn_start, 0), "Resume");
            // Keep current remaining time
            update_timer_label(timer_get_remaining());
            break;
            
        case POMODORO_PAUSED_BREAK:
            lv_label_set_text(lv_obj_get_child(btn_start, 0), "Resume");
            // Keep current remaining time
            update_timer_label(timer_get_remaining());
            break;
    }

    // Update remaining time and progress bar
    remaining_sec = pomodoro_get_remaining_sec();
    update_timer_label(remaining_sec); 

    // Update cycle counter
    char buf[32];
    snprintf(buf, sizeof(buf), "Cycle: %d / %d", 
             pomodoro_get_current_cycle(),
             POMODORO_DEF_CYCLES_BEFORE_LONG);
    lv_label_set_text(label_cycle, buf);
}

static void start_event_cb(lv_event_t *e)
{
    PomodoroState_e current_state = pomodoro_get_state();
    
    switch (current_state) {
        case POMODORO_IDLE:
            // Start new work session
            event_dispatch(EVENT_START, NULL);
            break;
            
        case POMODORO_WORK:
        case POMODORO_SHORT_BREAK:
        case POMODORO_LONG_BREAK:
            timer_pause();
            event_dispatch(EVENT_PAUSE, NULL);
            break;
            
        case POMODORO_PAUSED_WORK:
        case POMODORO_PAUSED_BREAK:
            timer_resume();
            event_dispatch(EVENT_RESUME, NULL);
            break;
    }
}

static void reset_event_cb(lv_event_t *e)
{
    event_dispatch(EVENT_RESET, NULL);
    timer_stop();
    pomodoro_state_changed(POMODORO_IDLE); // Force UI update
}

static void ui_tick_cb(uint32_t remaining) {
    LV_LOG_USER("[UI] Remaining: %02lu:%02lu\n", remaining / 60, remaining % 60);
    update_timer_label(remaining);
}

static void timer_tick_cb(lv_timer_t * timer) {
    timer_tick_handler();
}