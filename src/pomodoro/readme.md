## Main UI Screen

```
 ---------------------------------
|          POMODORO APP           |
|---------------------------------|
|          [ Work Mode ]          | <- State label
|                                 |
|           ( 25:00 )             | <- Timer label
|        ⭕ Progress Arc ⭕         |
|                                 |
| [ Start ] [ Pause ] [ Reset ]   | <- Buttons
|                                 |
|         Cycle: 1 / 4            | <- Status
 ---------------------------------

```

## High level Structure
```
Pomodoro App
│
├─ UI   <- Responsible for rendering and interaction
│   ├─ main_screen.c/h      <- Main Pomodoro UI: timer label, progress arc, buttons, status label
│   ├─ settings_screen.c/h  <- Optional: change work/break duration, cycles, theme
│   └─ ui_helpers.c/h       <- Utility functions: create buttons, labels, arcs, common styles
│
├─ Core     <- Handles timer and state machine
│   ├─ pomodoro.c/h    <- State machine: WORK / SHORT_BREAK / LONG_BREAK
│   ├─ timer.c/h       <- Countdown logic, tick callback
│   └─ event.c/h       <- Events from UI: start/pause/reset, state changes
│
├─ HAL  <- MCU-specific, hardware interaction
│   ├─ lcd.c/h         <- Initialize LCD, LVGL flush
│   ├─ touch.c/h       <- Read touch input
│   └─ buzzer.c/h      <- Trigger buzzer when timer ends
│
└─ main.c             <- Initialize LVGL, hardware, call UI and Core logic

```

## Podomoro State Machine
### Step 1: Define the states
| State          | Description                     |
| -------------- | ------------------------------- |
| `IDLE`         | Timer not started               |
| `WORK`         | Working session                 |
| `SHORT_BREAK`  | Short break after work          |
| `LONG_BREAK`   | Long break after several cycles |
| `PAUSED_WORK`  | Work paused                     |
| `PAUSED_BREAK` | Break paused (short/long)       |

### Step 2: Define events
| Event           | Source     | Description                    |
| --------------- | ---------- | ------------------------------ |
| `start`         | UI         | Start a work session from IDLE |
| `pause`         | UI         | Pause the current session      |
| `resume`        | UI         | Resume a paused session        |
| `reset`         | UI         | Reset timer to IDLE            |
| `timer_expired` | Timer/Core | Timer finished counting down   |

### Step 3: Draw transitions

```
[IDLE] --start--> [WORK]
[WORK] --timer_expired--> [SHORT_BREAK] (if not last cycle)
[WORK] --timer_expired--> [LONG_BREAK]  (if last cycle)
[WORK] --pause--> [PAUSED_WORK]
[PAUSED_WORK] --resume--> [WORK]
[SHORT_BREAK] --pause--> [PAUSED_BREAK]
[LONG_BREAK] --pause--> [PAUSED_BREAK]
[PAUSED_BREAK] --resume--> [SHORT_BREAK or LONG_BREAK]
[SHORT_BREAK or LONG_BREAK] --timer_expired--> [WORK]
[WORK/SHORT_BREAK/LONG_BREAK/PAUSED_*] --reset--> [IDLE]
```

### Step 4: Cycle count
- Keep track of number of work sessions completed.
- Example: After 4 work sessions, go to LONG_BREAK instead of SHORT_BREAK.
- This can be internal to Pomodoro module, UI doesn’t need to know.

### Step 5: Map states & transitions to API
| API Call               | Effect / Transition                                           |
| ---------------------- | ------------------------------------------------------------- |
| `pomodoro_start()`     | IDLE → WORK                                                   |
| `pomodoro_pause()`     | WORK → PAUSED_WORK, SHORT_BREAK/LONG_BREAK → PAUSED_BREAK |
| `pomodoro_resume()`    | PAUSED_WORK → WORK, PAUSED_BREAK → SHORT/LONG_BREAK        |
| `pomodoro_reset()`     | Any state → IDLE                                              |
| Timer tick callback    | Update remaining time in WORK/SHORT_BREAK/LONG_BREAK        |
| Timer expired callback | WORK → SHORT/LONG_BREAK, SHORT/LONG_BREAK → WORK            |

### Step 6: Optional: State callback for UI
```c
typedef void (*pomodoro_state_cb_t)(PomodoroState_e state);
typedef void (*pomodoro_tick_cb_t)(uint32_t remaining_sec);

void pomodoro_set_state_callback(pomodoro_state_cb_t cb);
void pomodoro_set_tick_callback(pomodoro_tick_cb_t cb);
```

### Step 7: Visual Diagram (ASCII)
```
      +--------+
      |  IDLE  |
      +--------+
          |
          | start
          v
       +------+
       | WORK |<------------------+
       +------+                   |
       |timer_expired              | resume
       v                           ^
+--------------+                  +-----------+
| SHORT_BREAK  |----------------->| PAUSED_*  |
+--------------+  pause            +----------+
       | timer_expired                  |
       v                                |
      WORK <----------------------------
       |
       | (after 4 cycles)
       v
   LONG_BREAK
       |
       v
      WORK
```

