#ifndef _MIME_H_
#define _MIME_H_

#include "DataObject_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

int mime_load(DataObject *dobj);
void mime_loadAll(DataObject *doj);

#ifdef __cplusplus
}
#endif

#endif