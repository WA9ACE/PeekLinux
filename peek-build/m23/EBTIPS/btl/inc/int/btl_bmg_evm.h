/*******************************************************************************\
*                                                                           	*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      btl_bmg_evm.h
*
*   DESCRIPTION:    
*
*	BMG Events Manager
*
*	The events manager is a generic manager of stack BMG events. It registers itself with ESI
*	and receives all BMG events. It dispatches the events to either BTL BMG handlers, or directly
*	to application callbacks.
*
*	The events manager is a singleton.
*
*   AUTHOR:         Udi Ron
*
\*******************************************************************************/

#ifndef __BTL_BMG_EVM_H
#define __BTL_BMG_EVM_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <bttypes.h>
#include <me.h>
#include <utils.h>
#include <btl_bmgi.h>
#include <btl_bmg_evm_defs.h>
#include <btl_bmg_request.h>

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/*
	Forward Declaration
*/
#ifndef BTL_BMG_REQUEST_STRUCT
#define BTL_BMG_REQUEST_STRUCT
typedef struct _BtlBmgRequest BtlBmgRequest;
#endif

/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_BMG_EVM_Create()
 *
 *		Create the singleton instance.
 *
 * Parameters:
 *		defaultBmgEvmHandler [in] - 	Default handler that will be called in case no handler should explicitly handle
 *									the event. 
 *									It is mandatory to provide a valid default handler.
 *
 *		contextsList [in] - Pointer to the list of BMG contexts.
 *		
 *		registerSecHandlers [out] - Indicates whether security handlers should be initially registered or not.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - BMG context was created successfully.
 *
 *		BT_STATUS_INTERNAL_ERROR -  Some fatal error has occurred.
 */
BtStatus BTL_BMG_EVM_Create(	BtlBmgEvmCallBack	defaultBmgEvmHandler,
									ListEntry				*contextsList,
									BOOL				registerSecHandlers);

/*-------------------------------------------------------------------------------
 * BTL_BMG_EVM_Destroy()
 *
 *		Destroys the singleton instance.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - BMG context was created successfully.
 *
 *		BT_STATUS_INTERNAL_ERROR -  Some fatal error has occurred.
 */
BtStatus BTL_BMG_EVM_Destroy(void);

/*-------------------------------------------------------------------------------
 * BTL_BMG_EVM_RegisterForStaticEvents()
 *
 *		Registers request for the specified static events
 *
 *		Once registered, the callback stored in the request will receive the events in
 *		staticEvents.
 *
 * Parameters:
 *		staticEvents [in] - An array of static events. request registers for all of these events
 *
 *		numOfStaticEvents [in] - Number of static events in staticEvents.
 *		
 *		request [in] - The registered request.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - BMG context was created successfully.
 *
 *		BT_STATUS_INTERNAL_ERROR -  Some fatal error has occurred.
 */
BtStatus BTL_BMG_EVM_RegisterForStaticEvents(	const BtEventType 	*staticEvents,
														U32				numOfStaticEvents,
														BtlBmgRequest 	*request);

/*-------------------------------------------------------------------------------
 * BTL_BMG_EVM_DeRegisterFromStaticEvents()
 *
 *		Revokes the registeration of request from the specified static events
 *
 *		request must have previously registered for all of the events in staticEvents
 *
 * Parameters:
 *		staticEvents [in] - An array of static events. request de-registers from all of these events
 *
 *		numOfStaticEvents [in] - Number of static events in staticEvents.
 *		
 *		request [in] - The request that is de-registering.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - BMG context was created successfully.
 *
 *		BT_STATUS_INTERNAL_ERROR -  Some fatal error has occurred.
 */
BtStatus BTL_BMG_EVM_DeRegisterFromStaticEvents(	const BtEventType 		*staticEvents,
															U32					numOfStaticEvents,
															const BtlBmgRequest 	*request);

/*-------------------------------------------------------------------------------
 * BTL_BMG_EVM_DispatchEvent()
 *
 *		Dispatches btEvent to the appropriate callback. There are 2 cases:
 *		1. The request is a sub-request of a compound BTL BMG request.
 *		2. The request is a top-level request
 *
 *		In case 1, the event is dispatched to the callback of the containing request. This callback 
 *		will process the event and decide how to handle it.
 *
 *		In case 2, the event is destined to the clients of the BTL BMG module. The event is dispatched
 *		to all applicable client callbacks, depending on the configured forwarding mode of the
 *		BMG context, and the forwarding mode of the dispatched event (forwardingMode). forwadingMode
 *		is set by the caller, according to the type of the event and the specific request. For example, a name result
 *		event will have forwardingMode = BTL_BMG_EVENT_FORWARDING_MY_EVENTS_ONLY, whereas an inquiry
 *		result will have forwardingMode = BTL_BMG_EVENT_FORWARDING_ALL_EVENTS
 *
 * Parameters:
 *		request [in] - The request that originates the dispatching
 *
 *		btEvent [in] - The dispatched event.
 *		
 *		forwaringMode [in] - The forwarding mode of btEvent
 *
 * Returns:
 *		BT_STATUS_SUCCESS - BMG context was created successfully.
 *
 *		BT_STATUS_INTERNAL_ERROR -  Some fatal error has occurred.
 */
BtStatus BTL_BMG_EVM_DispatchEvent(	BtlBmgRequest				*request,
											const BtEvent 					*btEvent,
											BtlBmgEventForwardingMode		forwaringMode);

BtCallBack BTL_BMG_EVM_GetHandler(BtlBmgRequestHandlerType handlerType);

/*-------------------------------------------------------------------------------
 * BTL_BMG_EVM_RegisterSecHandlers()
 *
 *		Registes the security handlers (required for security mode >= BSM_SEC_LEVEL_1)
 *
 * Returns:
 *		BT_STATUS_SUCCESS - BMG context was created successfully.
 *
 *		BT_STATUS_INTERNAL_ERROR -  Some fatal error has occurred.
 */
BtStatus	BTL_BMG_EVM_RegisterSecHandlers(void);

/*-------------------------------------------------------------------------------
 * BTL_BMG_EVM_DeRegisterSecHandlers()
 *
 *		De-Registes the security handlers (required for security mode == BSM_SEC_DISABLED)
 *
 * Returns:
 *		BT_STATUS_SUCCESS - BMG context was created successfully.
 *
 *		BT_STATUS_INTERNAL_ERROR -  Some fatal error has occurred.
 */
BtStatus BTL_BMG_EVM_DeRegisterSecHandlers(void);

/*-------------------------------------------------------------------------------
 * BTL_BMG_EVM_AreSecHandlersRegistered()
 *
 *		Checks if the security handlers are currently registered (assumes the events manager is the only entity that registers
 *		them)
 *
 * Returns:
 *		BT_STATUS_SUCCESS - BMG context was created successfully.
 *
 *		BT_STATUS_INTERNAL_ERROR -  Some fatal error has occurred.
 */
BOOL BTL_BMG_EVM_AreSecHandlersRegistered(void);

#endif /* __BTL_BMG_EVM_H */



