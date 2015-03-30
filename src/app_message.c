#include <pebble.h>
#include "Stop.h"
#include "app_message.h"
#include "stoplist.h"
#include "loading.h"
#include "stop.h"
#include "saved.h"
#include "removed.h"
#include "no_buses.h"
#include "no_stops.h"

  
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
	STATUS_END = 2,
  STATUS_ERROR = 3
};

enum {
  ACTION_GET_SAVED_STOPS = 0, 
  ACTION_GET_NEAREST_STOPS = 1,
  ACTION_GETSTOP = 2,
  ACTION_GETSTOPS = 3,
  ACTION_SAVE_STOP = 4,
  ACTION_REMOVE_STOP = 5,
};

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *action;
  Tuple *status;
  
	action = dict_find(received, ACTION_KEY);
  status = dict_find(received, STATUS_KEY);
  if(action && status) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "ACTION: %d", (int)action->value->uint32);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "STATUS: %d", (int)status->value->uint32);
    
    switch((int) action->value->uint32){
      case ACTION_REMOVE_STOP:
        if(STATUS_END == (int) status->value->uint32){
          hide_removed();
          APP_LOG(APP_LOG_LEVEL_DEBUG, "STOP SAVED"); 
        }
        break;
      case ACTION_SAVE_STOP:
        if(STATUS_END == (int) status->value->uint32){
          hide_saved();
          APP_LOG(APP_LOG_LEVEL_DEBUG, "STOP SAVED"); 
        }
        break;
      case ACTION_GETSTOPS:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "ACTION: GET STOPS"); 
        switch((int) status->value->uint32){
          case STATUS_ERROR:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "STOPS ERROR");
            show_no_stops();
            hide_loading();
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

                 stop_name = NULL;
                 free(stop_name);
                 stop_id = NULL;
                 free(stop_id);
                 stop_distance = NULL;
                 free(stop_distance);
                 stop_bearing = NULL;
                 free(stop_bearing);
              }  
            }
            show_stoplist();
            remove_loading_window();
          
            status = NULL;
            free(status);
            action = NULL;
            free(action);
            break;
        }
        break;
      
      case ACTION_GETSTOP:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "ACTION: GET STOP"); 
        switch((int) status->value->uint32){
          case STATUS_ERROR:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "STOP ERROR");
            show_no_buses();
            hide_loading();
            break;
          case STATUS_END:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "STATUS: END"); 
            Tuple *stop_id = dict_find(received, ID_KEY);
          
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
                
                 bus_route = NULL;
                 free(bus_route);
                 bus_destination = NULL;
                 free(bus_destination);
                 bus_duein = NULL;
                 free(bus_duein);
              }  
            }
          
            show_stop(stop_id->value->cstring);
            hide_loading();
          
            stop_id = NULL;
            free(stop_id);
            status = NULL;
            free(status);
            action = NULL;
            free(action);
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


static void out_sent_handler(DictionaryIterator *failed, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "MESSAGE SENT"); 
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
  
  status = NULL;
  free(status);
  action = NULL;
  free(action);
}
  
void message_init(void) {
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	
  app_message_register_outbox_sent(out_sent_handler);
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

void getStop(char *id){
  // Request from API
  DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);
  dict_write_int16(iter, ACTION_KEY, ACTION_GETSTOP);
  dict_write_cstring(iter, ID_KEY, id);
//   dict_write_cstring(iter, NAME_KEY, name);
  dict_write_end(iter);
  app_message_outbox_send();
}

void saveStop(char *id){
  // Request from API
  DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);
  dict_write_int16(iter, ACTION_KEY, ACTION_SAVE_STOP);
  dict_write_cstring(iter, ID_KEY, id);
  dict_write_end(iter);
  app_message_outbox_send();
}

void removeStop(char *id){
  // Request from API
  DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);
  dict_write_int16(iter, ACTION_KEY, ACTION_REMOVE_STOP);
  dict_write_cstring(iter, ID_KEY, id);
  dict_write_end(iter);
  app_message_outbox_send();
}