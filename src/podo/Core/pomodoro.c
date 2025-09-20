#include <stdio.h>
#include "pomodoro.h"
#include "timer.h"

// ====================== Internal State ======================
static PomodoroState_e current_state = POMODORO_IDLE;
static uint32_t remaining_ms = POMODORO_DEF_WORK_MIN * 60 * 1000;  /**< Remaining milliseconds in current session */
static uint8_t cycle_count = 0;  /**< Work sessions completed */

static uint32_t work_duration = POMODORO_DEF_WORK_MIN * 60 * 1000;
static uint32_t short_break_duration = POMODORO_DEF_SHORT_BREAK_MIN * 60 * 1000;
static uint32_t long_break_duration = POMODORO_DEF_LONG_BREAK_MIN * 60 * 1000;
static uint8_t max_cycles = POMODORO_DEF_CYCLES_BEFORE_LONG;

// Callbacks
static pomodoro_state_cb_t state_cb = NULL;
static pomodoro_tick_cb_t tick_cb = NULL;

// ====================== Private Functions ======================
/**
 * @brief Change state internally and trigger callback
 * @param new_state New Pomodoro state
 * @param duration_sec Duration for the new state
 */
static void change_state(PomodoroState_e new_state, uint32_t duration_sec) {
    current_state = new_state;
    remaining_ms = duration_sec * 1000;

    if (state_cb) state_cb(current_state); //UI callback to update display
}

// Timer tick callback
static void on_timer_tick(uint32_t remaining_ms) 
{
    remaining_ms = remaining_ms;  // Store current remaining time
    if (tick_cb) {
        tick_cb(remaining_ms);  // Pass current remaining time to UI
    }
}

// Timer finished callback
static void on_timer_finished(void) {
    if (current_state == POMODORO_WORK) {
        cycle_count++;
        if (cycle_count % max_cycles == 0) {
            change_state(POMODORO_LONG_BREAK, long_break_duration);
            timer_start(long_break_duration, on_timer_tick, on_timer_finished);
        } else {
            change_state(POMODORO_SHORT_BREAK, short_break_duration);
            timer_start(short_break_duration, on_timer_tick, on_timer_finished);
        }
    } else { // Break finished
        change_state(POMODORO_WORK, work_duration);
        timer_start(work_duration, on_timer_tick, on_timer_finished);
    }
}

void pomodoro_init(uint32_t work_min, uint32_t short_break_min,
                   uint32_t long_break_min, uint8_t cycles_before_long) {
    work_duration = work_min * 60 * 1000;
    short_break_duration = short_break_min * 60 * 1000;
    long_break_duration = long_break_min * 60 * 1000;
    max_cycles = cycles_before_long;
    cycle_count = 0;
    current_state = POMODORO_IDLE;
    remaining_ms = 0;
}

void pomodoro_start(void) {
    if (current_state == POMODORO_IDLE) {
        change_state(POMODORO_WORK, work_duration);
        timer_start(work_duration, on_timer_tick, on_timer_finished);
    }
}

void pomodoro_pause(void)
{
    // Follow state diagram transitions
    switch (current_state) {
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
    if (current_state == POMODORO_PAUSED_WORK) {
        change_state(POMODORO_WORK, remaining_ms / 1000);
        timer_resume();
    } else if (current_state == POMODORO_PAUSED_BREAK) {
        if (cycle_count % max_cycles == 0) {
            change_state(POMODORO_LONG_BREAK, remaining_ms / 1000);
        } else {
            change_state(POMODORO_SHORT_BREAK, remaining_ms / 1000);
        }
        timer_resume();
    }
}


void pomodoro_reset(void) {
    change_state(POMODORO_IDLE, 0);
    cycle_count = 0;
    timer_stop();
}

PomodoroState_e pomodoro_get_state(void) {
    return current_state;
}

uint32_t pomodoro_get_remaining_sec(void) 
{
    return remaining_ms / 1000;  // Convert to seconds for API
}

void pomodoro_set_state_callback(pomodoro_state_cb_t cb) {
    state_cb = cb;
}

void pomodoro_set_tick_callback(pomodoro_tick_cb_t cb) {
    tick_cb = cb;
}

uint8_t pomodoro_get_current_cycle(void) {
    return cycle_count;
}

