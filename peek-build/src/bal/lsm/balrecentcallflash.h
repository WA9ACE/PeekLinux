


#ifndef BAL_RECENTCALL_FLASH_H
#define BAL_RECENTCALL_FLASH_H

#ifdef __cplusplus
extern "C" 
{
#endif 

#include "sysdefs.h"
#include "balapi.h"


/******************************************************************************
FUNCTION BalRcFlashOpen

DESCRIPTION
  Open recentcall flash file

Parameters:     

Returns:   BAL_RC_ERR_NONE is success, and false is other error code  

*******************************************************************************/
uint32 BalRcFlashOpen(const char* pstrDbName, uint16 *  pDbHandle, uint16 iRecCount, uint16 iRecSize);


/******************************************************************************
FUNCTION BalRcFlashClose

DESCRIPTION
  to close the recentcall flash.

Parameters:     

Returns:   BAL_RC_ERR_NONE is success, and false is other error code  

*******************************************************************************/
uint32 BalRcFlashClose(uint16 *  pDbHandle);


/******************************************************************************
FUNCTION BalRcFlashReadRec

DESCRIPTION
  read a recent record from rc flash file. 

Parameters: 
    recIndex [in] the record index
    pRecData  [out]the pointer to record

Returns:
    Returns:   BAL_RC_ERR_NONE is success, and false is other error code  
*******************************************************************************/
uint32 BalRcFlashReadRec( uint16 dbHandle ,uint16 iRecIndex, void *pRecData,uint32* piRecSize );

/******************************************************************************
FUNCTION BalRcFlashUpdateRec

DESCRIPTION
  update a rc record at rc flash. 

Parameters: 
    recIndex [in] the record index
    pRecData  [in] the pointer to rc record

Returns:   
	BAL_RC_ERR_NONE is success, and false is other error code  
*******************************************************************************/
uint32 BalRcFlashUpdateRec(uint16 dbHandle ,uint16 iRecIndex, void *pRecData,uint32 iRecSize);

/******************************************************************************
FUNCTION BalRcFlashDelRec

DESCRIPTION
  Delete a rc record from flash. 

Parameters:     
  recIndex [in] the record index
Returns:   
	BAL_RC_ERR_NONE is success, and false is other error code

*******************************************************************************/
uint32 BalRcFlashDelRec(uint16 dbHandle ,uint16 iRecIndex);


#ifdef __cplusplus

}
#endif /* __cplusplus */
#endif 




