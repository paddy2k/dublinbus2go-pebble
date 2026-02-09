void show_stoplist(void);
void hide_stoplist(void);
extern int stop_list_type;
void stoplist_clear_all(void);
void stoplist_add_stop(int index, const char *name, const char *id, const char *distance, const char *bearing);