

#ifndef BAL_SCHEDULER_FLASH_H
#define BAL_SCHEDULER_FLASH_H

#ifdef __cplusplus
extern "C" 
{
#endif 

#include "sysdefs.h"
#include "balapi.h"


/******************************************************************************
FUNCTION BalScheduleFlashOpen

DESCRIPTION
  Open scheduler flash file

Parameters:     

Returns:   BAL_RC_ERR_NONE is success, and false is other error code  

*******************************************************************************/
uint32 BalSchedulerFlashOpen(const char* pstrDbName, uint16 *  pDbHandle, uint16 iRecCount, uint16 iRecSize);


/******************************************************************************
FUNCTION BalSchedulerFlashClose

DESCRIPTION
  to close the scheduler flash.

Parameters:     

Returns:   BAL_RC_ERR_NONE is success, and false is other error code  

*******************************************************************************/
uint32 BalSchedulerFlashClose(uint16 *  pDbHandle);


/******************************************************************************
FUNCTION BalSchedulerFlashReadRec

DESCRIPTION
  read a record from scheduler flash file. 

Parameters: 
    recIndex [in] the record index
    pRecData  [out]the pointer to record

Returns:
    Returns:   BAL_RC_ERR_NONE is success, and false is other error code  
*******************************************************************************/
uint32 BalSchedulerFlashReadRec( uint16 dbHandle ,uint16 iRecIndex, void *pRecData,uint32* piRecSize );

/******************************************************************************
FUNCTION BalSchedulerFlashUpdateRec

DESCRIPTION
  update a record at scheduler flash. 

Parameters: 
    recIndex [in] the record index
    pRecData  [in] the pointer to rc record

Returns:   
	BAL_RC_ERR_NONE is success, and false is other error code  
*******************************************************************************/
uint32 BalSchedulerFlashUpdateRec(uint16 dbHandle ,uint16 iRecIndex, void *pRecData,uint32 iRecSize);

/******************************************************************************
FUNCTION BalSchedulerFlashDelRec

DESCRIPTION
  Delete a record from flash. 

Parameters:     
  recIndex [in] the record index
Returns:   
	BAL_RC_ERR_NONE is success, and false is other error code

*******************************************************************************/
uint32 BalSchedulerFlashDelRec(uint16 dbHandle ,uint16 iRecIndex);


#ifdef __cplusplus

}
#endif /* __cplusplus */
#endif 




