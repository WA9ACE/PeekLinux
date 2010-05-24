#ifndef _CACHE_H_
#define _CACHE_H_

#include "DataObject.h"
#include "URL.h"

#ifdef __cplusplus
extern "C" {
#endif

void cache_init(void);
DataObject *cache_loadObject(URL *url);
int cache_addToObject(DataObject *obj, int fromServer,
		void *buffer, int len);
int cache_commitObject(DataObject *dobj);

#ifdef __cplusplus
}
#endif

#endif /* _CACHE_H_ */
