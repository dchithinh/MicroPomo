#include <stdio.h>
#include "lvgl.h"
#include "pomodoro.h"
#include "timer.h"

// ====================== Internal State ======================
static PomodoroState_e pomo_core_current_state = POMODORO_IDLE;
static PomodoroState_e pomo_core_previous_state = POMODORO_IDLE;
static uint32_t pomo_core_remaining_ms = POMODORO_DEF_WORK_MIN * 60 * 1000;  /**< Remaining milliseconds in current session */
static uint8_t pomo_core_cycle_count = 0;  /**< Work sessions completed */

static uint32_t pomo_core_work_duration_ms = POMODORO_DEF_WORK_MIN * 60 * 1000;
static uint32_t pomo_core_short_break_duration_ms = POMODORO_DEF_SHORT_BREAK_MIN * 60 * 1000;
static uint32_t pomo_core_long_break_duration_ms = POMODORO_DEF_LONG_BREAK_MIN * 60 * 1000;
static uint8_t pomo_core_max_cycles = POMODORO_DEF_CYCLES_BEFORE_LONG;

// Callbacks
static pomodoro_state_cb_t pomo_core_state_cb = NULL;
static pomodoro_tick_cb_t pomo_core_tick_cb = NULL;

// ====================== Private Functions ======================
/**
 * @brief Change state internally and trigger callback
 * @param new_state New Pomodoro state
 * @param duration_ms Duration for the new state
 */
static void change_state(PomodoroState_e new_state, uint32_t duration_ms) {
    pomo_core_previous_state = pomo_core_current_state;
    pomo_core_current_state = new_state;
    pomo_core_remaining_ms = duration_ms;

    if (pomo_core_state_cb) {
        pomo_core_state_cb(pomo_core_current_state); //UI callback to update display
    }
}

// Timer tick callback
static void on_timer_tick(uint32_t remaining_ms) 
{
    pomo_core_remaining_ms = remaining_ms;  // Store current remaining time
    LV_LOG_USER("[Pomodoro] Timer tick: %d ms remaining\n", pomo_core_remaining_ms);
    if (pomo_core_tick_cb) {
        pomo_core_tick_cb(pomo_core_remaining_ms);  // Pass current remaining time to UI
    }
}

// Timer finished callback
static void on_timer_finished(void) {
    LV_LOG_USER("[Pomodoro] Timer finished in state %d\n", pomo_core_current_state);
    if (pomo_core_current_state == POMODORO_WORK) {
        pomo_core_cycle_count++;
        if (pomo_core_cycle_count % pomo_core_max_cycles == 0) {
            change_state(POMODORO_LONG_BREAK, pomo_core_long_break_duration_ms);
            timer_start(pomo_core_long_break_duration_ms, on_timer_tick, on_timer_finished);
        } else {
            change_state(POMODORO_SHORT_BREAK, pomo_core_short_break_duration_ms);
            timer_start(pomo_core_short_break_duration_ms, on_timer_tick, on_timer_finished);
        }
    } else { // Break finished
        change_state(POMODORO_WORK, pomo_core_work_duration_ms);
        timer_start(pomo_core_work_duration_ms, on_timer_tick, on_timer_finished);
    }
}

void pomodoro_init(uint32_t work_min, uint32_t short_break_min,
                   uint32_t long_break_min, uint8_t cycles_before_long) {
    pomo_core_work_duration_ms = work_min * 60 * 1000;
    pomo_core_short_break_duration_ms = short_break_min * 60 * 1000;
    pomo_core_long_break_duration_ms = long_break_min * 60 * 1000;
    pomo_core_max_cycles = cycles_before_long;
    pomo_core_cycle_count = 0;
    pomo_core_current_state = POMODORO_IDLE;
    pomo_core_remaining_ms = pomo_core_work_duration_ms;
}

void pomodoro_start(void) {
    if (pomo_core_current_state == POMODORO_IDLE) {
        change_state(POMODORO_WORK, pomo_core_work_duration_ms);
        timer_start(pomo_core_work_duration_ms, on_timer_tick, on_timer_finished);
    }
}

void pomodoro_pause(void)
{
    // Follow state diagram transitions
    switch (pomo_core_current_state) {
        case POMODORO_WORK:
            change_state(POMODORO_PAUSED_WORK, timer_get_remaining());
            timer_pause();
            break;
            
        case POMODORO_SHORT_BREAK:
        case POMODORO_LONG_BREAK:
            change_state(POMODORO_PAUSED_BREAK, timer_get_remaining());
            timer_pause();
            break;
    }
}


void pomodoro_resume(void) {
    if (pomo_core_current_state == POMODORO_PAUSED_WORK) {
        change_state(POMODORO_WORK, pomo_core_remaining_ms);
        timer_resume();
    } else if (pomo_core_current_state == POMODORO_PAUSED_BREAK) {
        if (pomo_core_cycle_count % pomo_core_max_cycles == 0) {
            change_state(POMODORO_LONG_BREAK, pomo_core_remaining_ms);
        } else {
            change_state(POMODORO_SHORT_BREAK, pomo_core_remaining_ms);
        }
        timer_resume();
    }
}


void pomodoro_reset(void) {
    change_state(POMODORO_IDLE, 0);
    pomo_core_cycle_count = 0;
    timer_stop();
}

PomodoroState_e pomodoro_get_state(void) {
    return pomo_core_current_state;
}

uint32_t pomodoro_get_remaining_sec(void) 
{
    return pomo_core_remaining_ms / 1000;  // Convert to seconds for API
}

void pomodoro_set_state_callback(pomodoro_state_cb_t cb)
{
    pomo_core_state_cb = cb;
}

void pomodoro_set_tick_callback(pomodoro_tick_cb_t cb)
{
    pomo_core_tick_cb = cb;
}

uint8_t pomodoro_get_current_cycle(void)
{
    return pomo_core_cycle_count;
}

uint8_t pomodoro_get_max_cycles(void)
{
    return pomo_core_max_cycles;
}

bool pomodoro_is_resume_transition(void)
{
    // True if previous_state was PAUSED_WORK and current_state is WORK,
    // or previous_state was PAUSED_BREAK and current_state is SHORT_BREAK or LONG_BREAK
    return ((pomo_core_previous_state == POMODORO_PAUSED_WORK && pomo_core_current_state == POMODORO_WORK) ||
            (pomo_core_previous_state == POMODORO_PAUSED_BREAK && 
               (pomo_core_current_state == POMODORO_SHORT_BREAK || pomo_core_current_state == POMODORO_LONG_BREAK)));
}

bool pomodoro_is_pause_transition(void)
{
    return (
        (pomo_core_current_state == POMODORO_PAUSED_WORK ||
         pomo_core_current_state == POMODORO_PAUSED_BREAK)
    );
}

void pomodoro_update_durations(uint32_t work_min, uint32_t short_break_min,
                               uint32_t long_break_min, uint8_t cycles_before_long)
{
    pomo_core_work_duration_ms = work_min * 60 * 1000;
    pomo_core_short_break_duration_ms = short_break_min * 60 * 1000;
    pomo_core_long_break_duration_ms = long_break_min * 60 * 1000;
    pomo_core_max_cycles = cycles_before_long;

    // If currently idle, update remaining_ms to new work duration
    if (pomo_core_current_state == POMODORO_IDLE) {
        pomo_core_remaining_ms = pomo_core_work_duration_ms;
    }
}

int pomodoro_get_work_time(void)
{
    return pomo_core_work_duration_ms;
}

int pomodoro_get_short_break(void)
{
    return pomo_core_short_break_duration_ms;
}

int pomodoro_get_long_break(void)
{
    return pomo_core_long_break_duration_ms;
}

int pomodoro_get_cycle_count(void)
{
   return pomo_core_max_cycles;
}

uint8_t pomodoro_get_work_progress_in_percent(void)
{
    uint8_t percent = 0;
    if (pomo_core_work_duration_ms == 0) return 0;

    percent = (uint8_t)(((pomo_core_work_duration_ms - pomo_core_remaining_ms) * 100) / pomo_core_work_duration_ms);
    LV_LOG_USER("[Pomodoro] Remaining msec: %d, Work duration msec: %d\n",
                    pomo_core_remaining_ms , pomo_core_work_duration_ms);
    return percent;
}