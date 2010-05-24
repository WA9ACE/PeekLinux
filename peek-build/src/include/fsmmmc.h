#ifndef __FSM_MMC_H__

#define __FSM_MMC_H__

#include "fsmdefs.h"
#include "fsmDev.h"
#include "ostype.h"


typedef  struct
{
	FsmDevObjHdrT		DevObjHdr;
	uint32				DevStartAddr;
	uint32				DevSize;
} FsmSDCardDevT;

enum
{
	DEV_CTRL_GET_BLK_LEN = DEV_CTRL_END,
	DEV_CTRL_SET_DATA_BUS
};

#endif

