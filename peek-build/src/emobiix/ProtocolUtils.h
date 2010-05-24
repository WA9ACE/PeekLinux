#ifndef _PROTOCOL_UTILS_H_
#define _PROTOCOL_UTILS_H_

#include "FRIPacketP.h"
#include "DataObjectSyncP.h"

void protocolFreeFRIPacketP(FRIPacketP_t *p);
void protocolFreeFRIPacketP_children(FRIPacketP_t *p);
void protocolFreeDataObjectSyncP(DataObjectSyncP_t *p);
void protocolFreeDataObjectSyncP_children(DataObjectSyncP_t *p);

#endif

