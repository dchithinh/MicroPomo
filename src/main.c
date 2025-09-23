/**
 * @file main.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#ifndef _DEFAULT_SOURCE
  #define _DEFAULT_SOURCE /* needed for usleep() */
#endif

#include <stdlib.h>
#include <stdio.h>
#ifdef _MSC_VER
  #include <Windows.h>
#else
  #include <unistd.h>
  #include <pthread.h>
#endif
#include "lvgl/lvgl.h"
#include "lvgl/examples/lv_examples.h"
#include "lvgl/demos/lv_demos.h"
#include <SDL.h>

#include "hal/hal.h"
#include "main_screen.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
#define LCD_WIDTH  480
#define LCD_HEIGHT 480

// #define DEMO_WIDGET 1

static void demo_load_gif(lv_obj_t * parent);
static void demo_widget(lv_obj_t * parent);
static void demo_screen_anim_transis(lv_obj_t *parent);
static void demo_load_img(lv_obj_t * parent);

int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  /*Initialize LVGL*/
  lv_init();

  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  sdl_hal_init(LCD_WIDTH, LCD_HEIGHT);

  #ifndef DEMO_WIDGET
    ui_main_screen(lv_screen_active());
  #else

    // demo_widget(lv_screen_active());
    demo_load_img(lv_screen_active());
    // demo_load_gif(lv_screen_active());

  #endif

  while(1) {
    /* Periodically call the lv_task handler.
     * It could be done in a timer interrupt or an OS task too.*/
    uint32_t sleep_time_ms = lv_timer_handler();
    if(sleep_time_ms == LV_NO_TIMER_READY){
	    sleep_time_ms =  LV_DEF_REFR_PERIOD;
    }
#ifdef _MSC_VER
    Sleep(sleep_time_ms);
#else
    usleep(sleep_time_ms * 1000);
#endif

  }

  return 0;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/
void static demo_load_gif(lv_obj_t * parent)
{
  lv_obj_t * gif = lv_gif_create(parent);
  lv_gif_set_src(gif, "S:/png/run_target.gif");
  lv_obj_align(gif , LV_ALIGN_CENTER, 0, 0);
}

void static demo_widget(lv_obj_t * parent)
{ 

  static int col_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
  static int row_dsc[] = {
      LV_GRID_FR(1), //State
      LV_GRID_FR(2), //Timer + arc
      LV_GRID_FR(1), //Buttons 
      LV_GRID_FR(1), //Cycle
      LV_GRID_TEMPLATE_LAST
  };


  lv_obj_t * grid_cont = lv_obj_create(parent);
  lv_obj_set_size(grid_cont, LV_PCT(100), LV_PCT(100));
  lv_obj_set_grid_dsc_array(grid_cont, col_dsc, row_dsc);

  lv_obj_t *state_obj = lv_obj_create(grid_cont);
  lv_obj_set_grid_cell(state_obj, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

  lv_obj_t * state = lv_label_create(state_obj);
  lv_label_set_text(state, "WORK");
  lv_obj_align(state, LV_ALIGN_CENTER, 0, 0);

  lv_obj_t * timer_obj = lv_obj_create(grid_cont);
  lv_obj_set_grid_cell(timer_obj, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
  lv_obj_t * timer_count = lv_label_create(timer_obj);
  lv_label_set_text(timer_count, "00:00");
  lv_obj_align(timer_count, LV_ALIGN_CENTER, 0, 0);

  lv_obj_t * buttons = lv_obj_create(grid_cont);
  lv_obj_set_grid_cell(buttons, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
  lv_obj_t * buttons_label = lv_label_create(buttons);
  lv_label_set_text(buttons_label, "Buttons");
  lv_obj_align(buttons_label, LV_ALIGN_CENTER, 0, 0);

  lv_obj_t * cycle = lv_obj_create(grid_cont);
  lv_obj_set_grid_cell(cycle, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 3, 1);
  lv_obj_t * cycle_label = lv_label_create(cycle);
  lv_label_set_text(cycle_label, "Cycle");

}

/**********************
 *   STATIC FUNCTIONS
 **********************/
lv_obj_t *screen1 = NULL;
lv_obj_t *screen2 = NULL;

// Custom exec callback for LVGL animation
static void anim_set_opa_cb(void *var, int32_t value)
{
    lv_obj_set_style_opa((lv_obj_t *)var, value, LV_PART_MAIN);
}

void transis_cb(lv_event_t *e)
{
    LV_LOG_USER("Transitioning screens");
    lv_obj_clear_flag(screen2, LV_OBJ_FLAG_HIDDEN);
    
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, screen2);
    lv_anim_set_values(&a, LV_OPA_0, LV_OPA_COVER);
    lv_anim_set_exec_cb(&a, anim_set_opa_cb);
    lv_anim_set_time(&a, 5000);
    lv_anim_start(&a);
    lv_obj_move_foreground(screen2);
}

static void demo_screen_anim_transis(lv_obj_t *parent)
{
    screen1 = lv_obj_create(parent);
    lv_obj_set_size(screen1, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(screen1, lv_color_hex(0x300000), 0);

    lv_obj_t *button1 = lv_btn_create(screen1);
    lv_obj_t* label1 = lv_label_create(button1);
    lv_label_set_text(label1, "Transition");
    lv_obj_center(button1);

    lv_obj_add_event_cb(button1, transis_cb, LV_EVENT_CLICKED, NULL);

    screen2 = lv_obj_create(parent);
    lv_obj_set_size(screen2, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(screen2, lv_color_hex(0x005000), 0);
    lv_obj_add_flag(screen2, LV_OBJ_FLAG_HIDDEN);

    
}

static void demo_load_img(lv_obj_t * parent)
{
  lv_obj_t* settings_icon_img = lv_img_create(parent);
  lv_img_set_src(settings_icon_img, "S:/png/settings_icon.png");
  lv_obj_set_align(settings_icon_img, LV_ALIGN_CENTER);

}