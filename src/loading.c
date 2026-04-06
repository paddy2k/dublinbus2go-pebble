#include "loading.h"
#include "home.h"
#include <pebble.h>

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static BitmapLayer *s_bitmaplayer_1;
static TextLayer *s_textlayer_2;
static TextLayer *s_textlayer_1;
static AppTimer *timer;

void tick_handler(void *data){
  if (!s_window || !window_stack_contains_window(s_window)) {
    timer = NULL;
    return;
  }

  if (s_textlayer_1) {
    const char *dots = text_layer_get_text(s_textlayer_1);
    if (dots) {
      if(!strcmp(dots, "..")){
        text_layer_set_text(s_textlayer_1, "...");
      } else if(!strcmp(dots, ".")){
        text_layer_set_text(s_textlayer_1, "..");
      } else {
        text_layer_set_text(s_textlayer_1, ".");
      }
    }
  }

  timer = app_timer_register(1000, tick_handler, NULL);
}

static void initialise_ui(void) {
  GColor backgroundColour = COLOR_FALLBACK(GColorFromHEX(0x00A572), GColorBlack);
  GColor textColour = GColorWhite;
  GFont s_res_gothic_24 = fonts_get_system_font(FONT_KEY_GOTHIC_24);

  s_window = window_create();
  if (!s_window) return;
  window_set_background_color(s_window, backgroundColour);

  // s_bitmaplayer_1 - shares logo bitmap from home screen (not a separate copy)
  GRect imageSize = GRect(4, 13, 136, 40);
  #ifdef PBL_SDK_3
  imageSize = GRect(4, 22, 136, 40);
  #endif
  s_bitmaplayer_1 = bitmap_layer_create(imageSize);
  if (s_bitmaplayer_1) {
    GBitmap *logo = get_app_logo();
    if (logo) {
      bitmap_layer_set_bitmap(s_bitmaplayer_1, logo);
    }
    layer_add_child(window_get_root_layer(s_window), (Layer *)s_bitmaplayer_1);
  }

  // s_textlayer_2
  s_textlayer_2 = text_layer_create(GRect(37, 85, 100, 32));
  if (s_textlayer_2) {
    text_layer_set_background_color(s_textlayer_2, GColorClear);
    text_layer_set_text_color(s_textlayer_2, textColour);
    text_layer_set_text(s_textlayer_2, "Loading");
    text_layer_set_font(s_textlayer_2, s_res_gothic_24);
    layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_2);
  }

  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(90, 85, 14, 24));
  if (s_textlayer_1) {
    text_layer_set_background_color(s_textlayer_1, GColorClear);
    text_layer_set_text_color(s_textlayer_1, textColour);
    text_layer_set_text(s_textlayer_1, "...");
    text_layer_set_font(s_textlayer_1, s_res_gothic_24);
    layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_1);
  }

  timer = app_timer_register(1000, tick_handler, NULL);
}

static void destroy_ui(void) {
  if (timer) {
    app_timer_cancel(timer);
    timer = NULL;
  }

  // Destroy the bitmap layer but NOT the bitmap itself - it belongs to home.c
  if (s_bitmaplayer_1) bitmap_layer_destroy(s_bitmaplayer_1);
  s_bitmaplayer_1 = NULL;
  if (s_textlayer_2) text_layer_destroy(s_textlayer_2);
  s_textlayer_2 = NULL;
  if (s_textlayer_1) text_layer_destroy(s_textlayer_1);
  s_textlayer_1 = NULL;

  if (s_window) {
    window_destroy(s_window);
    s_window = NULL;
  }
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void remove_loading_window(void){
  if (timer) {
    app_timer_cancel(timer);
    timer = NULL;
  }
  if (s_window) {
    window_stack_remove(s_window, false);
  }
}

void show_loading(void) {
  initialise_ui();
  if (!s_window) return;
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, false);
}

void hide_loading(void) {
  if (timer) {
    app_timer_cancel(timer);
    timer = NULL;
  }
  if (s_window) {
    window_stack_remove(s_window, true);
  }
}
