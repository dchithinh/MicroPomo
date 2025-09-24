#include "lvgl.h"
#include "demo.h"
#include "lvgl.h"
#include "lvgl.h"


static void anim_y_cb(void * var, int32_t v)
{
    lv_obj_set_y((lv_obj_t *)var, v);
    lv_obj_set_x((lv_obj_t *)var, v);
}

void lv_example_flip(lv_obj_t * parent)
{
    lv_obj_t *upper_page = lv_obj_create(parent);
    lv_obj_set_style_bg_color(upper_page, lv_color_hex(0xfe00e0), 0);
    lv_obj_set_align(upper_page, LV_ALIGN_CENTER);


    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, upper_page);
    lv_anim_set_values(&a, 0, 90);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a, anim_y_cb);
    lv_anim_start(&a);
}



void demo_load_gif(lv_obj_t * parent)
{
  lv_obj_t * gif = lv_gif_create(parent);
  lv_gif_set_src(gif, "S:/run_target.gif");
  lv_obj_align(gif , LV_ALIGN_CENTER, 0, 0);
}

void demo_widget(lv_obj_t * parent)
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