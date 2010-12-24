#include "Map.h"

#include "List.h"
#include "Debug.h"

#include "p_malloc.h"
#include <string.h>

struct MapNode_t {
    union {
        char *kstring;
        int number;
        void *pointer;
    } key;
	void *data;
};
typedef struct MapNode_t MapNode;

typedef enum {STRING, NUMBER} MapType;
struct Map_t {
    MapType type;
    List *list;
};

Map *map_new(MapType htt)
{
    Map *output;

    output = (Map *)p_malloc(sizeof(Map));

	if (!output)
		return NULL;

    output->list = list_new();
    output->type = htt;

    return output;
}

Map *map_string(void)
{
    return map_new(STRING);
}

Map *map_int(void)
{
    return map_new(NUMBER);
}

void map_append(Map *ht, const void *key, void *data)
{
    MapNode *node;
    int kstrlen;

	EMO_ASSERT(ht != NULL, "map append to NULL map")
	EMO_ASSERT(key != NULL, "map append missing key")

	map_remove(ht, key);

    node = (MapNode *)p_malloc(sizeof(MapNode));
    node->data = data;

    switch (ht->type) {
        case STRING:
            node->key.kstring = p_strdup((const char *)key);
            kstrlen = strlen(node->key.kstring);
            break;
        case NUMBER:
        default:
            node->key.number = (int)key;
            break;
    }
    list_append(ht->list, (void *)node);
}

static void* map_find_number(const Map *const ht, const int key)
{
	MapNode *output;
	ListIterator iter;

	list_begin(ht->list, &iter);
	while (!listIterator_finished(&iter)) 
	{
		output = (MapNode *)listIterator_item(&iter);
		if (output->key.number == key)
			return output->data;
		listIterator_next(&iter);
	}
	return NULL;
}

static void* map_find_string(const Map *const ht, const char *key)
{
	MapNode *output;
	ListIterator iter;

	list_begin(ht->list, &iter);
	while (!listIterator_finished(&iter)) {
		output = (MapNode *)listIterator_item(&iter);
		if (strcmp(output->key.kstring, key) == 0) 
			return output->data;
		listIterator_next(&iter);
	}
	return NULL;
}

void *map_find(Map *ht, const void *key)
{
	EMO_ASSERT_NULL(ht != NULL, "map find on NULL map");
	EMO_ASSERT_NULL(key != NULL, "map find missing key");
	
	switch (ht->type) 
	{
		case STRING:
			return map_find_string(ht, (const char*)key);
		case NUMBER:
		default:
			return map_find_number(ht, (const int)key);
	}
	return NULL;
}

void map_remove(Map *ht, const void *key)
{
    MapNode *output;
    ListIterator iter;

	EMO_ASSERT(ht != NULL, "map remove on NULL map")
	EMO_ASSERT(key != NULL, "map remove missing key")

    list_begin(ht->list, &iter);
    while (!listIterator_finished(&iter)) {
        output = (MapNode *)listIterator_item(&iter);
        switch (ht->type) {
            case STRING:
				if (strcmp(output->key.kstring, (const char *)key) == 0) {
					listIterator_remove(&iter);
					p_free(output->key.kstring);
					p_free(output);
					/*listIterator_delete(iter);*/
                    return;
				}
                break;
            case NUMBER:
            default:
				if (output->key.number == (int)key) {
					listIterator_remove(&iter);
					p_free(output);
					/*listIterator_delete(iter);*/
                    return;
				}
                break;
        }
        listIterator_next(&iter);
    }

	/*listIterator_delete(iter);*/
}

void map_delete(Map *ht)
{
	ListIterator iter;
	MapNode *node;

	EMO_ASSERT(ht != NULL, "map delete on NULL map")

	do {
		list_begin(ht->list, &iter);
		if (listIterator_finished(&iter))
			break;
        node = (MapNode *)listIterator_item(&iter);
		switch (ht->type) {
			case STRING:
				p_free(node->key.kstring);
				break;
			default:
				break;
		}
		listIterator_remove(&iter);
		p_free(node);
	} while (1);
    list_delete(ht->list);
    p_free(ht);
}

void map_begin(Map *ht, MapIterator *iter)
{
	EMO_ASSERT(ht != NULL, "map begin on NULL map")
	EMO_ASSERT(iter != NULL, "map begin missing iterator")

	/*return (MapIterator *)*/
	list_begin(ht->list, iter);
}

int mapIterator_finished(MapIterator *iter)
{
	return listIterator_finished((ListIterator *)iter);
}

void *mapIterator_item(MapIterator *iter, void **key)
{
	MapNode *item;

	EMO_ASSERT_NULL(iter != NULL, "map item missing iterator")
	EMO_ASSERT_NULL(key != NULL, "map item missing key")
	
	item = (MapNode *)listIterator_item((ListIterator *)iter);
	if (key != NULL) {
		*key = *(void **)&item->key;
	}
	return item->data;
}

void mapIterator_remove(MapIterator *iter)
{
	MapNode *item;

	EMO_ASSERT(iter != NULL, "map remove missing iterator")
	
	item = (MapNode *)listIterator_item((ListIterator *)iter);
	listIterator_remove((ListIterator *)iter);
	p_free(item->key.kstring);
	p_free(item);
}

void mapIterator_removeInt(MapIterator *iter)
{
	MapNode *item;

	EMO_ASSERT(iter != NULL, "map removeInt missing iterator")
	
	item = (MapNode *)listIterator_item((ListIterator *)iter);
	listIterator_remove((ListIterator *)iter);
	p_free(item);
}

void mapIterator_next(MapIterator *iter)
{
	EMO_ASSERT(iter != NULL, "map next missing iterator")

	listIterator_next((ListIterator *)iter);
}

/*void mapIterator_delete(MapIterator *iter)
{
	listIterator_delete((ListIterator *)iter);
}*/
