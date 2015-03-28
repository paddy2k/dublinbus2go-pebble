#include "home.h"
#include "loading.h"
#include "stoplist.h"
#include "app_message.h"
#include <pebble.h>

#define NUM_MENU_SECTIONS 1
#define NUM_FIRST_MENU_ITEMS 2
#define NUM_MENU_ICONS 2

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GBitmap *s_res_dublin_bus_logo;
static GBitmap *s_menu_icons[NUM_MENU_ICONS];
static BitmapLayer *s_bitmaplayer_1;
static InverterLayer *s_inverterlayer_1;
static InverterLayer *s_inverterlayer_2;
static MenuLayer *s_menulayer_1;

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return NUM_FIRST_MENU_ITEMS;
    default:
      return 0;
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  switch (cell_index->section) {
    case 0:
      switch (cell_index->row) {
        case 0:
          menu_cell_basic_draw(ctx, cell_layer, "Saved Stops", NULL, s_menu_icons[0]);
          break;
        case 1:
          menu_cell_basic_draw(ctx, cell_layer, "Nearest", NULL, s_menu_icons[1]);
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
       show_stoplist();
      break;
    case 1:
       show_loading();
       getNearestStops();
      break;
  }
}

static void initialise_ui(void) {
  GColor backgroundColour = COLOR_FALLBACK(GColorYellow, GColorBlack);
  
  s_menu_icons[0] = gbitmap_create_with_resource(RESOURCE_ID_FAVOURITES_ICON);
  s_menu_icons[1] = gbitmap_create_with_resource(RESOURCE_ID_NEAREST_ICON);
  
  s_window = window_create();
  window_set_background_color(s_window, backgroundColour);
  window_set_fullscreen(s_window, false);
  
  s_res_dublin_bus_logo = gbitmap_create_with_resource(RESOURCE_ID_DUBLIN_BUS_LOGO);
  // s_bitmaplayer_1
  s_bitmaplayer_1 = bitmap_layer_create(GRect(4, 13, 136, 40));
  bitmap_layer_set_bitmap(s_bitmaplayer_1, s_res_dublin_bus_logo);
  //bitmap_layer_set_background_color(s_bitmaplayer_1, GColorBlack);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_bitmaplayer_1);
  
  #ifndef PBL_COLOR
  // s_inverterlayer_1
  s_inverterlayer_1 = inverter_layer_create(GRect(0, 63, 144, 90));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_inverterlayer_1);
  #endif
  
  // s_menulayer_1
  s_menulayer_1 = menu_layer_create(GRect(0, 63, 144, 90));
  menu_layer_set_callbacks(s_menulayer_1, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });
  menu_layer_set_click_config_onto_window(s_menulayer_1, s_window);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_menulayer_1);
  
  #ifndef PBL_COLOR
  // s_inverterlayer_2
  s_inverterlayer_2 = inverter_layer_create(GRect(0, 63, 144, 90));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_inverterlayer_2);
  #endif
}

static void destroy_ui(void) {
  window_destroy(s_window);
  bitmap_layer_destroy(s_bitmaplayer_1);
  menu_layer_destroy(s_menulayer_1);
  gbitmap_destroy(s_res_dublin_bus_logo);
  inverter_layer_destroy(s_inverterlayer_1);
  inverter_layer_destroy(s_inverterlayer_2);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
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
