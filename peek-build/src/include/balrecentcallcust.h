


#ifndef BAL_RECENTCALL_CUST_H
#define BAL_RECENTCALL_CUST_H

#include "sysdefs.h"
#include "balapi.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define BAL_RC_MAX_RECORD_COUNT	200 	/* The max count of recent call records,128 */
#define BAL_RC_MAX_NAME_LENGTH	33 		/* The max length of recent call user name,32 bytes */
#define BAL_RC_MAX_DATA_LENGTH	245		/* Must be sizeof(ItemOtherT)!!  The max length of recent call data,128 bytes */

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif


