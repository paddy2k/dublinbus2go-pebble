#include "saved.h"
#include <pebble.h>

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_saved_window;
static GBitmap *s_res_saved_large;
static GFont s_res_roboto_condensed_21;
static BitmapLayer *s_bitmaplayer_1;
static TextLayer *s_textlayer_1;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  window_stack_pop(true);
}

static void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void initialise_ui(void) {
  GColor backgroundColour = COLOR_FALLBACK(GColorYellow, GColorBlack);
  GColor textColour = COLOR_FALLBACK(GColorBlack, GColorWhite);
  
  s_saved_window = window_create();
  window_set_background_color(s_saved_window, backgroundColour);
  
  s_res_saved_large = gbitmap_create_with_resource(RESOURCE_ID_SAVED_LARGE);
  s_res_roboto_condensed_21 = fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21);
  // s_bitmaplayer_1
  s_bitmaplayer_1 = bitmap_layer_create(GRect(39, 30, 66, 66));
  bitmap_layer_set_bitmap(s_bitmaplayer_1, s_res_saved_large);
  layer_add_child(window_get_root_layer(s_saved_window), (Layer *)s_bitmaplayer_1);
  
  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(0, 99, 144, 49));
  text_layer_set_background_color(s_textlayer_1, GColorClear);
  text_layer_set_text_color(s_textlayer_1, textColour);
  text_layer_set_text(s_textlayer_1, "Saved");
  text_layer_set_text_alignment(s_textlayer_1, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_1, s_res_roboto_condensed_21);
  layer_add_child(window_get_root_layer(s_saved_window), (Layer *)s_textlayer_1);
  
  // Register click handler
  window_set_click_config_provider(s_saved_window, click_config_provider);
}

static void destroy_ui(void) {
  window_destroy(s_saved_window);
  bitmap_layer_destroy(s_bitmaplayer_1);
  text_layer_destroy(s_textlayer_1);
  gbitmap_destroy(s_res_saved_large);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_saved(void) {
  initialise_ui();
  window_set_window_handlers(s_saved_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_saved_window, true);
}

void hide_saved(void) {
  window_stack_remove(s_saved_window, true);
}
