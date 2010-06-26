/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */

#include "sys_types.h"
#include "types.h"
#include "general.h"
#include "bspUicc_Phy.h"
#include "bspUicc_Phy_intC.h"
#include "bspUicc_Lnk.h"
#include "bspUicc_LnkSM.h"

/*=============================================================================
 *  File Description:
 *  Source File with BSP_UICC Link Layer code
 */

/*=============================================================================
 *   Public Functions
 *============================================================================*/

/*=============================================================================
 * Description:
 *   Initialize the Link layer of the BSP_UICC device driver
 *   Called during poweron or from mainAppInit
 */
void
bspUicc_Lnk_init( void )
{
    /* Initialize the Link Layer State machine */
    bspUicc_LnkSM_init();
    /* Initialize the Physical layer, this should power on the card */
    bspUicc_Phy_init();
    /* Register the interrupt handlers */
    bspUicc_Phy_intCInit();
}

/*=============================================================================
 * Description:
 *   initiate the link layer initialize event
 *   init request from the SIM entity
*/
BspUicc_Lnk_ReturnCode
bspUicc_Lnk_initReq( BspUicc_Lnk_InitReq *eventDataPtr )
{
    BspUicc_Phy_ReturnCode status;

    /* process the link layer initialize event */
    status = bspUicc_LnkSM_handleEvent( BSP_UICC_LNK_SM_EVENT_INIT_REQ,
                                     eventDataPtr );
    return status;
}

/*=============================================================================
 * Description:
 *   Reset the 
 *   initiate the link layer reset event
 */

BspUicc_Lnk_ReturnCode
bspUicc_Lnk_resetReq( BspUicc_Lnk_ResetReq *eventDataPtr )
{
    BspUicc_Lnk_ReturnCode status;
    status = bspUicc_LnkSM_handleEvent( BSP_UICC_LNK_SM_EVENT_RESET_REQ,
                                        eventDataPtr );
    return status;
}

/*=============================================================================
 * Description:
 *   Reset the 
 *   Initiate the link layer Send command event
 */
BspUicc_Lnk_ReturnCode
bspUicc_Lnk_sendCommandReq( BspUicc_Lnk_SendCommandReq *eventDataPtr )
{
    BspUicc_Lnk_ReturnCode status;
    
    status = bspUicc_LnkSM_handleEvent( BSP_UICC_LNK_SM_EVENT_SEND_CMD_REQ,
                                     eventDataPtr );
   
    return status;
}

/*=============================================================================
 * Description:
 *   Reset the 
 *   initiate the link layer power off event
 */
BspUicc_Lnk_ReturnCode
bspUicc_Lnk_powerOffReq( void )
{
    BspUicc_Lnk_ReturnCode status;
    status = bspUicc_LnkSM_handleEvent( BSP_UICC_LNK_SM_EVENT_POWER_OFF_REQ,
                                     (void*) NULL );
    return status;
}


