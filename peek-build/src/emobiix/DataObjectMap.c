#include "DataObjectMap.h"
#include "List.h"

#include "p_malloc.h"

#include <stdlib.h>
#include <string.h>

struct DataObjectMap_t
{
	List *mapping;
};

struct Mapping_t
{
	char *from;
	List *to;
};
typedef struct Mapping_t Mapping;

DataObjectMap *dataobjectmap_new(void)
{
	DataObjectMap *output;
	
	output = (DataObjectMap *)p_malloc(sizeof(DataObjectMap));
	output->mapping = list_new();

	return output;
}

void dataobjectmap_setTranslation(DataObjectMap *dmap, const char *from,
		const char *to)
{
	Mapping *item;
	ListIterator iter;
	int wasFound;

	wasFound = 0;
	list_begin(dmap->mapping, &iter);
	while (!listIterator_finished(&iter)) {
		item = (Mapping *)listIterator_item(&iter);
		if (strcmp(item->from, from) == 0) {
			wasFound = 1;
			break;
		}
	}
	/*listIterator_delete(iter);*/

	if (!wasFound) {
		item = (Mapping *)p_malloc(sizeof(Mapping));
		item->from = p_strdup(from);
		item->to = list_new();
		list_append(dmap->mapping, item);
	}
	list_append(item->to, (void *)p_strdup(to));
}

List *dataobjectmap_translate(DataObjectMap *dmap, const char *field)
{
	Mapping *item;
	ListIterator iter;
	List *output;
	
	list_begin(dmap->mapping, &iter);
	while (!listIterator_finished(&iter)) {
		item = (Mapping *)listIterator_item(&iter);
		if (strcmp(item->from, field) == 0) {
			output = item->to;
			/*listIterator_delete(iter);*/
			return output;
		}
	}

	/*listIterator_delete(iter);*/

	return NULL;
}

