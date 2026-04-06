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

static Bus *buses[NUM_BUSES_IN_LIST] = {};
static BusLayer *bus_layers[NUM_BUSES_IN_LIST] = {};

char stop_id[16];
char stop_name[64];
int stoplist_type = 0;

static void destroy_ui(void) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "stop destroy_ui: start, heap_free=%zu", heap_bytes_free());
  accel_tap_service_unsubscribe();

  int busLayersSize = sizeof bus_layers / sizeof bus_layers[0];
  for(int i = 0; i<busLayersSize; i++){
    if(bus_layers[i]){
      bus_layer_destroy(bus_layers[i]);
      bus_layers[i] = NULL;
    }
  }

  #ifdef PBL_SDK_3
  if (s_status_bar) {
    status_bar_layer_destroy(s_status_bar);
    s_status_bar = NULL;
  }
  #endif

  if (s_scroll_layer) {
    scroll_layer_destroy(s_scroll_layer);
    s_scroll_layer = NULL;
  }
  if (s_headerlayer_2) {
    text_layer_destroy(s_headerlayer_2);
    s_headerlayer_2 = NULL;
  }
  if (s_headerlayer_1) {
    text_layer_destroy(s_headerlayer_1);
    s_headerlayer_1 = NULL;
  }
  s_res_gothic_18 = NULL;
  s_res_gothic_18_bold = NULL;

  if (s_window) {
    window_destroy(s_window);
    s_window = NULL;
  }
}

void tap_handler(AccelAxisType axis, int32_t direction)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "tap_handler: fired");
  show_loading();
  hide_stop();
  getStop(stop_id);
}

static void initialise_ui() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "stop init_ui: start, heap_free=%zu", heap_bytes_free());
  s_window = window_create();
  if (!s_window) return;

  GColor backgroundColour = COLOR_FALLBACK(GColorFromHEX(0x00A572), GColorBlack);
  window_set_background_color(s_window, backgroundColour);

  GRect bounds = GRect(0, 24, 144, 130);
  #ifdef PBL_SDK_3
  bounds = GRect(0, 40, 144, 130);
  #endif

  s_scroll_layer = scroll_layer_create(bounds);
  if (!s_scroll_layer) return;
  scroll_layer_set_click_config_onto_window(s_scroll_layer, s_window);

  s_res_gothic_18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  s_res_gothic_18_bold = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);


  // s_headerlayer_1
  GRect headerSize = GRect(0, 0, 144, 24);
  #ifdef PBL_SDK_3
  headerSize = GRect(0, STATUS_BAR_LAYER_HEIGHT,  144, 24);
  #endif
  s_headerlayer_1 = text_layer_create(headerSize);
  if (s_headerlayer_1) {
    text_layer_set_text(s_headerlayer_1, "                                       Due");
    text_layer_set_background_color(s_headerlayer_1, GColorFromHEX(0x00A572));
    text_layer_set_font(s_headerlayer_1, s_res_gothic_18);
    text_layer_set_text_color(s_headerlayer_1, GColorWhite);
    layer_add_child(window_get_root_layer(s_window), (Layer *)s_headerlayer_1);
  }

  // s_headerlayer_2
  GRect headerSizeTwo = GRect(2, 0, 100, 24);
  #ifdef PBL_SDK_3
  headerSizeTwo = GRect(2, STATUS_BAR_LAYER_HEIGHT,  100, 24);
  #endif
  s_headerlayer_2 = text_layer_create(headerSizeTwo);
  if (s_headerlayer_2) {
    text_layer_set_background_color(s_headerlayer_2, GColorClear);
    text_layer_set_text_color(s_headerlayer_2, GColorWhite);
    text_layer_set_text(s_headerlayer_2, stop_name);
    text_layer_set_font(s_headerlayer_2, s_res_gothic_18_bold);
    layer_add_child(window_get_root_layer(s_window), (Layer *)s_headerlayer_2);
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "stop init_ui: headers done");

  int rows = 0;
  int maxRows = 5;
  int busesSize = sizeof buses / sizeof buses[0];
  for(int i = 0; i< busesSize; i++){
    if (bus_layers[i]) {
      bus_layer_destroy(bus_layers[i]);
      bus_layers[i] = NULL;
    }
    if(buses[i] && rows < maxRows){
      bus_layers[i] = bus_layer_create(GRect(0, (24 * rows), 0, 0));
      if (bus_layers[i]) {
        bus_layer_set_bus(bus_layers[i], buses[i]);
        scroll_layer_add_child(s_scroll_layer, bus_layer_get_layer(bus_layers[i]));
        rows++;
      }
    }
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "stop init_ui: %d bus rows", rows);

  scroll_layer_set_content_size(s_scroll_layer, GSize(bounds.size.w, (24*rows)));
  layer_add_child(window_get_root_layer(s_window), scroll_layer_get_layer(s_scroll_layer));

  #ifdef PBL_SDK_3
  // Set up the status bar last to ensure it is on top of other Layers
  s_status_bar = status_bar_layer_create();
  if (s_status_bar) {
    status_bar_layer_set_colors(s_status_bar, GColorFromHEX(0x00A572), GColorWhite);
    layer_add_child(window_get_root_layer(s_window), status_bar_layer_get_layer(s_status_bar));
  }
  #endif

  accel_tap_service_subscribe(tap_handler);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "stop init_ui: done, heap_free=%zu", heap_bytes_free());
}

// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
   destroy_ui();
}

void stop_clear_all(void) {
  for(int i = 0; i < NUM_BUSES_IN_LIST; i++){
    if(buses[i]){
      bus_destroy(buses[i]);
      buses[i] = NULL;
    }
  }
}

void show_stop(const char *id) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "show_stop: start id=%s", id);
  strncpy(stop_id, id, sizeof(stop_id) - 1);
  stop_id[sizeof(stop_id) - 1] = '\0';

  if (s_window && window_stack_contains_window(s_window)) {
    int busLayersSize = sizeof bus_layers / sizeof bus_layers[0];
    for(int i = 0; i<busLayersSize; i++){
      if(bus_layers[i]){
        bus_layer_destroy(bus_layers[i]);
        bus_layers[i] = NULL;
      }
    }
    window_stack_remove(s_window, false);
  }

  if (s_window) {
     window_destroy(s_window);
     s_window = NULL;
  }

  initialise_ui();
  if (s_window) {
    window_set_window_handlers(s_window, (WindowHandlers) {
      .unload = handle_window_unload,
    });
    APP_LOG(APP_LOG_LEVEL_DEBUG, "show_stop: about to push window, heap_free=%zu", heap_bytes_free());
    window_stack_push(s_window, true);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "show_stop: window pushed");
  }
}

void hide_stop(void) {
  if (s_window) {
    window_stack_remove(s_window, true);
  }
}

void stop_add_bus(int index, const char *route, const char *destination, int dueIn){
  if (index < 0 || index >= NUM_BUSES_IN_LIST) return;
  if (buses[index]) {
    bus_destroy(buses[index]);
    buses[index] = NULL;
  }
  buses[index] = bus_create(
    route, 
    destination,
    dueIn
  );
}

void set_stop_name(const char *name) {
  snprintf(stop_name, sizeof stop_name, "%s", name);
}