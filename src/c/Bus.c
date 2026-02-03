#include <pebble.h>
#include "Bus.h"

Bus *bus_create(const char *route, const char *destination, int dueIn)
{
    Bus *bus = malloc(sizeof(Bus));
    
    int route_size = strlen(route) + 1;    
    bus->route = malloc(sizeof(char) * route_size);
    strncpy(bus->route, route, route_size);
    
    int destination_size = strlen(destination) + 1;
    bus->destination = malloc(sizeof(char) * destination_size);
    strncpy(bus->destination, destination, destination_size);

    int dueIn_size = 4;
    bus->dueIn = malloc(sizeof(char) * dueIn_size);
    snprintf(bus->dueIn, dueIn_size, "%d", dueIn);

    return bus;
}

void bus_destroy(Bus *bus)
{
    if(bus == NULL){
      return ;
    }
    
    free(bus->route);
    bus->route = NULL;
    free(bus->dueIn);
    bus->dueIn = NULL;
    free(bus->destination);
    bus->destination = NULL;
    free(bus);
    bus = NULL;
}