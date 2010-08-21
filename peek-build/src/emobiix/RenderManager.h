#ifndef _RENDERMANAGER_H_
#define _RENDERMANAGER_H_

#include "DataObject_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

void renderman_init(void);
void renderman_queue(DataObject *widget);
void renderman_dequeue(DataObject *widget);
void renderman_clearQueue(void);
void renderman_markLayoutChanged(void);
void renderman_flush(void);

#ifdef __cplusplus
}
#endif

#endif

