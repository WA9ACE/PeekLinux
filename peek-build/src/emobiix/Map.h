#ifndef _MAP_H_
#define _MAP_H_

#include "List.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Map_t;
typedef struct Map_t Map;

/*struct MapIterator_t;
typedef struct MapIterator_t MapIterator;*/
#define MapIterator ListIterator

Map *map_string(void);
Map *map_int(void);
void map_append(Map *ht, const void *key, void *data);
void *map_find(Map *ht, const void *key);
void *map_find_number(const Map *const ht, const int key);
void map_remove(Map *ht, const void *key);
void map_delete(Map *ht);

void map_begin(Map *ht, MapIterator *iter);
int mapIterator_finished(MapIterator *iter);
void *mapIterator_item(MapIterator *iter, void **key);
void mapIterator_remove(MapIterator *iter);
void mapIterator_removeInt(MapIterator *iter);
void mapIterator_next(MapIterator *iter);
/*void mapIterator_delete(MapIterator *iter);*/

#ifdef __cplusplus
}
#endif

#endif /* _HASHTABLE_H_ */
