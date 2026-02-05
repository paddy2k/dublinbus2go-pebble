#include <pebble.h>
#include "Bus.h"

Bus *bus_create(const char *route, const char *destination, int dueIn)
{
    Bus *bus = malloc(sizeof(Bus));
    if (!bus) return NULL;
    
    int route_size = strlen(route) + 1;    
    bus->route = malloc(sizeof(char) * route_size);
    if (!bus->route) { free(bus); return NULL; }
    strncpy(bus->route, route, route_size);
    
    int destination_size = strlen(destination) + 1;
    bus->destination = malloc(sizeof(char) * destination_size);
    if (!bus->destination) { free(bus->route); free(bus); return NULL; }
    strncpy(bus->destination, destination, destination_size);

    int dueIn_size = 8;
    bus->dueIn = malloc(sizeof(char) * dueIn_size);
    if (!bus->dueIn) { free(bus->destination); free(bus->route); free(bus); return NULL; }
    snprintf(bus->dueIn, dueIn_size, "%d", dueIn);

    return bus;
}

void bus_destroy(Bus *bus)
{
    if(bus == NULL){
      return ;
    }
    
    if (bus->route) free(bus->route);
    bus->route = NULL;
    if (bus->dueIn) free(bus->dueIn);
    bus->dueIn = NULL;
    if (bus->destination) free(bus->destination);
    bus->destination = NULL;
    free(bus);
}