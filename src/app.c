#include "pebble.h"
#include "home.h"
#include "app_message.h"

int main(void) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "START - HEAP SIZE %u", heap_bytes_free());
  message_init();
  show_home();
  app_event_loop();
  hide_home();
  message_deinit();
}
