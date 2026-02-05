#include <pebble.h>
#include "Stop.h"

Stop *stop_create(const char *name, const char *id, const char *distance, const char *bearing)
{
    Stop *stop = malloc(sizeof(Stop));
    if (!stop) return NULL;
    
    int name_size = strlen(name) + 1;    
    stop->name = malloc(sizeof(char) * name_size);
    if (!stop->name) { free(stop); return NULL; }
    strncpy(stop->name, name, name_size);
    
    int id_size = strlen(id) + 1;
    stop->id = malloc(sizeof(char) * id_size);
    if (!stop->id) { free(stop->name); free(stop); return NULL; }
    strncpy(stop->id, id, id_size);

    int distance_size = strlen(distance) + 1;
    stop->distance = malloc(sizeof(char) * distance_size);
    if (!stop->distance) { free(stop->id); free(stop->name); free(stop); return NULL; }
    strncpy(stop->distance, distance, distance_size);

    int bearing_size = strlen(bearing) + 1;    
    stop->bearing = malloc(sizeof(char) * bearing_size);
    if (!stop->bearing) { free(stop->distance); free(stop->id); free(stop->name); free(stop); return NULL; }
    strncpy(stop->bearing, bearing, bearing_size);

    return stop;
}

void stop_destroy(Stop *stop)
{
    if (!stop) {
        return;
    }
    if (stop->name) free(stop->name);
    if (stop->id) free(stop->id);
    if (stop->distance) free(stop->distance);
    if (stop->bearing) free(stop->bearing);
    free(stop);
}