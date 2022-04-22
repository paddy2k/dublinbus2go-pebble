#include <pebble.h>
#include "stop.h"
#include "loading.h"
#include "Bus.h"
#include "BusLayer.h"
#include "app_message.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
#define NUM_BUSES_IN_LIST 20
  
static Window *s_window;
static GFont s_res_gothic_18;
static GFont s_res_gothic_18_bold;
static TextLayer *s_headerlayer_1;
static TextLayer *s_headerlayer_2;
static ScrollLayer *s_scroll_layer;
#ifdef PBL_SDK_3
static StatusBarLayer *s_status_bar;
#endif 
#ifdef PBL_BW
static InverterLayer *s_inverterlayer_1;
#endif

static Bus *buses[NUM_BUSES_IN_LIST] = {};
static BusLayer *bus_layers[NUM_BUSES_IN_LIST] = {};

char stop_id[6];
char stop_name[20];
int stoplist_type = 0;

static void destroy_ui(void) {
  window_destroy(s_window);
  s_window = NULL;
  free(s_window);

  text_layer_destroy(s_headerlayer_1);
  s_headerlayer_1 = NULL;
  free(s_headerlayer_1);
  text_layer_destroy(s_headerlayer_2);
  s_headerlayer_2 = NULL;
  free(s_headerlayer_2);
  scroll_layer_destroy(s_scroll_layer);
  s_scroll_layer = NULL;
  free(s_scroll_layer);
  s_res_gothic_18 = NULL;
  free(s_res_gothic_18);
  s_res_gothic_18_bold = NULL;
  free(s_res_gothic_18_bold);
  
  #ifdef PBL_SDK_3
  status_bar_layer_destroy(s_status_bar);
  #endif
  
  #ifdef PBL_BW
  inverter_layer_destroy(s_inverterlayer_1);
  #endif
    
    
  int busesSize = sizeof buses / sizeof buses[0];
  for(int i = 0; i<busesSize; i++){
    if(buses[i]){
      bus_destroy(buses[i]);
      buses[i] = NULL;
      free(buses[i]);
    }
  }
//   free(buses);
  
  int busLayersSize = sizeof bus_layers / sizeof bus_layers[0];
  for(int i = 0; i<busLayersSize; i++){
    if(bus_layers[i]){
      bus_layer_destroy(bus_layers[i]);
      bus_layers[i] = NULL;
      free(bus_layers[i]);
    }
  }
//   free(bus_layers);
  
   accel_tap_service_unsubscribe();
}

void tap_handler(AccelAxisType axis, int32_t direction)
{
  show_loading();
  hide_stop();
  getStop(stop_id);
}

static void initialise_ui() {
  s_window = window_create();
  GColor backgroundColour = COLOR_FALLBACK(GColorYellow, GColorWhite);
  window_set_background_color(s_window, backgroundColour);
  
  GRect bounds = GRect(0, 24, 144, 130);
  #ifdef PBL_SDK_3
  bounds = GRect(0, 40, 144, 130);
  #endif

  s_scroll_layer = scroll_layer_create(bounds);
  scroll_layer_set_click_config_onto_window(s_scroll_layer, s_window);
  
  s_res_gothic_18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  s_res_gothic_18_bold = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  
  
  // s_headerlayer_1
  GRect headerSize = GRect(0, 0, 144, 24);
  #ifdef PBL_SDK_3
  headerSize = GRect(0, STATUS_BAR_LAYER_HEIGHT,  144, 24);
  #endif
  s_headerlayer_1 = text_layer_create(headerSize);
  text_layer_set_text(s_headerlayer_1, "                                       Due");
  text_layer_set_background_color(s_headerlayer_1, GColorBlack);
  text_layer_set_font(s_headerlayer_1, s_res_gothic_18);
  text_layer_set_text_color(s_headerlayer_1, GColorWhite);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_headerlayer_1);
  
  // s_headerlayer_2
  GRect headerSizeTwo = GRect(2, 0, 100, 24);
  #ifdef PBL_SDK_3
  headerSizeTwo = GRect(2, STATUS_BAR_LAYER_HEIGHT,  100, 24);
  #endif
  s_headerlayer_2 = text_layer_create(headerSizeTwo);
  text_layer_set_background_color(s_headerlayer_2, GColorClear);
  text_layer_set_text_color(s_headerlayer_2, GColorWhite);
  text_layer_set_text(s_headerlayer_2, stop_name);
  text_layer_set_font(s_headerlayer_2, s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_headerlayer_2);
  
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
  
  #ifdef PBL_SDK_3
  // Set up the status bar last to ensure it is on top of other Layers
  s_status_bar = status_bar_layer_create();  
  status_bar_layer_set_colors(s_status_bar, GColorBlack, GColorWhite);
  layer_add_child(window_get_root_layer(s_window), status_bar_layer_get_layer(s_status_bar));
  #endif
  
  #ifdef PBL_PLATFORM_APLITE
  GRect window_bounds = layer_get_bounds(window_get_root_layer(s_window));
  GRect inverterSize = GRect(0, 0, window_bounds.size.w, window_bounds.size.h);
  // s_inverterlayer_2
  s_inverterlayer_1 = inverter_layer_create(inverterSize);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_inverterlayer_1);
  #endif 
    
  accel_tap_service_subscribe(tap_handler);
}

// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
   destroy_ui();
}

void show_stop(int id) {
  snprintf(stop_id, sizeof stop_id, "%d", id);
    
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_stop(void) {
  window_stack_remove(s_window, true);
}

void stop_add_bus(int index, const char *route, const char *destination, int dueIn){
  buses[index] = bus_create(
    route, 
    destination,
    dueIn
  );
}

void set_stop_name(const char *name) {
  snprintf(stop_name, sizeof stop_name, "%s", name);
}