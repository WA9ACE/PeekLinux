#include "StaticList.h"

#include <stdlib.h>
#include <string.h>

void *staticlist_find(StaticList *l, const char *key)
{
	int i;

	if (l == NULL || key == NULL)
		return NULL;

	for (i = 0; l[i].key != NULL; ++i) {
		if (strcmp(l[i].key, key) == 0)
			return l[i].data;
	}
	return NULL;
}

