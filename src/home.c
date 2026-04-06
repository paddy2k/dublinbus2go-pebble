#include "home.h"
#include "loading.h"
#include "stoplist.h"
#include "app_message.h"
#include <pebble.h>

#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ITEMS 4

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GBitmap *s_res_dublin_bus_logo;
static GBitmap *s_menu_icons[NUM_MENU_ITEMS];
static BitmapLayer *s_bitmaplayer_1;
static MenuLayer *s_menulayer_1;
static Layer *s_circle_layer;

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = GPoint(bounds.size.w / 2, bounds.size.h / 2);
  uint16_t radius = bounds.size.h / 2;

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, radius);
}

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return NUM_MENU_ITEMS;
    default:
      return 0;
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  switch (cell_index->section) {
    case 0:
      switch (cell_index->row) {
        case 0:
          menu_cell_basic_draw(ctx, cell_layer, "Favourites", NULL, s_menu_icons[0]);
          break;
        case 1:
          menu_cell_basic_draw(ctx, cell_layer, "Bus", NULL, s_menu_icons[1]);
          break;
        case 3:
            menu_cell_basic_draw(ctx, cell_layer, "Luas", NULL, s_menu_icons[3]);
            break;
        case 2:
          menu_cell_basic_draw(ctx, cell_layer, "Train", NULL, s_menu_icons[2]);
          break;

      }
      break;
  }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
  stop_list_type = cell_index->row;
  switch (cell_index->row) {
    // This is the menu item with the cycling icon
    case 0:
       show_loading();
       getSavedStops();
      break;
    case 1:
       show_loading();
       getNearestBus();
      break;
    case 2:
       show_loading();
       getNearestTrain();
      break;
    case 3:
       show_loading();
       getNearestTram();
      break;
  }
}

void on_animation_stopped(Animation *anim, bool finished, void *context)
{
  // s_menulayer_1
  GRect menuSize = GRect(0, 63, 144, 90);
  #ifdef PBL_SDK_3
  menuSize = GRect(0, 80, 144, 115);
  #endif

  s_menulayer_1 = menu_layer_create(menuSize);
  menu_layer_set_callbacks(s_menulayer_1, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });
  menu_layer_set_click_config_onto_window(s_menulayer_1, s_window);

  #ifdef PBL_COLOR
  menu_layer_set_normal_colors(s_menulayer_1, GColorFromHEX(0x00A572), GColorWhite);
  menu_layer_set_highlight_colors(s_menulayer_1, GColorWhite, GColorFromHEX(0x00A572));
  #endif

  layer_add_child(window_get_root_layer(s_window), (Layer *)s_menulayer_1);

  //Free the memory used by the Animation
  property_animation_destroy((PropertyAnimation*) anim);
}

void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay)
{
    //Declare animation
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);

    //Set characteristics
    animation_set_duration((Animation*) anim, duration);
    animation_set_delay((Animation*) anim, delay);

    //Set stopped handler to free memory
    AnimationHandlers handlers = {
        //The reference to the stopped handler is the only one in the array
        .stopped = (AnimationStoppedHandler) on_animation_stopped
    };
    animation_set_handlers((Animation*) anim, handlers, NULL);

    //Start animation!
    animation_schedule((Animation*) anim);
}

static void initialise_ui(void) {
  GColor backgroundColour = COLOR_FALLBACK(GColorFromHEX(0x00A572), GColorBlack);

  s_menu_icons[0] = gbitmap_create_with_resource(RESOURCE_ID_FAVOURITES_ICON_BLACK);
  if (!s_menu_icons[0]) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to load favourites icon PNG");
  }
  s_menu_icons[1] = gbitmap_create_with_resource(RESOURCE_ID_NEAREST_ICON_BLACK);
  if (!s_menu_icons[1]) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to load nearest icon PNG");
  }
  s_menu_icons[2] = gbitmap_create_with_resource(RESOURCE_ID_NEAREST_ICON_BLACK);
  if (!s_menu_icons[2]) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to load nearest icon PNG");
  }
  s_menu_icons[3] = gbitmap_create_with_resource(RESOURCE_ID_NEAREST_ICON_BLACK);
  if (!s_menu_icons[3]) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to load nearest icon PNG");
  }

  s_window = window_create();
  window_set_background_color(s_window, backgroundColour);

  s_res_dublin_bus_logo = gbitmap_create_with_resource(RESOURCE_ID_DUBLIN_BUS_LOGO);
  if (!s_res_dublin_bus_logo) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to load Dublin Bus logo PNG in home");
  }

  // s_circle_layer
  #ifdef PBL_SDK_3
  GRect circle_bounds = GRect(47, 57, 50, 50);
  #else
  GRect circle_bounds = GRect(47, 45, 50, 50);
  #endif
  s_circle_layer = layer_create(circle_bounds);
  layer_set_update_proc(s_circle_layer, canvas_update_proc);
  layer_add_child(window_get_root_layer(s_window), s_circle_layer);

  // s_bitmaplayer_1
  #ifdef PBL_SDK_3
  s_bitmaplayer_1 = bitmap_layer_create(GRect(4, 62, 136, 40));
  #else
  s_bitmaplayer_1 = bitmap_layer_create(GRect(4, 50, 136, 40));
  #endif
  if (s_bitmaplayer_1) {
    if (s_res_dublin_bus_logo) {
      bitmap_layer_set_bitmap(s_bitmaplayer_1, s_res_dublin_bus_logo);
      bitmap_layer_set_compositing_mode(s_bitmaplayer_1, GCompOpSet);
    }
    layer_add_child(window_get_root_layer(s_window), (Layer *)s_bitmaplayer_1);
  }
}

static void destroy_ui(void) {
  if (s_menulayer_1) {
    menu_layer_destroy(s_menulayer_1);
    s_menulayer_1 = NULL;
  }
  if (s_bitmaplayer_1) {
    bitmap_layer_destroy(s_bitmaplayer_1);
    s_bitmaplayer_1 = NULL;
  }
  if (s_circle_layer) {
    layer_destroy(s_circle_layer);
    s_circle_layer = NULL;
  }
  if (s_res_dublin_bus_logo) {
    gbitmap_destroy(s_res_dublin_bus_logo);
    s_res_dublin_bus_logo = NULL;
  }
  for ( int i=0; i<NUM_MENU_ITEMS; i++ ) {
    if (s_menu_icons[i]) {
      gbitmap_destroy( s_menu_icons[i] );
      s_menu_icons[i] = NULL;
    }
  }
  if (s_window) {
    window_destroy(s_window);
    s_window = NULL;
  }
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

GBitmap *get_app_logo(void) {
  return s_res_dublin_bus_logo;
}

void show_home(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_home(void) {
  window_stack_remove(s_window, true);
}

void show_ui(void){
  GRect start = GRect(4, 50, 136, 40);
  GRect finish = GRect(4, 13, 136, 40);
  GRect c_start = GRect(47, 45, 50, 50);
  GRect c_finish = GRect(47, 8, 50, 50);
  #ifdef PBL_SDK_3
  start = GRect(4, 60, 136, 40);
  finish = GRect(4, 22, 136, 40);
  c_start = GRect(47, 57, 50, 50);
  c_finish = GRect(47, 19, 50, 50);
  #endif


  animate_layer(bitmap_layer_get_layer(s_bitmaplayer_1), &start, &finish, 300, 0);
  animate_layer(s_circle_layer, &c_start, &c_finish, 300, 0);
}
