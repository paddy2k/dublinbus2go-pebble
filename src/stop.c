#include <pebble.h>
#include "stop.h"
#include "Bus.h"
#include "BusLayer.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
#define NUM_BUSES_IN_LIST 10
static Bus *buses[NUM_BUSES_IN_LIST] = {};
  
static Window *s_window;
static GFont s_res_gothic_18;
static GFont s_res_gothic_18_bold;
static TextLayer *s_headerlayer_1;
static TextLayer *s_headerlayer_2;
static ScrollLayer *s_scroll_layer;

static BusLayer *bus_layers[NUM_BUSES_IN_LIST] = {};

static void initialise_ui(const char *name) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_fullscreen(s_window, false);
  GRect bounds = GRect(0, 24, 144, 148);
  
  s_scroll_layer = scroll_layer_create(bounds);
  scroll_layer_set_click_config_onto_window(s_scroll_layer, s_window);
  
  s_res_gothic_18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  s_res_gothic_18_bold = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  
  
  // s_headerlayer_1
  s_headerlayer_1 = text_layer_create(GRect(0, 0, 144, 22));
  text_layer_set_text(s_headerlayer_1, "                                       Due");
  text_layer_set_font(s_headerlayer_1, s_res_gothic_18);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_headerlayer_1);
  
  // s_headerlayer_2
  s_headerlayer_2 = text_layer_create(GRect(2, 0, 100, 20));
  text_layer_set_background_color(s_headerlayer_2, GColorClear);
  text_layer_set_text(s_headerlayer_2, name);
  text_layer_set_font(s_headerlayer_2, s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_headerlayer_2);
  
  
//   int step_offset = 0;
//   int step_size = 24;
//   int bus_head = -4;
//   int bus_foot = 14;

  int rows = 0;
  int busesSize = sizeof buses / sizeof buses[0];
  
  for(int i = 0; i< busesSize; i++){
    if(buses[i]){
      bus_layers[i] = bus_layer_create(GRect(0, (24 * i), 0, 0));
      bus_layer_set_bus(bus_layers[i], buses[i]);
      
      scroll_layer_add_child(s_scroll_layer, bus_layer_get_layer(bus_layers[i]));
      rows++;
    }
  }
  
  scroll_layer_set_content_size(s_scroll_layer, GSize(bounds.size.w, (24*rows)));
  layer_add_child(window_get_root_layer(s_window), scroll_layer_get_layer(s_scroll_layer));
}

static void destroy_ui(void) {
  window_destroy(s_window);

  text_layer_destroy(s_headerlayer_1);
  text_layer_destroy(s_headerlayer_2);
  scroll_layer_destroy(s_scroll_layer);
 
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_stop(const char *name) {
  initialise_ui(name);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_stop(void) {
  window_stack_remove(s_window, true);
}

void stop_add_bus(int index, const char *route, const char *destination, int dueIn){
  Bus *bus = bus_create(
    route, 
    destination,
    dueIn
  );
  buses[index] = bus;
}