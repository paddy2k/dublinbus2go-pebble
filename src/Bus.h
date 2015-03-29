typedef struct Bus {
    char *route;
    char *destination;
    char *dueIn;
} Bus;

Bus *bus_create(const char *route, const char *destination, int dueIn);
void bus_destroy(Bus *bus);