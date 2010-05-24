#ifndef _DATAOBJECT_MAP_
#define _DATAOBJECT_MAP_

#include "List.h"

#ifdef __cplusplus
extern "C" {
#endif

struct DataObjectMap_t;
typedef struct DataObjectMap_t DataObjectMap;

DataObjectMap *dataobjectmap_new(void);
void dataobjectmap_setTranslation(DataObjectMap *dmap, const char *infield,
		const char *outfield);
List *dataobjectmap_translate(DataObjectMap *dmap, const char *field);

#ifdef __cplusplus
}
#endif

#endif /* _DATAOBJET_MAP_ */

