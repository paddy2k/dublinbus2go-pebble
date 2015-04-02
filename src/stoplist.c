#include "Stop.h"
#include "stoplist.h"
#include "stop.h"
#include "loading.h"
#include "app_message.h"
#include "saved.h"
#include "removed.h"
#include <pebble.h>

#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ITEMS 5
#define NUM_STOPS_IN_LIST 20
  
static Window *s_window;
static MenuLayer *s_menulayer_1;
int stop_list_type = 0;
int stop_list[NUM_STOPS_IN_LIST];

static Stop *stops[NUM_STOPS_IN_LIST] = {};

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  int rows = 0;
  int stopsSize = sizeof stops / sizeof stops[0];
  for(int i = 0; i<stopsSize; i++){
    if(stops[i]){
     rows++; 
    }  
  }
  
  return rows;
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  graphics_context_set_text_color(ctx, GColorWhite); // This is important.
  switch(stop_list_type){
    case 0: 
      menu_cell_basic_header_draw(ctx, cell_layer, "Saved Stops");
      break;
    default: 
      menu_cell_basic_header_draw(ctx, cell_layer, "Nearest Stops");
      break;
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  if(stops[cell_index->row]){
    Stop *stop = stops[cell_index->row];
    stop_list[cell_index->row] = atoi(stop->id);
    char buffer[20] = "                ";
    int bufferSize = 24;
    bufferSize = bufferSize - strlen(stop->distance);
    bufferSize = bufferSize - strlen(stop->id);
    switch(bufferSize){
      case 8:
        snprintf(buffer, sizeof buffer, "%s", "  ");
        break;
      case 9:
        snprintf(buffer, sizeof buffer, "%s", "    ");
        break;
      case 10:
        snprintf(buffer, sizeof buffer, "%s", "      ");
        break;
      case 11:
        snprintf(buffer, sizeof buffer, "%s", "        ");
        break;
      case 12:
        snprintf(buffer, sizeof buffer, "%s", "          ");
        break;
      case 13:
        snprintf(buffer, sizeof buffer, "%s", "           ");
        break;
      case 14:
        snprintf(buffer, sizeof buffer, "%s", "            ");
        break;
      case 15:
        snprintf(buffer, sizeof buffer, "%s", "             ");
        break;
      case 16:
        snprintf(buffer, sizeof buffer, "%s", "              ");
        break;
      case 17:
        snprintf(buffer, sizeof buffer, "%s", "               ");
        break;
      default:
        snprintf(buffer, sizeof buffer, "%s", "                ");
        break;
    }
    
    char bottom[256];
    snprintf(bottom, sizeof bottom, "#%s%s%s %s", stop->id, buffer, stop->distance, stop->bearing);
  
    menu_cell_basic_draw(ctx, cell_layer, stop->name, bottom, NULL);
  }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  Stop *stop = stops[cell_index->row];
  set_stop_name(stop->name);
  getStop(stop->id);
  show_loading();  
}

static void menu_select_long_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  Stop *stop = stops[cell_index->row];

  switch(stoplist_type){
    case 0:
      removeStop(stop->id);
      show_removed();
      hide_stoplist();
      break;
    case 1:
      saveStop(stop->id);
      show_saved();
      break;
  }
}


static void initialise_ui(void) {
  GColor backgroundColour = COLOR_FALLBACK(GColorYellow, GColorWhite);
  
  s_window = window_create();
  window_set_background_color(s_window, backgroundColour);
  window_set_fullscreen(s_window, false);
  
  s_menulayer_1 = menu_layer_create(GRect(0, 0, 144, 152));
  
  menu_layer_set_callbacks(s_menulayer_1, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
    .select_long_click = menu_select_long_callback,
  });
  
  menu_layer_set_click_config_onto_window(s_menulayer_1, s_window);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_menulayer_1);
  
  stoplist_type = stop_list_type;
}

static void destroy_ui(void) {  
  int stopsSize = sizeof stops / sizeof stops[0];
  for(int i = 0; i<stopsSize; i++){
    if(stops[i]){
      stop_destroy(stops[i]);
      stops[i] = NULL;
      free(stops[i]);
    }
  }
  
  window_destroy(s_window);
  menu_layer_destroy(s_menulayer_1);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_stoplist(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_stoplist(void) {
  window_stack_remove(s_window, true);
}

void stoplist_add_stop(int index, const char *name, int id, const char *distance, const char *bearing){
  Stop *stop = stop_create(
    name, 
    id,
    distance,
    bearing
  );
  stops[index] = stop;
}