#include "no_stops.h"
#include <pebble.h>

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_roboto_condensed_21;
static GFont s_res_gothic_14;
static GBitmap *s_res_info_large;
static TextLayer *s_textlayer_1;
static TextLayer *s_textlayer_2;
static BitmapLayer *s_bitmaplayer_1;

static void initialise_ui(void) {
  GColor backgroundColour = COLOR_FALLBACK(GColorYellow, GColorBlack);
  GColor textColour = COLOR_FALLBACK(GColorBlack, GColorWhite);
  
  s_window = window_create();
  window_set_background_color(s_window, backgroundColour);
  window_set_fullscreen(s_window, false);
  
  s_res_roboto_condensed_21 = fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21);
  s_res_info_large = gbitmap_create_with_resource(RESOURCE_ID_INFO_LARGE);
  s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  
  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(20, 95, 104, 95));
  text_layer_set_background_color(s_textlayer_1, GColorClear);
  text_layer_set_text_color(s_textlayer_1, textColour);
  text_layer_set_text(s_textlayer_1, "No Stops");
  text_layer_set_text_alignment(s_textlayer_1, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_1, s_res_roboto_condensed_21);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_1);
  
  // s_bitmaplayer_1
  s_bitmaplayer_1 = bitmap_layer_create(GRect(39, 20, 66, 66));
  bitmap_layer_set_bitmap(s_bitmaplayer_1, s_res_info_large);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_bitmaplayer_1);
  
  // s_textlayer_2
  s_textlayer_2 = text_layer_create(GRect(0, 130, 144, 20));
  text_layer_set_background_color(s_textlayer_2, GColorClear);
  text_layer_set_text_color(s_textlayer_2, textColour);
  text_layer_set_text(s_textlayer_2, "Long press stop to save");
  text_layer_set_text_alignment(s_textlayer_2, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_2, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_2);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_textlayer_1);
  text_layer_destroy(s_textlayer_2);
  bitmap_layer_destroy(s_bitmaplayer_1);
  gbitmap_destroy(s_res_info_large);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_no_stops(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_no_stops(void) {
  window_stack_remove(s_window, true);
}
