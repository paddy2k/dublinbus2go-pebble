// forward declarations
typedef struct Layer Layer;
typedef struct TextLayer TextLayer;
typedef struct GRect GRect;
typedef struct Bus Bus;


typedef struct BusLayer {
    Layer *root_layer;
    TextLayer *route_text_layer;
    TextLayer *destination_text_layer;
    TextLayer *due_text_layer;
    TextLayer *footer_text_layer;
} BusLayer;

// Bus layers are a set size (124 x 24). The size information in the frame argument is ignored.
BusLayer *bus_layer_create(GRect frame);

void bus_layer_destroy(BusLayer *bus_layer);

void bus_layer_set_bus(BusLayer *bus_layer, Bus *bus);

Layer *bus_layer_get_layer(BusLayer *bus_layer);