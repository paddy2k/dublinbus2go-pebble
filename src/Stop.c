#include <pebble.h>
#include "Stop.h"

Stop *stop_create(const char *name, int id, const char *distance, const char *bearing)
{
    Stop *stop = malloc(sizeof(Stop));
    
    int name_size = strlen(name) + 1;    
    stop->name = malloc(sizeof(char) * name_size);
    strncpy(stop->name, name, name_size);
    
    int id_size = 6;
    stop->id = malloc(sizeof(char) * id_size);
    snprintf(stop->id, id_size, "%d", id);

    int distance_size = strlen(distance) + 1;
    stop->distance = malloc(sizeof(char) * distance_size);
    strncpy(stop->distance, distance, distance_size);

    int bearing_size = strlen(bearing) + 1;    
    stop->bearing = malloc(sizeof(char) * bearing_size);
    strncpy(stop->bearing, bearing, bearing_size);

    return stop;
}

void stop_destroy(Stop *stop)
{
    if (!stop) {
        return;
    }
    free(stop->name);
    free(stop->id);
    free(stop->distance);
    free(stop->bearing);
    free(stop);
}