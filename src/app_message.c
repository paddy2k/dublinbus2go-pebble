#include <pebble.h>
#include "Stop.h"
#include "app_message.h"
#include "stoplist.h"
#include "loading.h"
#include "home.h"
#include "stop.h"
#include "saved.h"
#include "removed.h"
#include "no_buses.h"
#include "no_stops.h"

static bool debug = true;
  
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
  ACTION_SHOW_UI = 6,
  ACTION_GET_NEAREST_BUS = 7,
  ACTION_GET_NEAREST_TRAIN = 8,
  ACTION_GET_NEAREST_TRAM = 9
};

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

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *action;
  Tuple *status;
  
	action = dict_find(received, ACTION_KEY);
  status = dict_find(received, STATUS_KEY);
  if(action && status) {
    if(debug){APP_LOG(APP_LOG_LEVEL_DEBUG, "ACTION: %d", (int)action->value->uint32);}
    if(debug){APP_LOG(APP_LOG_LEVEL_DEBUG, "STATUS: %d", (int)status->value->uint32);}
    
    switch((int) action->value->uint32){
      case ACTION_SHOW_UI:
        if(STATUS_END == (int) status->value->uint32){
          show_ui();
          if(debug){APP_LOG(APP_LOG_LEVEL_DEBUG, "JS LOADED (UI shown)"); }
        }
        break;
      case ACTION_REMOVE_STOP:
        if(STATUS_END == (int) status->value->uint32){
          getSavedStops();
          if(debug){APP_LOG(APP_LOG_LEVEL_DEBUG, "STOP REMOVED");}
        }
        break;
      case ACTION_SAVE_STOP:
        if(STATUS_END == (int) status->value->uint32){
          hide_saved();
          if(debug){APP_LOG(APP_LOG_LEVEL_DEBUG, "STOP SAVED");}
        }
        break;
      case ACTION_GETSTOPS:
        if(debug){APP_LOG(APP_LOG_LEVEL_DEBUG, "ACTION: GET STOPS");}
        switch((int) status->value->uint32){
          case STATUS_ERROR:
            if(debug){APP_LOG(APP_LOG_LEVEL_DEBUG, "STOPS ERROR");}
            show_no_stops();
            hide_loading();
            break;
          case STATUS_END:
            if(debug){APP_LOG(APP_LOG_LEVEL_DEBUG, "STATUS: END");}
            stoplist_clear_all();
            
            for(int i = 0; i<20; i++){
              int stopNameId = 2000+i;
              int stopIdId = 2100+i;
              int stopDistanceId = 2200+i;
              int stopBearingId = 2300+i;
              
              Tuple *stop_id = dict_find(received, stopIdId);
              if(stop_id){
                Tuple *stop_name = dict_find(received, stopNameId);
                Tuple *stop_distance = dict_find(received, stopDistanceId);
                Tuple *stop_bearing = dict_find(received, stopBearingId);
                
                if (stop_name && stop_name->type == TUPLE_CSTRING && 
                    stop_distance && stop_distance->type == TUPLE_CSTRING && 
                    stop_bearing && stop_bearing->type == TUPLE_CSTRING) {
                   char stopIdStr[16];
                   if (stop_id->type == TUPLE_CSTRING) {
                     strncpy(stopIdStr, stop_id->value->cstring, sizeof(stopIdStr)-1);
                   } else {
                     snprintf(stopIdStr, sizeof(stopIdStr), "%d", (int)stop_id->value->uint32);
                   }
                   stopIdStr[sizeof(stopIdStr)-1] = '\0';

                   stoplist_add_stop(
                     i,
                     stop_name->value->cstring,
                     stopIdStr,
                     stop_distance->value->cstring, 
                     stop_bearing->value->cstring
                   );
                }
              }  
            }
            show_stoplist();
            remove_loading_window();
            hide_removed();
            break;
        }
        break;
      
      case ACTION_GETSTOP:
        if(debug){APP_LOG(APP_LOG_LEVEL_DEBUG, "ACTION: GET STOP");}
        switch((int) status->value->uint32){
          case STATUS_ERROR:
            if(debug){APP_LOG(APP_LOG_LEVEL_DEBUG, "STOP ERROR");}
            show_no_buses();
            hide_loading();
            break;
          case STATUS_END:
            if(debug){APP_LOG(APP_LOG_LEVEL_DEBUG, "STATUS: END");}
            stop_clear_all();
            Tuple *stop_id = dict_find(received, ID_KEY);
            
            if (!stop_id) {
              APP_LOG(APP_LOG_LEVEL_ERROR, "Missing ID_KEY in GETSTOP response");
              show_no_buses();
              hide_loading();
              break;
            }
          
            for(int i = 0; i<20; i++){
              int routeId = 1000+i;
              int destinationId = 1100+i;
              int dueinId = 1200+i;
              
              Tuple *bus_route = dict_find(received, routeId);
              if(bus_route && bus_route->type == TUPLE_CSTRING){
                Tuple *bus_destination = dict_find(received, destinationId);
                Tuple *bus_duein = dict_find(received, dueinId);
                
                if (bus_destination && bus_destination->type == TUPLE_CSTRING && bus_duein) {
                   int dueIn = 0;
                   if (bus_duein->type == TUPLE_CSTRING) {
                     dueIn = atoi(bus_duein->value->cstring);
                   } else {
                     dueIn = (int)bus_duein->value->int32;
                   }

                   stop_add_bus(
                     i, 
                     bus_route->value->cstring, 
                     bus_destination->value->cstring,
                     dueIn
                   );
                }
              }  
            }
          
            char stopIdStr[16];
            if (stop_id->type == TUPLE_CSTRING) {
              strncpy(stopIdStr, stop_id->value->cstring, sizeof(stopIdStr)-1);
            } else {
              snprintf(stopIdStr, sizeof(stopIdStr), "%d", (int)stop_id->value->uint32);
            }
            stopIdStr[sizeof(stopIdStr)-1] = '\0';
            
            show_stop(stopIdStr);
            hide_loading();
            break;
        }
        break;
    }
  }
}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {	
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message Inbound Dropped: %s", translate_error(reason));
}

static void out_sent_handler(DictionaryIterator *failed, void *context) {
  if(debug){APP_LOG(APP_LOG_LEVEL_DEBUG, "MESSAGE SENT");}
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "MESSAGE OUTBOUND FAILED: %s", translate_error(reason)); 
  
  if(debug){
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
}
  
void message_init(void) {
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);
		
  // 2048 is safe for aplite, but enough for 20 stops if strings are short.
  // outbox 256 is plenty for small requests.
  AppMessageResult res = app_message_open(2048, 256);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "AppMessage Open Result: %s", translate_error(res));
}

void message_deinit(void) {
	app_message_deregister_callbacks();
}

void getSavedStops(void){
  DictionaryIterator *iter;
 	AppMessageResult result = app_message_outbox_begin(&iter);
  if (result == APP_MSG_OK) {
    dict_write_int16(iter, ACTION_KEY, ACTION_GET_SAVED_STOPS);
    dict_write_end(iter);
    app_message_outbox_send();
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox begin failed (getSavedStops): %s", translate_error(result));
  }
}

void getNearestStops(void){
  DictionaryIterator *iter;
 	AppMessageResult result = app_message_outbox_begin(&iter);
  if (result == APP_MSG_OK) {
    dict_write_int16(iter, ACTION_KEY, ACTION_GET_NEAREST_STOPS);
    dict_write_end(iter);
    app_message_outbox_send();
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox begin failed (getNearestStops): %s", translate_error(result));
  }
}

void getNearestBus(void){
  DictionaryIterator *iter;
 	AppMessageResult result = app_message_outbox_begin(&iter);
  if (result == APP_MSG_OK) {
    dict_write_int16(iter, ACTION_KEY, ACTION_GET_NEAREST_BUS);
    dict_write_end(iter);
    app_message_outbox_send();
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox begin failed (getNearestBus): %s", translate_error(result));
  }
}

void getNearestTrain(void){
  DictionaryIterator *iter;
 	AppMessageResult result = app_message_outbox_begin(&iter);
  if (result == APP_MSG_OK) {
    dict_write_int16(iter, ACTION_KEY, ACTION_GET_NEAREST_TRAIN);
    dict_write_end(iter);
    app_message_outbox_send();
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox begin failed (getNearestTrain): %s", translate_error(result));
  }
}

void getNearestTram(void){
  DictionaryIterator *iter;
 	AppMessageResult result = app_message_outbox_begin(&iter);
  if (result == APP_MSG_OK) {
    dict_write_int16(iter, ACTION_KEY, ACTION_GET_NEAREST_TRAM);
    dict_write_end(iter);
    app_message_outbox_send();
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox begin failed (getNearestTram): %s", translate_error(result));
  }
}

void getStop(char *id){
  DictionaryIterator *iter;
 	AppMessageResult result = app_message_outbox_begin(&iter);
  if (result == APP_MSG_OK) {
    dict_write_int16(iter, ACTION_KEY, ACTION_GETSTOP);
    dict_write_cstring(iter, ID_KEY, id);
    dict_write_end(iter);
    app_message_outbox_send();
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox begin failed (getStop): %s", translate_error(result));
  }
}

void saveStop(char *id){
  DictionaryIterator *iter;
 	AppMessageResult result = app_message_outbox_begin(&iter);
  if (result == APP_MSG_OK) {
    dict_write_int16(iter, ACTION_KEY, ACTION_SAVE_STOP);
    dict_write_cstring(iter, ID_KEY, id);
    dict_write_end(iter);
    app_message_outbox_send();
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox begin failed (saveStop): %s", translate_error(result));
  }
}

void removeStop(char *id){
  DictionaryIterator *iter;
 	AppMessageResult result = app_message_outbox_begin(&iter);
  if (result == APP_MSG_OK) {
    dict_write_int16(iter, ACTION_KEY, ACTION_REMOVE_STOP);
    dict_write_cstring(iter, ID_KEY, id);
    dict_write_end(iter);
    app_message_outbox_send();
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox begin failed (removeStop): %s", translate_error(result));
  }
}