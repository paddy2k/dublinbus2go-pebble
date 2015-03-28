// #include "stoplist.h"
// #include "stop.h"
// #include "test.h"
// #include "loading.h"       
// #include <pebble.h>

// #define NUM_MENU_SECTIONS 1
// #define NUM_MENU_ITEMS 5
// #define NUM_STOPS_IN_LIST 20
  
// static Window *s_window;
// static MenuLayer *s_menulayer_1;
// int stop_list_type = 0;
// int stop_list[NUM_STOPS_IN_LIST];

// static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
//   return NUM_MENU_SECTIONS;
// }

// static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
//   return NUM_MENU_ITEMS;
// }

// static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
//   return MENU_CELL_BASIC_HEADER_HEIGHT;
// }

// static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
//   graphics_context_set_text_color(ctx, GColorWhite); // This is important.
//   switch(stop_list_type){
//     case 0: 
//       menu_cell_basic_header_draw(ctx, cell_layer, "Saved Stops");
//       break;
//     case 1: 
//       menu_cell_basic_header_draw(ctx, cell_layer, "Nearest Stops");
//       break;
//   }
// }

// static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "STOPLIST - DRAW ROW: %d", cell_index->row);
//   switch (cell_index->row) {
//     case 0:
//       // This is a basic menu item with a title and subtitle
//       stop_list[cell_index->row] = 1445;
//       // menu_cell_basic_draw(ctx, cell_layer, "Ushers Quay", "#1445 - 145,25,25a,25b,25x,26,37,39,39a,51d,66,66a,66b,66x,67,67x,69,69x,70,747,79,79a", NULL);
//       menu_cell_basic_draw(ctx, cell_layer, "Ushers Quay", "#1445                10m W", NULL);
//       break;
//     case 1:
//       // This is a basic menu item with a title and subtitle
//       stop_list[cell_index->row] = 1447;
//       // menu_cell_basic_draw(ctx, cell_layer, "Victoria Quay", "#1447 - 66x,67c,69x", NULL);
//       menu_cell_basic_draw(ctx, cell_layer, "Victoria Quay", "#1447               179m SW", NULL);
//       break;
//     case 2:
//       // This is a basic menu item with a title and subtitle
//       stop_list[cell_index->row] = 1476;
// // menu_cell_basic_draw(ctx, cell_layer, "Wolfe Tone Quay", "#1476 - 145,25,25a,25b,25x,26,51d,66,66a,66b,66x,67,67x,69,79,79a", NULL);
//       menu_cell_basic_draw(ctx, cell_layer, "Wolfe Tone Quay", "#1476               523m NE", NULL);
//       break;
//     case 3:
//       // This is a basic menu item with a title and subtitle
//       stop_list[cell_index->row] = 1477;
//       menu_cell_basic_draw(ctx, cell_layer, "Arran Quay", "#1477 - 145,25,25a,25b,25x,26,37,39,39a,51d,51x,66,66a,66b,66x,67,67x,69,70,79,79a", NULL);
//       break;
//     case 4:
//       // This is a basic menu item with a title and subtitle
//       stop_list[cell_index->row] = 1715;
//       menu_cell_basic_draw(ctx, cell_layer, "Blackhall Place", "#1715 - 37,39,39a,70", NULL);
//       break;
//   }
// }

// static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
//   stop_number = stop_list[cell_index->row];
//    show_stop();
// //  show_test();
// }

// static void initialise_ui(void) {
//   GColor backgroundColour = COLOR_FALLBACK(GColorYellow, GColorWhite);
  
//   s_window = window_create();
//   window_set_background_color(s_window, backgroundColour);
//   window_set_fullscreen(s_window, false);
  
//   // s_menulayer_1
//   s_menulayer_1 = menu_layer_create(GRect(0, 0, 144, 152));
  
//   menu_layer_set_callbacks(s_menulayer_1, NULL, (MenuLayerCallbacks){
//     .get_num_sections = menu_get_num_sections_callback,
//     .get_num_rows = menu_get_num_rows_callback,
//     .get_header_height = menu_get_header_height_callback,
//     .draw_header = menu_draw_header_callback,
//     .draw_row = menu_draw_row_callback,
//     .select_click = menu_select_callback,
//   });
  
//   menu_layer_set_click_config_onto_window(s_menulayer_1, s_window);
//   layer_add_child(window_get_root_layer(s_window), (Layer *)s_menulayer_1);
// }

// static void destroy_ui(void) {
//   window_destroy(s_window);
//   menu_layer_destroy(s_menulayer_1);
// }
// // END AUTO-GENERATED UI CODE

// static void handle_window_unload(Window* window) {
//   destroy_ui();
// }

// void show_stoplist(void) {
//   initialise_ui();
//   window_set_window_handlers(s_window, (WindowHandlers) {
//     .unload = handle_window_unload,
//   });
//   window_stack_push(s_window, true);
//   remove_loading_window();
// }

// void hide_stoplist(void) {
//   window_stack_remove(s_window, true);
// }
