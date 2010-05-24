




#ifndef __FSM_FLASH_H__

#define __FSM_FLASH_H__


#include "fsmdefs.h"
#include "FsmDev.h"
#include "ostype.h"


/*******************************************
 *
 * The following definitions are used for
 * Device Driver.
 *
 *******************************************/

typedef uint16			FLASH_DATA;

typedef FLASH_DATA	*	FLASH_DATA_PTR;



/*******************************************
 *
 * The following definitions are used for
 * Device Driver & Applications.
 *
 *******************************************/


typedef struct
{
	FsmDevObjHdrT		DevObjHdr;
	uint32				BlockSize;
	uint16				Blocks;
	uint32				DevStartAddr;
} FsmFlashDevT;



#endif /* __FSM_FLASH_H__ */






