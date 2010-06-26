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
*   FILE NAME:      btl_bmg_request.h
*
*   DESCRIPTION:  
*
*	This file contains definitions for BMG Request "classes".
*
*	A request "class" represent a single BMG request. There are several types of requests. They
*	are organized in a C++-simulated hirearchy:
*
*	1. A simple request (the "base class").
*	2. A Compund request
*	3. A dynamic request
*	
*	A dynamic request is a BMG request that has a dynamic token associated with it. For example, 
*	a name request that uses a MeCommandToken.
*
*	A compound request is a request that is composed of one or more sub-requests. For example, 
*	a search request is composed of an inquiry, (possibly) name requests, and (possibly) SDP requests.
*	The compound request contains its sub-requests. The sub-requests are children of the compound request.
*
*   AUTHOR:         Udi Ron
*
\*******************************************************************************/

#ifndef __BTL_BMG_REQUEST_H
#define __BTL_BMG_REQUEST_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <bttypes.h>
#include <utils.h>
#include <btl_bmgi.h>
#include <btl_bmg_evm_defs.h>
#include <btl_defs.h>

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/*
	Types of all possible requests
*/
typedef enum _BtlBmgRequestType
{
	BTL_BMG_REQUEST_TYPE_UNSPECIFIED = 0,	/* Used for auxiliary requests (e.g., templates for searching) */
	BTL_BMG_REQUEST_TYPE_SEARCH_BY_COD = 1,
	BTL_BMG_REQUEST_TYPE_SEARCH_BY_DEVICES = 2,
	BTL_BMG_REQUEST_TYPE_INQUIRY = 3,
	BTL_BMG_REQUEST_TYPE_NAME_REQUEST = 4,
	BTL_BMG_REQUEST_TYPE_SEND_HCI_COMMAND = 6,
	BTL_BMG_REQUEST_TYPE_DISCOVER_SERVICES = 7,
	BTL_BMG_REQUEST_TYPE_BOND = 8,
	BTL_BMG_REQUEST_TYPE_CREATE_LINK = 9,
	BTL_BMG_REQUEST_TYPE_SET_SECURITY_MODE = 10,
	BTL_BMG_REQUEST_TYPE_ENABLE_SEC_MODE_3 = 11,
	BTL_BMG_REQUEST_TYPE_DISABLE_SEC_MODE_3 = 12,
	BTL_BMG_REQUEST_TYPE_AUTHENTICATE_LINK = 13,
	BTL_BMG_REQUEST_TYPE_ENABLE_TEST_MODE = 14,
	BTL_BMG_REQUEST_TYPE_RADIO_ON = 15,
	BTL_BMG_REQUEST_TYPE_SET_LINK_ENCRYPTION = 16,
	BTL_BMG_REQUEST_TYPE_SET_CONTROL_INCOMING_CON_ACCEPT = 17,
	BTL_BMG_REQUEST_TYPE_SERVICE_SEARCH_ATTR = 18,
	BTL_BMG_REQUEST_TYPE_INTERCEPT_HCI_INIT_EVENTS = 19,
	BTL_BMG_REQUEST_TYPE_RADIO_OFF = 20,
	BTL_BMG_REQUEST_TYPE_SET_ACCESSIBILITY = 21,
	BTL_BMG_REQUEST_TYPE_SERVICE_SEARCH = 22,
	BTL_BMG_REQUEST_TYPE_SERVICE_ATTR = 23
} BtlBmgRequestType;

/*
	Type of handler in a dynamic request (the union tag)
*/
typedef enum _BtlBmgRequestHandlerType
{
	BTL_BMG_REQUEST_HANDLER_TYPE_ME_TOKEN,
	BTL_BMG_REQUEST_HANDLER_TYPE_BT_HANDLER,
	BTL_BMG_REQUEST_HANDLER_TYPE_INVALID
} BtlBmgRequestHandlerType;

/* Virtual Function #0 - Destructor */
typedef void (*BtlBmgRequestDestroyVirtFunc)(BtlBmgRequest *request);

/* Virtual Function #1  - Clean */
typedef void (*BtlBmgRequestCleanVirtFunc)(BtlBmgRequest *request, BtlDefsMemReleaseFunc releaseFunc);

/* Virtual Function #2  - IsMatching */
typedef BOOL (*BtlBmgRequestIsMatchingVirtFunc)(BtlBmgRequest *thisRequest, BtlBmgRequest *otherRequest);

typedef struct _BtlBmgRequestVirtTable
{
	BtlBmgRequestDestroyVirtFunc		destroyFunc;
	BtlBmgRequestCleanVirtFunc			cleanFunc;
	BtlBmgRequestIsMatchingVirtFunc		isMatchingFunc;
} BtlBmgRequestVirtTable;

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/* [@ToDo][Udi] - Space could be saved by placing bmgContext + parent in a union */

#ifndef BTL_BMG_REQUEST_STRUCT
#define BTL_BMG_REQUEST_STRUCT
typedef struct _BtlBmgRequest 				BtlBmgRequest;
#endif

typedef struct _BtlBmgCompoundRequest 	BtlBmgCompoundRequest;

/*
	Simple Request Data Members
*/
struct _BtlBmgRequest
{
	ListEntry					node;		/* For placing the request on a list  - must be the first field*/

	BtlBmgRequestVirtTable		vptr;		/* virtual pointer */
	
	BtlBmgRequestType		requestType;
	BtlBmgCompoundRequest	*parent;		/* Points to the compound request containing this one */
	BtlBmgContext			*bmgContext;
	BtlBmgEvmCallBack		bmgCallback;
	BD_ADDR				bdAddress;
	void*					userData;	/* Allows additional data to be attached to the request */
};

/*
	Dynamic Request Data Members
*/
typedef struct _BtlBmgDynamicRequest
{
	BtlBmgRequest			base;

	/* Union Tag */
	BtlBmgRequestHandlerType	handlerType;

	/* The token */
	union
	{
		MeCommandToken		meCmdToken;
		BtHandler			btHandler;
	} token;		
} BtlBmgDynamicRequest;

/*
	Compund Request Data Members
*/
struct _BtlBmgCompoundRequest
{
	BtlBmgRequest			base;

	/* List of contained childrenrequests */
	ListEntry					childRequests;
};

/*
	Simple Request "Methods"
*/
BtStatus BTL_BMG_REQUEST_Create(	BtlBmgRequest			*request,
											BtlBmgRequestType		requestType,
											BtlBmgCompoundRequest	*parentRequest,
											BtlBmgContext			*bmgContext,
											BtlBmgEvmCallBack		bmgCallback,
											const BD_ADDR			*bdAddress);

void BTL_BMG_REQUEST_DESTROY(BtlBmgRequest *request);

void BTL_BMG_REQUEST_Clean(BtlBmgRequest *request, BtlDefsMemReleaseFunc releaseFunc);

BOOL BTL_BMG_REQUEST_IsMatching(BtlBmgRequest *thisRequest, BtlBmgRequest *otherRequest);

/*
	Dynamic Request "Methods"
*/
BtStatus BTL_BMG_DYNAMIC_REQUEST_Create(	
				BtlBmgDynamicRequest		*request,
				BtlBmgRequestType		requestType,
				BtlBmgCompoundRequest	*parentRequest,
				BtlBmgContext			*bmgContext,
				BtlBmgEvmCallBack		bmgCallback,
				const BD_ADDR			*bdAddress,
				BtlBmgRequestHandlerType	handlerType,
				BtCallBack				handlerCallback);

void BTL_BMG_DYNAMIC_REQUEST_Destroy(BtlBmgRequest *request);
void BTL_BMG_DYNAMIC_REQUEST_Clean(BtlBmgRequest *request, BtlDefsMemReleaseFunc releaseFunc);

/*
	Compound Request "Methods"
*/

BtStatus BTL_BMG_COMPOUND_REQUEST_Create(	
											BtlBmgCompoundRequest	*request,
											BtlBmgRequestType		requestType,
											BtlBmgCompoundRequest	*parentRequest,
											BtlBmgContext			*bmgContext,
											BtlBmgEvmCallBack		bmgCallback,
											const BD_ADDR			*bdAddress);

void BTL_BMG_COMPOUND_REQUEST_Destroy(BtlBmgRequest *request);
void BTL_BMG_COMPOUND_REQUEST_Clean(BtlBmgRequest *request, BtlDefsMemReleaseFunc releaseFunc);

/*
	Dispatchers for the virtual functions
*/
void BTL_BMG_REQUEST_DestroyVirtFunc(BtlBmgRequest *request);
void BTL_BMG_REQUEST_CleanVirtFunc(BtlBmgRequest *request, BtlDefsMemReleaseFunc releaseFunc);
BOOL BTL_BMG_REQUEST_IsMatchingVirtFunc(BtlBmgRequest *thisRequest, BtlBmgRequest *otherRequest);

#endif /* __BTL_BMG_REQUEST_H */



