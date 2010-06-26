/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */

#include "sys_types.h"
#include "types.h"
#include "bspUicc_Phy.h"
#include "bspUicc_Lnk.h"
#include "bspUicc_LnkSM.h"
#include "bspUicc_Lnk_resetMgr.h"
#include "bspUicc_Lnk_dataConfig.h"
#include "bspUicc_Lnk_t0.h"
#include "bspUicc_Lnk_t1.h"

/*=============================================================================
 *  File Description:
 *  Source File with code for the Link layer state machine handlers
 */

/*=============================================================================
 *   Public Functions
 *============================================================================*/

/*=============================================================================
 * Description:
 *    Handles the Initialize event
 */
BspUicc_LnkSM_ReturnCode
bspUicc_LnkSM_handleInitReq( BspUicc_Lnk_InitReq* eventDataPtr ) 
{
    

    BspUicc_Phy_CardAbsentDetectHandler  removeFunc;
    removeFunc = eventDataPtr->removeFunc;
    
    bspUicc_Phy_init( );
    bspUicc_Phy_init_CardDetectFuncs( &removeFunc );
    
    return (BSP_UICC_LNK_RETURN_CODE_SUCCESS);
}

/*=============================================================================
 * Description:
 *    Handles the reset event
 */
BspUicc_LnkSM_ReturnCode
bspUicc_LnkSM_handleResetReq( BspUicc_Lnk_ResetReq* eventDataPtr )
{
    BspUicc_LnkSM_ReturnCode status;
    status = bspUicc_Lnk_resetMgrStartAtr( eventDataPtr->resetDataPtr,
                                           eventDataPtr->resetDataSizePtr,
                                           eventDataPtr->voltageSelect,
                                           eventDataPtr->resetNewVoltClass,
                                           eventDataPtr->newVoltClass);                                       
   
    return status;
}

/*=============================================================================
 * Description:
 *    Handles the send command event
 */
BspUicc_LnkSM_ReturnCode
bspUicc_LnkSM_handleSendCommandReq( BspUicc_Lnk_SendCommandReq* eventDataPtr )
{
    BspUicc_LnkSM_ReturnCode status;

    if( bspUicc_Lnk_dataConfigGetProtocol() == BSP_UICC_LNK_RESET_MGR_T0_PROTOCOL)
    {
        status = bspUicc_Lnk_t0WriteCommand( eventDataPtr->class,
                                             eventDataPtr->instruction,
                                             eventDataPtr->parameter1,
                                             eventDataPtr->parameter2,
                                             eventDataPtr->writeDataSize,
                                             eventDataPtr->writeDataPtr,
                                             eventDataPtr->readDataSize,
                                             eventDataPtr->readDataPtr,
                                             eventDataPtr->resultSizePtr,
                                             eventDataPtr->commandType );

    }
    else
    {
        status = bspUicc_Lnk_t1WriteCommand( eventDataPtr->class,
                                             eventDataPtr->instruction,
                                             eventDataPtr->parameter1,
                                             eventDataPtr->parameter2,
                                             eventDataPtr->writeDataSize,
                                             eventDataPtr->writeDataPtr,
                                             eventDataPtr->readDataSize,
                                             eventDataPtr->readDataPtr,
                                             eventDataPtr->resultSizePtr,
                                             eventDataPtr->commandType ); 
        
    }
    return status;
}

/*=============================================================================
 * Description:
 *    Handles the PowerOff request event
 */
BspUicc_LnkSM_ReturnCode
bspUicc_LnkSM_handlePowerOffReq( void )
{
    BspUicc_LnkSM_ReturnCode status;
    status = bspUicc_Phy_powerOff();
    return status;
}
