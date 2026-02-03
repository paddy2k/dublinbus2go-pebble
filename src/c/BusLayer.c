#include <pebble.h>
#include "BusLayer.h"
#include "Bus.h"

/*

  Bus layer layout:

  +--------------144--------------+
  |+--------89-------+ 10 +--25--+|
 26|      Destination|    |Due   ||
  |+-----------------+    +------+|
  +-------------------------------+

 */

#define BUS_LAYER_WIDTH 144
#define BUS_LAYER_HEIGHT 26
#define ROUTE_TEXT_LAYER_FRAME GRect(2, 0, 24, 22)
#define DESTINATION_TEXT_LAYER_FRAME GRect(27, 0, 95, 22)
#define DUE_TEXT_LAYER_FRAME GRect(121, 0, 18, 22)
#define FOOTER_TEXT_LAYER_FRAME GRect(0, 18, 144, 8)

BusLayer *bus_layer_create(GRect frame)
{
    BusLayer *bus_layer = malloc(sizeof(BusLayer));
    
    bus_layer->root_layer = layer_create(GRect(frame.origin.x, frame.origin.y, BUS_LAYER_WIDTH, BUS_LAYER_HEIGHT));
    
    bus_layer->route_text_layer = text_layer_create(ROUTE_TEXT_LAYER_FRAME);
    bus_layer->destination_text_layer = text_layer_create(DESTINATION_TEXT_LAYER_FRAME);
    bus_layer->due_text_layer = text_layer_create(DUE_TEXT_LAYER_FRAME);
    bus_layer->footer_text_layer = text_layer_create(FOOTER_TEXT_LAYER_FRAME);
    
    text_layer_set_font(
        bus_layer->route_text_layer, 
        fonts_get_system_font(FONT_KEY_GOTHIC_18)
    );
    
    text_layer_set_font(
        bus_layer->destination_text_layer, 
        fonts_get_system_font(FONT_KEY_GOTHIC_18)
    );
   
    text_layer_set_font(
        bus_layer->due_text_layer, 
        fonts_get_system_font(FONT_KEY_GOTHIC_18)
    );
  
    text_layer_set_font(
        bus_layer->footer_text_layer, 
        fonts_get_system_font(FONT_KEY_GOTHIC_18)
    );
    
    text_layer_set_text_alignment(
        bus_layer->due_text_layer, 
        GTextAlignmentCenter
    );
    
    text_layer_set_text_color(
        bus_layer->route_text_layer, 
        GColorBlack
    );
    
    text_layer_set_text_color(
        bus_layer->destination_text_layer, 
        GColorBlack
    );
  
    text_layer_set_text_color(
        bus_layer->due_text_layer, 
        GColorBlack
    );
    
    text_layer_set_text_color(
        bus_layer->footer_text_layer, 
        GColorBlack
    );
   
    text_layer_set_background_color(
        bus_layer->route_text_layer, 
        GColorClear
    );
    
    text_layer_set_background_color(
        bus_layer->destination_text_layer, 
        GColorClear
    );

    text_layer_set_background_color(
        bus_layer->due_text_layer, 
        GColorClear
    );
    
    text_layer_set_background_color(
        bus_layer->footer_text_layer, 
        GColorClear
    );
  
    text_layer_set_text(
        bus_layer->footer_text_layer, 
        "iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii"
    );
      
    layer_add_child(
        bus_layer->root_layer, 
        (Layer *)bus_layer->route_text_layer
    );
        
    layer_add_child(
        bus_layer->root_layer, 
        (Layer *)bus_layer->destination_text_layer
    );

    layer_add_child(
        bus_layer->root_layer, 
        (Layer *)bus_layer->due_text_layer
    );
        
    layer_add_child(
        bus_layer->root_layer, 
        (Layer *)bus_layer->footer_text_layer
    );
  
    return bus_layer;
}

void bus_layer_destroy(BusLayer *bus_layer)
{
  text_layer_destroy(bus_layer->route_text_layer);
  text_layer_destroy(bus_layer->destination_text_layer);
  text_layer_destroy(bus_layer->due_text_layer);
  text_layer_destroy(bus_layer->footer_text_layer);
  layer_destroy(bus_layer->root_layer);

  free(bus_layer);
}

void bus_layer_set_bus(BusLayer *bus_layer, Bus *bus)
{
  text_layer_set_text(
    bus_layer->route_text_layer, 
    bus->route
  );
  text_layer_set_text(
    bus_layer->destination_text_layer, 
    bus->destination
  );
  text_layer_set_text(
    bus_layer->due_text_layer, 
    bus->dueIn
  );
}

Layer *bus_layer_get_layer(BusLayer *bus_layer)
{
    return bus_layer->root_layer;
}