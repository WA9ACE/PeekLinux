#include "Map.h"

#include "List.h"

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

void *map_find(Map *ht, const void *key)
{
    MapNode *output;
    ListIterator iter;

    list_begin(ht->list, &iter);
    while (!listIterator_finished(&iter)) {
        output = (MapNode *)listIterator_item(&iter);
        switch (ht->type) {
            case STRING:
				if (strcmp(output->key.kstring, (const char *)key) == 0) {
					/*listIterator_delete(iter);*/
                    return output->data;
				}
                break;
            case NUMBER:
            default:
				if (output->key.number == (int)key) {
					/*listIterator_delete(iter);*/
                    return output->data;
				}
                break;
        }
        listIterator_next(&iter);
    }

	/*listIterator_delete(iter);*/

    return NULL;
}

void map_remove(Map *ht, const void *key)
{
    MapNode *output;
    ListIterator iter;

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
	item = (MapNode *)listIterator_item((ListIterator *)iter);
	if (key != NULL) {
		*key = *(void **)&item->key;
	}
	return item->data;
}

void mapIterator_remove(MapIterator *iter)
{
	MapNode *item;
	item = (MapNode *)listIterator_item((ListIterator *)iter);
	listIterator_remove((ListIterator *)iter);
	p_free(item->key.kstring);
	p_free(item);
}

void mapIterator_removeInt(MapIterator *iter)
{
	MapNode *item;
	item = (MapNode *)listIterator_item((ListIterator *)iter);
	listIterator_remove((ListIterator *)iter);
	p_free(item);
}

void mapIterator_next(MapIterator *iter)
{
	listIterator_next((ListIterator *)iter);
}

/*void mapIterator_delete(MapIterator *iter)
{
	listIterator_delete((ListIterator *)iter);
}*/
