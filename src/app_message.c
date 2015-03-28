#include <pebble.h>
#include "Stop.h"
#include "app_message.h"
#include "teststoplist.h"
#include "loading.h"
#include "test.h"

// Key values for AppMessage Dictionary
enum {
  ACTION_KEY = 0,	
  STATUS_KEY = 1,	
  DATA_KEY = 2,
	NAME_KEY = 3,
  ID_KEY = 4,
  DISTANCE_KEY = 5,
  BEARING_KEY = 6,
  DUE_KEY = 7,
  INDEX_KEY = 8
};

enum {
  STATUS_START = 0,	
  STATUS_INPROGRESS = 1,	
	STATUS_END = 2
};

enum {
  ACTION_GET_SAVED_STOPS = 0, 
  ACTION_GET_NEAREST_STOPS = 1,
  ACTION_GETSTOP = 2,
  ACTION_GETSTOPS = 3
};

// Write message to buffer & send
void send_message(void){
// 	DictionaryIterator *iter;
//  	app_message_outbox_begin(&iter);
//   Tuplet cfg_val = TupletCString(STATUS_KEY, "sms");
//   dict_write_tuplet(iter, &cfg_val);

// // 	dict_write_uint8(iter, STATUS_KEY, 0x1);
	
// 	dict_write_end(iter);
//   	app_message_outbox_send();
}

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *action;
  Tuple *status;
  Tuple *name;
  Tuple *index;
  Tuple *id;
  Tuple *distance;
  Tuple *bearing;
//   Tuple *data;
  
	action = dict_find(received, ACTION_KEY);
  status = dict_find(received, STATUS_KEY);
  if(action && status) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "ACTION: %d", (int)action->value->uint32);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "STATUS: %d", (int)status->value->uint32);
    
    switch((int) action->value->uint32){
      case ACTION_GETSTOPS:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "ACTION: GET STOPS"); 
        switch((int) status->value->uint32){
          case STATUS_START:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "STATUS: START"); 
            break;
          
          case STATUS_INPROGRESS:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "STATUS: IN-PROGRESS"); 
          
            index = dict_find(received, INDEX_KEY); 
            name = dict_find(received, NAME_KEY);
            distance = dict_find(received, DISTANCE_KEY);
            id = dict_find(received, ID_KEY);
            bearing = dict_find(received, BEARING_KEY);
            
//             stoplist_add_stop(
//               (int)index->value->uint32, 
//               name->value->cstring, 
//               (int)id->value->uint32,
//               distance->value->cstring,
//               bearing->value->cstring
//             );

            break;
          case STATUS_END:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "STATUS: END"); 
            
            for(int i = 0; i<10; i++){
              int stopNameId = 200+i;
              int stopIdId = 210+i;
              int stopDistanceId = 220+i;
              int stopBearingId = 230+i;
              
              Tuple *stop_id = dict_find(received, stopIdId);
              if(stop_id){
                Tuple *stop_name = dict_find(received, stopNameId);
                Tuple *stop_distance = dict_find(received, stopDistanceId);
                Tuple *stop_bearing = dict_find(received, stopBearingId);
                
                 stoplist_add_stop(
                   i,
                   stop_name->value->cstring,
                   (int)stop_id->value->uint32,
                   stop_distance->value->cstring, 
                   stop_bearing->value->cstring
                 );
              }  
            
          
            show_stoplist();
            remove_loading_window();
            break;
        }
        break;
      
      case ACTION_GETSTOP:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "ACTION: GET STOP"); 
        switch((int) status->value->uint32){
          case STATUS_START:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "STATUS: START"); 
            break;
          case STATUS_INPROGRESS:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "STATUS: IN-PROGRESS"); 
            break;
          case STATUS_END:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "STATUS: END"); 
            Tuple *stop_name = dict_find(received, NAME_KEY);
            for(int i = 0; i<10; i++){
              int routeId = 100+i;
              int destinationId = 110+i;
              int dueinId = 120+i;
              
              Tuple *bus_route = dict_find(received, routeId);
              if(bus_route){
                Tuple *bus_destination = dict_find(received, destinationId);
                Tuple *bus_duein = dict_find(received, dueinId);
                
                 stop_add_bus(
                   i, 
                   bus_route->value->cstring, 
                   bus_destination->value->cstring,
                   (int)bus_duein->value->int16
                 );
              }  
            }
          
            show_test(stop_name->value->cstring);
            hide_loading();
            break;
        }
        break;
    }
  }
}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {	
}

char *translate_error(AppMessageResult result) {
  switch (result) {
    case APP_MSG_OK: return "APP_MSG_OK";
    case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
    case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
    case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
    case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
    case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
    case APP_MSG_BUSY: return "APP_MSG_BUSY";
    case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
    case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
    case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
    case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
    case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
    case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
    case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
    default: return "UNKNOWN ERROR";
  }
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "MESSAGE FAILED"); 
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "MESSAGE REASON: %s", translate_error(reason)); 
  
  Tuple *action;
	action = dict_find(failed, ACTION_KEY);
  if(action){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "FAILED ACTION: %d", (int)action->value->uint32);
  }
  
  Tuple *status;
	status = dict_find(failed, STATUS_KEY);
  if(status){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "FAILED STATUS: %d", (int)status->value->uint32);
  }
}
  
void message_init(void) {
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);
		
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void message_deinit(void) {
	app_message_deregister_callbacks();
}

void getSavedStops(void){
  // Request from API
  DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);
  dict_write_int16(iter, ACTION_KEY, ACTION_GET_SAVED_STOPS);
  dict_write_end(iter);
  app_message_outbox_send();
}

void getNearestStops(void){
  // Request from API
  DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);
  dict_write_int16(iter, ACTION_KEY, ACTION_GET_NEAREST_STOPS);
  dict_write_end(iter);
  app_message_outbox_send();
}

void getStop(char *id, char *name){
  // Request from API
  DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);
  dict_write_int16(iter, ACTION_KEY, ACTION_GETSTOP);
  dict_write_cstring(iter, ID_KEY, id);
  dict_write_cstring(iter, NAME_KEY, name);
  dict_write_end(iter);
  app_message_outbox_send();
}