typedef struct Stop {
    char *name;
    char *id;
    char *distance;
    char *bearing;
} Stop;

Stop *stop_create(const char *name, int id, const char *distance, const char *bearing);
void stop_destroy(Stop *stop);