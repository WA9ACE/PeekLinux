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
*   FILE NAME:      btl_bmg_evm.c
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

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <bttypes.h>
#include "me.h"
#include "mexp.h"
#include "debug.h"
#include <sec.h>
#include <utils.h>
#include <config.h>

#include "btl_bmg_debug.h"
#include "btl_bmg_request.h"
#include "btl_defs.h"
#include "btl_bmg_evm.h"
#include "btl_pool.h"
#include "btl_utils.h"
#include "btl_apphandle.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BMG);

/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/

#define BTL_BMG_EVM_MAX_NUM_OF_REGISTERED_EVENTS		30
#define BTL_BMG_EVM_MAX_NUM_OF_REQUESTS_PER_EVENT		3

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*
	An entry that stores all the requests that registsred for a specific static event
*/
typedef struct _BtlBmgEvmStaticEventsMapEntry
{
	/* Entries are placed on a list */
	ListEntry			node;

	BtEventType		eventType;
	ListEntry			registeredRequests;	/* List of registered requests */
} BtlBmgEvmStaticEventsMapEntry; 

/*
	An entry for a single registered request
*/
typedef struct _BtlBmgEvmRegisteredRequestPerEventEntry
{
	ListEntry			node;
	
	BtlBmgRequest 	*request;
} BtlBmgEvmRegisteredRequestPerEventEntry; 

/*
	Data structure that holds the data of the events manager object
*/
typedef struct _BtlBmgEvmData
{
	BtHandler	globalHandlerBtHandler;
	BtHandler	authorizationHandlerBtHandler;	
	BtHandler	pairingHandlerBtHandler;

	/* */
	BtlBmgEvent	btlEvent;

	BtlBmgEvmCallBack	defaultBmgEvmHandler;

	/* Lists of BMG contexts - used when dispatching events */
	ListEntry				*contextsList;

	/* Map of all registsred requests for static events */
	ListEntry		staticEventsMap;	

	BTL_POOL_DECLARE_POOL(staticEventsMapPool, staticEventsMapPoolMemory, BTL_BMG_EVM_MAX_NUM_OF_REGISTERED_EVENTS, sizeof(BtlBmgEvmStaticEventsMapEntry));
	BTL_POOL_DECLARE_POOL(registeredRequestsPerEvensPool, registeredRequestsPerEvensPoolMemory, (BTL_BMG_EVM_MAX_NUM_OF_REGISTERED_EVENTS * BTL_BMG_EVM_MAX_NUM_OF_REQUESTS_PER_EVENT), sizeof(BtlBmgEvmRegisteredRequestPerEventEntry));

	/* Indicates whether security handlers are currently registered */
	BOOL		areSecHandlersRegistered;
} BtlBmgEvmData;

/* Definition of Events Manager data object */
static BtlBmgEvmData btlBmgEvmData;

/* Indicates if EVM was successfully created - prevents illegal usage */
BOOL btlBmgEvmCreated = FALSE;

static const char BtlBmgEvmEventsMapPoolName[] = "BmgEvmEvents";
static const char BtlBmgEvmEventRequestsMapPoolName[] = "BmgEvmEventRequests";


/********************************************************************************
 *
 * Forward Declarations
 *
 *******************************************************************************/
static void BTL_BMG_EVM_InitEvmData(void);

static BtStatus BTL_BMG_EVM_CreatePools(void);
static BtStatus BTL_BMG_EVM_DestroyPools(void);

BtStatus	BTL_BMG_EVM_InternalRegisterSecHandlers();

static BtStatus BTL_BMG_EVM_RegisterStaticHandlers(BOOL registerSecHandlers);

/*
	Handler for stack static events:
	1. Global handler events
	2. Pairing Handler events
	3. Authorization handler events
*/
static void  BTL_BMG_EVM_StaticHandler(const BtEvent *event);

/*
	Handler for all ESI events that are using an MeCommandToken
*/
static void BTL_BMG_EVM_MeTokenHandler(const BtEvent *event);

/*
	Handler for all ESI events that are using a BtHandler
*/
static void BTL_BMG_EVM_BtHandlerHandler(const BtEvent *event);

/*
	Comparison function that is used to find an event entry in a list, based on the type of the event
*/
static BOOL BTL_BMG_EVM_CompareEventEntries(const ListEntry *entryToMatch, const ListEntry* checkedEntry);

/*
	Obtains the map entry for a static event
*/
static BtlBmgEvmStaticEventsMapEntry *BTL_BMG_EVM_GetRegisteredEventEntry(BtEventType eventType);

/*
	Comparison function that is used to find a request based on the address of the request 
*/
static BOOL BTL_BMG_EVM_CompareRequestPerEventEntries(	const ListEntry *entryToMatch, 
																	const ListEntry* checkedEntry);

/*
	Obtains the request entry within the list of requests that are registered for a specific event (eventEntry)
*/
static BtlBmgEvmRegisteredRequestPerEventEntry *BTL_BMG_EVM_GetRegisteredRequestPerEventEntry(
												BtlBmgEvmStaticEventsMapEntry	*eventEntry,
												const BtlBmgRequest			*request);

/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/
 
BtStatus BTL_BMG_EVM_Create(	BtlBmgEvmCallBack 	defaultBmgEvmHandler,
									ListEntry				*contextsList,
									BOOL				registerSecHandlers)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_EVM_Create");

	BTL_VERIFY_FATAL(FALSE == btlBmgEvmCreated, BT_STATUS_INTERNAL_ERROR, ("BMG EVM Already Created"));
	BTL_VERIFY_FATAL((0 != defaultBmgEvmHandler), BT_STATUS_INTERNAL_ERROR, ("Null defaultBmgEvmHandler"));
	BTL_VERIFY_FATAL((0 != contextsList), BT_STATUS_INTERNAL_ERROR, ("Null contextsList"));

	BTL_BMG_EVM_InitEvmData();
	
	status = BTL_BMG_EVM_CreatePools();	
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, (""));
	
	btlBmgEvmData.defaultBmgEvmHandler = defaultBmgEvmHandler;
	btlBmgEvmData.contextsList = contextsList;
	
	status = BTL_BMG_EVM_RegisterStaticHandlers(registerSecHandlers);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, (""));

	btlBmgEvmData.areSecHandlersRegistered = registerSecHandlers;

	btlBmgEvmCreated = TRUE;
	
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_EVM_Destroy()
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("BTL_BMG_EVM_Destroy");
	
	BTL_VERIFY_FATAL(TRUE == btlBmgEvmCreated, BT_STATUS_INTERNAL_ERROR, ("BMG EVM Wasn't Created"));
	
	btlBmgEvmCreated = FALSE;
	
	status = BTL_BMG_EVM_DestroyPools();
	
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_EVM_RegisterForStaticEvents(	const BtEventType 	*staticEvents,
														U32				numOfStaticEvents,
														BtlBmgRequest 	*request)
{
	BtStatus 									status = BT_STATUS_SUCCESS;
	U32 										eventIndex = 0;
	BtlBmgEvmStaticEventsMapEntry				*eventEntry = 0;
	BtlBmgEvmRegisteredRequestPerEventEntry	*requestEntry = 0;

	BTL_FUNC_START("BTL_BMG_EVM_RegisterForStaticEvents");
	
	BTL_VERIFY_FATAL(TRUE == btlBmgEvmCreated, BT_STATUS_INTERNAL_ERROR, ("BMG EVM Wasn't Created"));
	
	for (eventIndex = 0; eventIndex < numOfStaticEvents; ++eventIndex)
	{
		/* Check if there is already a corresponding event entry */
		eventEntry = BTL_BMG_EVM_GetRegisteredEventEntry(staticEvents[eventIndex]);

		if (0 == eventEntry)
		{
			BTL_LOG_DEBUG(("Creating a new events map entry for event: %s", pME_Event(staticEvents[eventIndex])));
			
			/* No event entry, create one, initialize it, and insert into events map */
			
			status = BTL_POOL_Allocate(&btlBmgEvmData.staticEventsMapPool, (void **)(&eventEntry));
			BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));

			InitializeListEntry(&(eventEntry->node));
			eventEntry->eventType = staticEvents[eventIndex];
			InitializeListHead(&(eventEntry->registeredRequests));

			/* Insert the new entry into the map */
			InsertTailList(&btlBmgEvmData.staticEventsMap, &(eventEntry->node));
		}
		else
		{
			BTL_LOG_DEBUG(("There is already an event entry in the events map for event: %s", 
							pME_Event(staticEvents[eventIndex])));
		}

		/* eventEntry now surely points at an existing entry in the map */

		/* [@ToDo][Udi] Remove the assumption that the registered request is not already registered on this event */

		status = BTL_POOL_Allocate(&btlBmgEvmData.registeredRequestsPerEvensPool, (void **)(&requestEntry));

		if (BT_STATUS_SUCCESS != status)
		{
			BTL_POOL_Free(&btlBmgEvmData.staticEventsMapPool, (void **)(&eventEntry));

			BTL_VERIFY_ERR((BT_STATUS_SUCCESS == status), status, (""));
		}

		InitializeListEntry(&(requestEntry->node));
		requestEntry->request = request;

		/* Insert the new entry into the map */
		InsertTailList(&(eventEntry->registeredRequests), &(requestEntry->node));
	}

	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_EVM_DeRegisterFromStaticEvents(	const BtEventType 		*staticEvents,
															U32					numOfStaticEvents,
															const BtlBmgRequest 	*request)
{
	BtStatus 									status = BT_STATUS_SUCCESS;
	U32 										eventIndex = 0;
	BtlBmgEvmStaticEventsMapEntry				*eventEntry = 0;
	BtlBmgEvmRegisteredRequestPerEventEntry	*requestEntry = 0;

	BTL_FUNC_START("BTL_BMG_EVM_DeRegisterFromStaticEvents");
	
	BTL_VERIFY_FATAL(TRUE == btlBmgEvmCreated, BT_STATUS_INTERNAL_ERROR, ("BMG EVM Wasn't Created"));
	
	for (eventIndex = 0; eventIndex < numOfStaticEvents; ++eventIndex)
	{
		/* Obtain the corresponding event entry */
		eventEntry = BTL_BMG_EVM_GetRegisteredEventEntry(staticEvents[eventIndex]);
		BTL_VERIFY_FATAL((0 != eventEntry), BT_STATUS_INTERNAL_ERROR, 
							("Failed to locate registered event (%d) entry", staticEvents[eventIndex]))

		BTL_LOG_DEBUG(("De-Registering Request (%s) from Event (%s) ", 
						BTL_BMG_DEBUG_pRequestType(request->requestType), pME_Event(staticEvents[eventIndex])));

		requestEntry = BTL_BMG_EVM_GetRegisteredRequestPerEventEntry(eventEntry, request);
		BTL_VERIFY_FATAL((0 != requestEntry), BT_STATUS_INTERNAL_ERROR, ("Failed to locate registered request"))

		/* Remove the request entry (effectively de-registering) */
		RemoveEntryList(&(requestEntry->node));

		/* Release the memory of the request entry */
		BTL_POOL_Free(&btlBmgEvmData.registeredRequestsPerEvensPool, (void **)(&requestEntry));

		if (0 != IsListEmpty(&(eventEntry->registeredRequests)))
		{
			/* No more requests registered on this event entry - remove and release it */

			BTL_LOG_DEBUG(("No more requests in this event entry - removing event entry & Releasing ite memory"));
				
			RemoveEntryList(&(eventEntry->node));
				
			BTL_POOL_Free(&btlBmgEvmData.staticEventsMapPool, (void **)(&eventEntry));
		}
	}

	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_EVM_DispatchEvent(	BtlBmgRequest				*request, 
											const BtEvent 					*btEvent,
											BtlBmgEventForwardingMode		forwardingMode)
{	
	BtStatus 			status = BT_STATUS_SUCCESS;
	BtlContext		*appContext = 0;
	BtlBmgContext	*appBmgContext = 0;
	BOOL			forwardEvent = FALSE;

	BTL_FUNC_START("BTL_BMG_EVM_DispatchEvent");

	BTL_VERIFY_FATAL(TRUE == btlBmgEvmCreated, BT_STATUS_INTERNAL_ERROR, ("BMG EVM Wasn't Created"));
	
	BTL_VERIFY_FATAL(	(0 != request) || 
						((0 == request) && (BTL_BMG_EVENT_FORWARDING_ALL_EVENTS == forwardingMode)),
						BT_STATUS_INTERNAL_ERROR, ("Null request, but forwarding mode != ALL")); 
	
	if ((0 == request) || (TRUE == BTL_BMG_IsRequestTopLevel(request)))
	{		
		BTL_VERIFY_FATAL((0 != btlBmgEvmData.contextsList), BT_STATUS_INTERNAL_ERROR, ("Null btlBmgEvmData.contextsList"));

		BTL_LOG_DEBUG(("Dispatching Event (%s) to Applications, forwardingMode = %s", 
						pME_Event(btEvent->eType), BTL_BMG_DEBUG_pForwardingMode(forwardingMode)));
		
		/* A top-level event destined for the application */
		btlBmgEvmData.btlEvent.event = btEvent;

		IterateList((*btlBmgEvmData.contextsList), appContext, BtlContext*)
		{
			appBmgContext = (BtlBmgContext*)appContext;
			
			BTL_LOG_DEBUG(("Checking Application [%s], Forwarding Mode: %s", 
							appBmgContext->base.appHandle->appName, 
							BTL_BMG_DEBUG_pForwardingMode(appBmgContext->forwardingMode)));
			
			/* Set the context of the application that may receive the event */
			btlBmgEvmData.btlEvent.bmgContext = appBmgContext;

			forwardEvent = FALSE;

			switch (appBmgContext->forwardingMode)
			{				
				case BTL_BMG_EVENT_FORWARDING_ALL_EVENTS:

					switch (forwardingMode)
					{
						case BTL_BMG_EVENT_FORWARDING_ALL_EVENTS:

							forwardEvent = TRUE;
							
							break;

						case BTL_BMG_EVENT_FORWARDING_MY_EVENTS_ONLY:

							if ((0 != request) && (appBmgContext == request->bmgContext))
							{
								forwardEvent = TRUE;
							}

							break;

						default:

							BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Invalid Event Forwarding Mode: %d", forwardingMode));
							
					};
					
					break;

				case BTL_BMG_EVENT_FORWARDING_MY_EVENTS_ONLY:

					/* The application should receive only its own initiated events */
					if ((0 != request) && (appBmgContext == request->bmgContext))
					{
						forwardEvent = TRUE;
					}

					break;
					
				default:

					BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Invalid App Event Forwarding Mode: %d", appBmgContext->forwardingMode));
					
			};

			if (TRUE == forwardEvent)
			{
				BTL_LOG_INFO(("Forwarding Event To Application"));
				(appBmgContext->bmgCallcack)(&btlBmgEvmData.btlEvent);			
			}
			else
			{
				BTL_LOG_DEBUG(("NOT Forwarding Event To Application"));
			}
		}
	}
	else
	{
		/* A nested request => call the parent's callback */
		(request->parent->base.bmgCallback)(btEvent, &(request->parent->base));
	}

	BTL_FUNC_END();

	return status;
}

BtCallBack BTL_BMG_EVM_GetHandler(BtlBmgRequestHandlerType handlerType)
{
	BtCallBack	callBack = 0;

	BTL_FUNC_START("BTL_BMG_EVM_GetHandler");
	
	BTL_VERIFY_FATAL_SET_RETVAR(TRUE == btlBmgEvmCreated, callBack=0, ("BMG EVM Wasn't Created"));
	
	switch (handlerType)
	{
		case BTL_BMG_REQUEST_HANDLER_TYPE_ME_TOKEN:

			callBack =   BTL_BMG_EVM_MeTokenHandler;
			
		break;
		
		case BTL_BMG_REQUEST_HANDLER_TYPE_BT_HANDLER:

			callBack =   BTL_BMG_EVM_BtHandlerHandler;
			
		break;
		
		default:

			BTL_FATAL_SET_RETVAR(callBack = 0, ("Invalid Handler Type (%d)", handlerType));

	};

	BTL_FUNC_END();
	
	return callBack;
}

BtStatus	BTL_BMG_EVM_RegisterSecHandlers()
{
	BtStatus 		status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_EVM_RegisterSecHandlers");

	BTL_VERIFY_FATAL(TRUE == btlBmgEvmCreated, BT_STATUS_INTERNAL_ERROR, ("BMG EVM Wasn't Created"));

	status = BTL_BMG_EVM_InternalRegisterSecHandlers();

	BTL_FUNC_END();
	
	return status;
}

BtStatus	BTL_BMG_EVM_InternalRegisterSecHandlers()
{
	BtStatus 		status = BT_STATUS_SUCCESS;
	BtHandler	*handler = 0;

	BTL_FUNC_START("BTL_BMG_EVM_InternalRegisterSecHandlers");
	
	ME_InitHandler(&btlBmgEvmData.authorizationHandlerBtHandler);
	ME_InitHandler(&btlBmgEvmData.pairingHandlerBtHandler);

	/* Register handlers and verify that there was no other handler already registered */
	
	handler = SEC_RegisterAuthorizeHandler(&btlBmgEvmData.authorizationHandlerBtHandler);
	BTL_VERIFY_FATAL((0 == handler), BT_STATUS_INTERNAL_ERROR, ("Another Authorization handler was registered"));

	handler = SEC_RegisterPairingHandler(&btlBmgEvmData.pairingHandlerBtHandler);
	BTL_VERIFY_FATAL((0 == handler), BT_STATUS_INTERNAL_ERROR, ("Another Pairing handler was registered"));

	btlBmgEvmData.areSecHandlersRegistered = TRUE;

	BTL_FUNC_END();
	
	return status;
}

BtStatus	BTL_BMG_EVM_DeRegisterSecHandlers()
{	
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_EVM_DeRegisterSecHandlers");

	BTL_LOG_INFO(("De-Registering Security Handlers (Authorization, Pairing)"));
	
	BTL_VERIFY_FATAL(TRUE == btlBmgEvmCreated, BT_STATUS_INTERNAL_ERROR, ("BMG EVM Wasn't Created"));
	
	SEC_RegisterAuthorizeHandler(0);
	SEC_RegisterPairingHandler(0);

	btlBmgEvmData.areSecHandlersRegistered = FALSE;

	BTL_FUNC_END();

	return status;
}

BOOL BTL_BMG_EVM_AreSecHandlersRegistered()
{
	BOOL answer = btlBmgEvmData.areSecHandlersRegistered;

	BTL_FUNC_START("BTL_BMG_EVM_AreSecHandlersRegistered");

	BTL_VERIFY_FATAL_SET_RETVAR(TRUE == btlBmgEvmCreated, answer = FALSE, ("BMG EVM Wasn't Created"));
	
	BTL_FUNC_END();

	return answer;
}


void BTL_BMG_EVM_InitEvmData()
{
	ME_InitHandler(&btlBmgEvmData.globalHandlerBtHandler);
	ME_InitHandler(&btlBmgEvmData.authorizationHandlerBtHandler);
	ME_InitHandler(&btlBmgEvmData.pairingHandlerBtHandler);

	btlBmgEvmData.globalHandlerBtHandler.callback = BTL_BMG_EVM_StaticHandler;
	btlBmgEvmData.authorizationHandlerBtHandler.callback = BTL_BMG_EVM_StaticHandler;
	btlBmgEvmData.pairingHandlerBtHandler.callback = BTL_BMG_EVM_StaticHandler;

	InitializeListHead(&btlBmgEvmData.staticEventsMap);

	btlBmgEvmData.defaultBmgEvmHandler = 0;
	btlBmgEvmData.contextsList = 0;
}

BtStatus BTL_BMG_EVM_CreatePools()
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_EVM_CreatePools");

	status = BTL_POOL_Create(	&btlBmgEvmData.staticEventsMapPool,
								BtlBmgEvmEventsMapPoolName,
								btlBmgEvmData.staticEventsMapPoolMemory, 
								BTL_BMG_EVM_MAX_NUM_OF_REGISTERED_EVENTS,
								sizeof(BtlBmgEvmStaticEventsMapEntry));
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Events Map pool creation failed"));

	status = BTL_POOL_Create(	&btlBmgEvmData.registeredRequestsPerEvensPool,
								BtlBmgEvmEventRequestsMapPoolName,
								btlBmgEvmData.registeredRequestsPerEvensPoolMemory, 
								(	BTL_BMG_EVM_MAX_NUM_OF_REGISTERED_EVENTS *
									BTL_BMG_EVM_MAX_NUM_OF_REQUESTS_PER_EVENT),
								sizeof(BtlBmgEvmRegisteredRequestPerEventEntry));
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Registered Requests pool creation failed"));

	BTL_FUNC_END();

	return status;
}

BtStatus BTL_BMG_EVM_DestroyPools()
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_EVM_DestroyPools");

	status = BTL_POOL_Destroy(&btlBmgEvmData.staticEventsMapPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Static Events pool destruction failed"));

	status = BTL_POOL_Destroy(&btlBmgEvmData.registeredRequestsPerEvensPool);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("Static Events pool destruction failed"));
	
	BTL_FUNC_END();

	return status;
}

BtStatus BTL_BMG_EVM_RegisterStaticHandlers(BOOL registerSecHandlers)
{
	BtStatus 		status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_EVM_RegisterStaticHandlers");
	
	status = ME_IntRegisterGlobalHandler(&btlBmgEvmData.globalHandlerBtHandler);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("Failed registering global handler"));

	status = ME_IntSetEventMask(&btlBmgEvmData.globalHandlerBtHandler, BEM_ALL_EVENTS);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("Failed setting events mask for global handler"));

	if (TRUE == registerSecHandlers)
	{
		status = BTL_BMG_EVM_InternalRegisterSecHandlers();
		BTL_VERIFY_FATAL(BT_STATUS_SUCCESS == status, BT_STATUS_INTERNAL_ERROR, (""));
	}
	
	BTL_FUNC_END();
	
	return status;
}

void BTL_BMG_EVM_StaticHandler(const BtEvent *event)
{
	BtlBmgEvmStaticEventsMapEntry				*eventEntry = 0;
	BtlBmgEvmRegisteredRequestPerEventEntry	*requestEntry = 0;
	BtlBmgEvmRegisteredRequestPerEventEntry	*nextRequestEntry = 0;
	BtlBmgRequest							*request = 0;
	BOOL									eventHandled = FALSE;

	BTL_FUNC_START("BTL_BMG_EVM_StaticHandler");

	BTL_LOG_DEBUG(("Event Type: %s, Error Code: %d", pME_Event(event->eType), event->errCode));

	eventEntry = BTL_BMG_EVM_GetRegisteredEventEntry(event->eType);

	if (0 != eventEntry)
	{
		BTL_LOG_DEBUG(("There are registered requests for this event"));
		
		/* Forward the event to all the request-callbacks that were registered for this event */
		IterateListSafe(eventEntry->registeredRequests, requestEntry, nextRequestEntry, BtlBmgEvmRegisteredRequestPerEventEntry*)
		{
			request = requestEntry->request;

			BTL_LOG_INFO(("Forwarding Event (%d) To a BMG Internal Handler", event->eType));

			/* Forward the event */
			eventHandled = (request->bmgCallback)(event, request);
		}
	}

	if (FALSE == eventHandled)
	{
		BTL_VERIFY_FATAL_NORET((0 != btlBmgEvmData.defaultBmgEvmHandler), ("Null Default BMG Handler"));
		
		BTL_LOG_INFO(("Forwarding Event (%s) To the Default Internal BMG Handler", pME_Event(event->eType)));

		/* Forward the event */
		(btlBmgEvmData.defaultBmgEvmHandler)(event, request);
	}

	BTL_FUNC_END();
}

void BTL_BMG_EVM_MeTokenHandler(const BtEvent *event)
{
	BtlBmgDynamicRequest*	request = 0;

	BTL_FUNC_START("BTL_BMG_EVM_MeTokenHandler");

	BTL_LOG_DEBUG(("Event Type: %s, Error Code: %s", pME_Event(event->eType), pHC_Status(event->errCode)));

	/* Find the dynamic request that contains the token */
	request = ContainingRecord(event->p.meToken , BtlBmgDynamicRequest, token);

	BTL_VERIFY_FATAL_NO_RETVAR(	BTL_BMG_REQUEST_HANDLER_TYPE_ME_TOKEN == request->handlerType,
									("Invalid request type (%d)", request->handlerType));
	
	/* Call the corresponding BMG callback*/
	(request->base.bmgCallback)(event, &request->base);

	BTL_FUNC_END();
}

void BTL_BMG_EVM_BtHandlerHandler(const BtEvent *event)
{
	BtlBmgDynamicRequest*	request = 0;
	BtHandler				*btHandler = 0;

	BTL_FUNC_START("BTL_BMG_EVM_BtHandlerHandler");

	BTL_LOG_DEBUG(("Event Type: %s, Error Code: %s", pME_Event(event->eType), pHC_Status(event->errCode)));

	/* Obtain the handler from the correct field in the event, depending on the type of the event */
	
	switch (event->eType)
	{
		case BTEVENT_AUTHENTICATE_CNF:
			btHandler = event->p.remDev->authHandler;
			break;
		
		case BTEVENT_ENCRYPT_COMPLETE:
			btHandler = event->p.remDev->encryptHandler;
			break;

		case BTEVENT_SECURITY3_COMPLETE:
		case BTEVENT_LINK_CONNECT_CNF:
		case BTEVENT_LINK_CONNECT_REQ:
		case BTEVENT_AUTHENTICATED:

			btHandler = event->handler;

			break;

		default:

			BTL_ERR_NO_RETVAR(("Unexpected Event Received (%s)", pME_Event(event->eType)));
	};

	BTL_VERIFY_FATAL_NORET(0 != btHandler, ("Null btHandler"));
	
	/* Find the dynamic request that contains btHandler */
	request = ContainingRecord(btHandler, BtlBmgDynamicRequest, token);

	BTL_VERIFY_FATAL_NO_RETVAR(	BTL_BMG_REQUEST_HANDLER_TYPE_BT_HANDLER == request->handlerType,
									("Invalid request type (%d)", request->handlerType));

	/* Call the corresponding BMG callback*/
	(request->base.bmgCallback)(event, &request->base);

	BTL_FUNC_END();
}

BOOL BTL_BMG_EVM_CompareEventEntries(const ListEntry *entryToMatch, const ListEntry* checkedEntry)
{
	const BtlBmgEvmStaticEventsMapEntry		*eventEntryToMatch = (BtlBmgEvmStaticEventsMapEntry*)entryToMatch;
	const BtlBmgEvmStaticEventsMapEntry 	*checkedEventEntry = (BtlBmgEvmStaticEventsMapEntry*)checkedEntry;

	if (eventEntryToMatch->eventType == checkedEventEntry->eventType)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BtlBmgEvmStaticEventsMapEntry *BTL_BMG_EVM_GetRegisteredEventEntry(BtEventType eventType)
{
	BtlBmgEvmStaticEventsMapEntry	templateEntry;
	ListEntry						*matchingEntryAsListEntry = 0;
	

	templateEntry.eventType = eventType;
	
	BTL_UTILS_FindMatchingListEntry(&btlBmgEvmData.staticEventsMap,
									&(templateEntry.node),
									BTL_BMG_EVM_CompareEventEntries,
									&matchingEntryAsListEntry);

	return (BtlBmgEvmStaticEventsMapEntry*)matchingEntryAsListEntry;
}

BOOL BTL_BMG_EVM_CompareRequestPerEventEntries(const ListEntry *entryToMatch, const ListEntry* checkedEntry)
{
	const BtlBmgEvmRegisteredRequestPerEventEntry	*requestEntryToMatch = 
													(BtlBmgEvmRegisteredRequestPerEventEntry*)entryToMatch;
	
	const BtlBmgEvmRegisteredRequestPerEventEntry 	*checkedRequestEntry = 
													(BtlBmgEvmRegisteredRequestPerEventEntry*)checkedEntry;
	
	if (requestEntryToMatch->request == checkedRequestEntry->request)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BtlBmgEvmRegisteredRequestPerEventEntry *BTL_BMG_EVM_GetRegisteredRequestPerEventEntry(
											BtlBmgEvmStaticEventsMapEntry		*eventEntry,
											const BtlBmgRequest				*request)
{
	BtlBmgEvmRegisteredRequestPerEventEntry	templateEntry;
	ListEntry									*matchingEntryAsListEntry = 0;
	

	templateEntry.request = (BtlBmgRequest*)request;
	
	BTL_UTILS_FindMatchingListEntry(&(eventEntry->registeredRequests),
									&(templateEntry.node),
									BTL_BMG_EVM_CompareRequestPerEventEntries,
									&matchingEntryAsListEntry);

	return (BtlBmgEvmRegisteredRequestPerEventEntry*)matchingEntryAsListEntry;
}


