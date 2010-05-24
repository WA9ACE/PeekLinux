


#ifndef BAL_RECENTCALL_DEF_H
#define BAL_RECENTCALL_DEF_H

#include "sysdefs.h"
#include "balapi.h"
#include "balrecentcallapi.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
	uint8 iStartIndex;	/* the start index of search operation */
	uint16 iRecId;		/* current record id */
}BalRcSearchPosT;

/* Bal recentcall summary info struct,just used by record cache */
typedef struct
{
	BalRcTypeT  RcType;						/* the type of recent call,see BalRcTypeT */
	uint32 iSortKey; 						/* sort key,it will be the same as sequence value when the sort mode is operation sequence. */
	uint16 iNextRecId;						/* Record id,flag the address where the next call log is stored and it is ordered by current sorting mode */
	uint8  Name;	/* user name */
}BalRcSummaryInfoT;

/*****************************************************************************

FUNCTION NAME: BalRcCustInit

DESCRIPTION:
        Initializing cust param for recent call system
        internal api for BalRcInit to use it.
PARAMETERS:
        null;
        
RETURNED VALUES:
        null;
        
*****************************************************************************/
void BalRcCustInit(void);

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif



