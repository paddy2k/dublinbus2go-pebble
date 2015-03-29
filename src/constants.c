#include <pebble.h>
#include "constants.h"

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