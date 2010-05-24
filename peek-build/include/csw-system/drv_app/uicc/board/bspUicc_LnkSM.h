/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_UICC_LNK_SM_HEADER
#define BSP_UICC_LNK_SM_HEADER

/*===========================================================================
 * Component Description:
 */
/*!  
 * @header bspUicc_LnkSM.h
 *  State machine for the link layer of BSP_UICC driver.
 */

/*=============================================================================*/
/*!
 * @typedef BspUicc_LnkSM_ReturnCode
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies the return code/status
 */
enum
{
    BSP_UICC_LNK_SM_RETURN_CODE_FAILURE   = (-1),
    BSP_UICC_LNK_SM_RETURN_CODE_SUCCESS   = (0)
};
typedef SYS_WORD16 BspUicc_LnkSM_ReturnCode;


/*=============================================================================
 * Typedefs
 */

/*=============================================================================*/
/*!
 * @typedef BspUicc_LnkSM_State
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies the state in a BSP_UICC Link layer state machine
 */
enum
{
    BSP_UICC_LNK_SM_STATE_NULL            = 0,
    BSP_UICC_LNK_SM_STATE_INIT            = 1,
    BSP_UICC_LNK_SM_STATE_CONFIGURED      = 2
};
typedef SYS_UWORD8 BspUicc_LnkSM_State;

/*=============================================================================*/
/*!
 * @typedef BspUicc_LnkSM_Event
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies the event in a BSP_UICC Link layer state machine
 */
enum
{
    BSP_UICC_LNK_SM_EVENT_INIT_REQ        = 0,
    BSP_UICC_LNK_SM_EVENT_RESET_REQ       = 1,
    BSP_UICC_LNK_SM_EVENT_SEND_CMD_REQ    = 2,
    BSP_UICC_LNK_SM_EVENT_POWER_OFF_REQ   = 3
};
typedef SYS_UWORD8 BspUicc_LnkSM_Event;

/*============================================================================*/
/*!
 * @function bspUicc_LnkSM_init
 * 
 * @discussion
 * <b> Description  </b><br>
 *     Initialize the BSP_UICC link layer state machine
 *
 * <b> Context </b><br>
 *     Called from protocol stack
 *
 * <b> Returns </b><br>
 *     None
 *
 * @param None
 *
 */
void bspUicc_LnkSM_init( void );

/*============================================================================*/
/*!
 * @function bspUicc_LnkSM_handleEvent
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This function manages the states,  Processes the BSP_UICC link layer events
 *  and calls the corresponding handler.
 *
 * <b> Context </b><br>
 *    Called to process any BSP_UICC link layer event
 *
 * <b> Returns </b><br>
 *     BspUicc_LnkSM_ReturnCode
 *     The return code indicates a success or failure
 *
 * @param event
 *        The event to be processed by the state machine
 *
 * @param eventDataPtr   
 *        Pointer to the received event's data structure
 *
 */
BspUicc_LnkSM_ReturnCode bspUicc_LnkSM_handleEvent( BspUicc_LnkSM_Event event,
                                              void *eventDataPtr );

/*===========================================================================*/
/*!
 * @function bspUicc_LnkSM_handleInitReq
 * 
 * @discussion
 * <b> Description  </b><br>
 *     Handles the Initialize event
 *
 * <b> Context </b><br>
 *     Called from the state machine's event handler.
 *
 * <b> Returns </b><br>
 *     BspUicc_LnkSM_ReturnCode
 *     The return code indicates a success or failure
 *
 * @param eventDataPtr   
 *        Pointer to the received event's data structure
 *
 */
BspUicc_LnkSM_ReturnCode bspUicc_LnkSM_handleInitReq( BspUicc_Lnk_InitReq* eventDataPtr );

/*===========================================================================*/
/*!
 * @function bspUicc_LnkSM_handleResetReq
 * 
 * @discussion
 * <b> Description  </b><br>
 *     Handles the Reset  request
 *
 * <b> Context </b><br>
 *     Called from the state machine's event handler.
 *
 * <b> Returns </b><br>
 *     BspUicc_LnkSM_ReturnCode
 *     The return code indicates a success or failure
 *
 * @param eventDataPtr   
 *        Pointer to the received event's data structure
 * 
 */
BspUicc_LnkSM_ReturnCode bspUicc_LnkSM_handleResetReq( BspUicc_Lnk_ResetReq* eventDataPtr );

/*===========================================================================*/
/*!
 * @function bspUicc_LnkSM_handleWriteReq
 * 
 * @discussion
 * <b> Description  </b><br>
 *     Handles the Write  request
 *
 * <b> Context </b><br>
 *     Called from the state machine's event handler.
 *
 * <b> Returns </b><br>
 *     BspUicc_LnkSM_ReturnCode
 *     The return code indicates a success or failure
 *
 * @param eventDataPtr   
 *        Pointer to the received event's data structure
 *
 */
BspUicc_LnkSM_ReturnCode bspUicc_LnkSM_handleSendCommandReq( BspUicc_Lnk_SendCommandReq* eventDataPtr );

/*===========================================================================*/
/*!
 * @function bspUicc_LnkSM_handlePowerOffReq
 * 
 * @discussion
 * <b> Description  </b><br>
 *     Handles the PowerOff  request
 *
 * <b> Context </b><br>
 *     Called from the state machine's event handler.
 *
 * <b> Returns </b><br>
 *     BspUicc_LnkSM_ReturnCode
 *     The return code indicates a success or failure
 *
 * @param None
 *
 */
BspUicc_LnkSM_ReturnCode bspUicc_LnkSM_handlePowerOffReq( void );

#endif
