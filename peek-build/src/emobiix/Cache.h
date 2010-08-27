#ifndef _CACHE_H_
#define _CACHE_H_

#include "DataObject.h"
#include "URL.h"

#ifdef __cplusplus
extern "C" {
#endif

void cache_init(void);
int cache_commitServerSide(DataObject *dobj, URL *url);
int cache_commitClientSide(DataObject *dobj, URL *url);
DataObject *cache_loadObject(URL *url);
int cache_commit(void);

#ifdef __cplusplus
}
#endif

#endif /* _CACHE_H_ */
