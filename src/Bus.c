#include <pebble.h>
#include "Bus.h"
#include "stoplist.h"

Bus *bus_create(const char *route, const char *destination, int dueIn)
{
    if (!route || !destination) return NULL;

    if (stop_list_type == 2 || stop_list_type == 3) {
        route = "";
    }

    char dueIn_str[16];
    snprintf(dueIn_str, sizeof(dueIn_str), "%d", dueIn);

    int route_len = strlen(route);
    int dest_len = strlen(destination);
    int due_len = strlen(dueIn_str);

    // Single allocation for the struct and all its strings
    int total_size = sizeof(Bus) + route_len + dest_len + due_len + 3;
    Bus *bus = malloc(total_size);
    if (!bus) return NULL;

    char *ptr = (char *)(bus + 1);

    bus->route = ptr;
    strcpy(bus->route, route);
    ptr += route_len + 1;

    bus->destination = ptr;
    strcpy(bus->destination, destination);
    ptr += dest_len + 1;

    bus->dueIn = ptr;
    strcpy(bus->dueIn, dueIn_str);

    return bus;
}

void bus_destroy(Bus *bus)
{
    if (bus) {
        free(bus);
    }
}
