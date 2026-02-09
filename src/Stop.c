#include <pebble.h>
#include "Stop.h"

Stop *stop_create(const char *name, const char *id, const char *distance, const char *bearing)
{
    if (!name || !id || !distance || !bearing) return NULL;

    int name_len = strlen(name);
    int id_len = strlen(id);
    int dist_len = strlen(distance);
    int bear_len = strlen(bearing);

    // Single allocation for the struct and all its strings
    int total_size = sizeof(Stop) + name_len + id_len + dist_len + bear_len + 4;
    Stop *stop = malloc(total_size);
    if (!stop) return NULL;

    char *ptr = (char *)(stop + 1);

    stop->name = ptr;
    strcpy(stop->name, name);
    ptr += name_len + 1;

    stop->id = ptr;
    strcpy(stop->id, id);
    ptr += id_len + 1;

    stop->distance = ptr;
    strcpy(stop->distance, distance);
    ptr += dist_len + 1;

    stop->bearing = ptr;
    strcpy(stop->bearing, bearing);

    return stop;
}

void stop_destroy(Stop *stop)
{
    if (stop) {
        free(stop);
    }
}
