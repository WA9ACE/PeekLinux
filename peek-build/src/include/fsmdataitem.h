




#ifndef __FSM_DATAITEM_H__

#define __FSM_DATAITEM_H__

#include "fsmdefs.h"


typedef struct
{
	uint16                  ItemType;
	uint16                  ItemId;
	uint32			        ItemLength;
	uint8			        Attrib;
    uint8                   Method;
} FsmItemInfoT;

/* wildcard for data item search. */
#define WILDCARD_TYPE             0xFFFF
#define WILDCARD_ID               0xFFFF

/* Interface definition */

uint32		FsmDataItemOpen(uint16 type, uint16 id, uint32 mode);
uint32		FsmDataItemClose(uint32 ItemFd);
uint32		FsmDataItemWrite(uint16 type, uint16 id, uint32 offset, uint8 * buffer, uint32 size);
uint32		FsmDataItemRead(uint16 type, uint16 id, uint32 offset, uint8 * buffer, uint32 size);

uint32		FsmDataItemDelete(uint16 type, uint16 id);

uint32		FsmDataItemFindFirst(uint16 type, uint16 id, FsmItemInfoT * Info);

uint32		FsmDataItemFindNext(uint32 ItemFd, FsmItemInfoT * Info);

#define FsmDataItemFindClose(a) FsmDataItemClose(a)

uint32		FsmDataItemFlush(uint16 type, uint16 id);
uint32		FsmDataItemFlushAll(void);
uint32		FsmDataItemError(uint32 ItemFd);
uint32		FsmDataItemGetError(void);

/* interface define end */

#endif


