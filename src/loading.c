#include "loading.h"
#include <pebble.h>

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GBitmap *s_res_dublin_bus_logo;
static GFont s_res_gothic_24;
static BitmapLayer *s_bitmaplayer_1;
static TextLayer *s_textlayer_2;
static TextLayer *s_textlayer_1;

enum {
  ACTION_KEY = 0,	
  STATUS_KEY = 1,	
	DATA_KEY = 2
};

enum {
  GET_SAVED_STOPS_ACTION = 0,	
  GET_NEAREST_STOPS_ACTION = 1,	
  GET_STOP_ACTION = 2
};


void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  switch(tick_time->tm_sec % 3){
    case 2: 
      text_layer_set_text(s_textlayer_1, "...");
      break;
    case 1: 
      text_layer_set_text(s_textlayer_1, "..");
      break;
    default: 
      text_layer_set_text(s_textlayer_1, ".");
  }
}

static void initialise_ui(void) {
  GColor backgroundColour = COLOR_FALLBACK(GColorYellow, GColorBlack);
  GColor textColour = COLOR_FALLBACK(GColorBlack, GColorWhite);
  
  s_window = window_create();
  window_set_background_color(s_window, backgroundColour);
  window_set_fullscreen(s_window, false);
  
  s_res_dublin_bus_logo = gbitmap_create_with_resource(RESOURCE_ID_DUBLIN_BUS_LOGO);
  s_res_gothic_24 = fonts_get_system_font(FONT_KEY_GOTHIC_24);
  // s_bitmaplayer_1
  s_bitmaplayer_1 = bitmap_layer_create(GRect(4, 13, 136, 40));
  bitmap_layer_set_bitmap(s_bitmaplayer_1, s_res_dublin_bus_logo);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_bitmaplayer_1);
  
  // s_textlayer_2
  s_textlayer_2 = text_layer_create(GRect(37, 85, 100, 32));
  text_layer_set_background_color(s_textlayer_2, GColorClear);
  text_layer_set_text_color(s_textlayer_2, textColour);
  text_layer_set_text(s_textlayer_2, "Loading");
  text_layer_set_font(s_textlayer_2, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_2);
  
  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(90, 85, 14, 24));
  text_layer_set_background_color(s_textlayer_1, GColorClear);
  text_layer_set_text_color(s_textlayer_1, textColour);
  text_layer_set_text(s_textlayer_1, "...");
  text_layer_set_font(s_textlayer_1, s_res_gothic_24);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_1);
  
  tick_timer_service_subscribe(SECOND_UNIT, (TickHandler) tick_handler);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  bitmap_layer_destroy(s_bitmaplayer_1);
  text_layer_destroy(s_textlayer_2);
  text_layer_destroy(s_textlayer_1);
  gbitmap_destroy(s_res_dublin_bus_logo);
  
  tick_timer_service_unsubscribe();

}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void remove_loading_window(void){
  window_stack_remove(s_window, false);
}

void show_loading(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, false);
}

void hide_loading(void) {
  window_stack_remove(s_window, true);
}