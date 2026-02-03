#include "pebble.h"
#include "home.h"
#include "app_message.h"

int main(void) {
  message_init();
  show_home();
  app_event_loop();
  hide_home();
  message_deinit();
}
