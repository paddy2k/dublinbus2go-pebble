#include "stop.h"
#include <pebble.h>

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static TextLayer *s_textlayer_1;
static TextLayer *s_textlayer_2;
int stop_number;

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


static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_fullscreen(s_window, false);

  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(0, 20, 144, 20));
  text_layer_set_text(s_textlayer_1, "Stop Number:");
  text_layer_set_text_alignment(s_textlayer_1, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_1);
  
  static char stopNumber[5] = "1234";
  snprintf(stopNumber, sizeof(stopNumber), "%d", stop_number);

  
  // s_textlayer_2
  s_textlayer_2 = text_layer_create(GRect(0, 40, 144, 20));
  text_layer_set_text(s_textlayer_2, stopNumber);
  text_layer_set_text_alignment(s_textlayer_2, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_2);
  
  // Request from API
  DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);
  dict_write_int16(iter, ACTION_KEY, GET_STOP_ACTION);
  dict_write_int16(iter, DATA_KEY, stop_number);
  dict_write_end(iter);
  app_message_outbox_send();
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_textlayer_1);
  text_layer_destroy(s_textlayer_2);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_stop(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_stop(void) {
  window_stack_remove(s_window, true);
}
