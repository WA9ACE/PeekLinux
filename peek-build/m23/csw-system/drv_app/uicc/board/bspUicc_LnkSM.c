/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */

#include "sys_types.h"
#include "types.h"
#include "bspUicc_Lnk.h"
#include "bspUicc_LnkSM.h"

/*=============================================================================
 * Description:
 *   Global variable to hold state information
 */
BspUicc_LnkSM_State bspUicc_LnkSM_state;

/*=============================================================================
 * Description
 *  Function to initialize the state machine's initial state to Null state
 */
void
bspUicc_LnkSM_init( void )
{
    bspUicc_LnkSM_state = BSP_UICC_LNK_SM_STATE_NULL;
}

/*=============================================================================
 * Description
 *  Function to handle all BSP_UICC Link layer events
 *  Implemented as nested switches, first on State and then on events
 */
BspUicc_LnkSM_ReturnCode
bspUicc_LnkSM_handleEvent( BspUicc_LnkSM_Event event,
                        void *eventDataPtr )
{
    BspUicc_LnkSM_ReturnCode status = BSP_UICC_LNK_SM_RETURN_CODE_FAILURE;
    switch ( bspUicc_LnkSM_state )
    {
        case  BSP_UICC_LNK_SM_STATE_NULL:
        {
            switch (event)
            {
                case  BSP_UICC_LNK_SM_EVENT_INIT_REQ:
                {
                    status = bspUicc_LnkSM_handleInitReq( eventDataPtr );
                    if( status == BSP_UICC_LNK_SM_RETURN_CODE_SUCCESS )
                    {
                        bspUicc_LnkSM_state = BSP_UICC_LNK_SM_STATE_INIT;
                    }
                    break;
                }
                default:
                {
                    status =  BSP_UICC_LNK_SM_RETURN_CODE_FAILURE;
                    break;
                }
            }
            break;
        }
        case BSP_UICC_LNK_SM_STATE_INIT:
        {
            switch (event)
            {
                case  BSP_UICC_LNK_SM_EVENT_INIT_REQ:
                {
                    status = bspUicc_LnkSM_handleInitReq( eventDataPtr );
                    if( status == BSP_UICC_LNK_SM_RETURN_CODE_SUCCESS )
                    {
                        bspUicc_LnkSM_state = BSP_UICC_LNK_SM_STATE_INIT;
                    }
                    break;
                }
                case BSP_UICC_LNK_SM_EVENT_RESET_REQ:
                {
                    status = bspUicc_LnkSM_handleResetReq( eventDataPtr );
                    if( status == BSP_UICC_LNK_SM_RETURN_CODE_SUCCESS )
                    {
                        bspUicc_LnkSM_state = BSP_UICC_LNK_SM_STATE_CONFIGURED;
                    }
                    break;
                }
                default:
                {
                    status =  BSP_UICC_LNK_SM_RETURN_CODE_FAILURE;
                    break;
                }
            }
            break;
        }
        case BSP_UICC_LNK_SM_STATE_CONFIGURED:
        {
            switch (event)
            {
                case BSP_UICC_LNK_SM_EVENT_INIT_REQ:
                {
                    status = bspUicc_LnkSM_handleInitReq( eventDataPtr );
                    if( status == BSP_UICC_LNK_SM_RETURN_CODE_SUCCESS )
                    {
                        bspUicc_LnkSM_state = BSP_UICC_LNK_SM_STATE_INIT;
                    }
                    break;
                }
                case BSP_UICC_LNK_SM_EVENT_RESET_REQ:
                {
                    status = bspUicc_LnkSM_handleResetReq( eventDataPtr );
                    if( status == BSP_UICC_LNK_SM_RETURN_CODE_SUCCESS )
                    {
                        bspUicc_LnkSM_state = BSP_UICC_LNK_SM_STATE_CONFIGURED;
                    }
                    break;
                }
                case BSP_UICC_LNK_SM_EVENT_SEND_CMD_REQ:
                {
                    status =  bspUicc_LnkSM_handleSendCommandReq( eventDataPtr );
                    break;
                }
                case BSP_UICC_LNK_SM_EVENT_POWER_OFF_REQ:
                {
                    status = bspUicc_LnkSM_handlePowerOffReq();
                    break;
                }
                default:
                {
                    status =  BSP_UICC_LNK_SM_RETURN_CODE_FAILURE;
                    break;
                }
            }
            break;
        }
    }
    return status;
}
