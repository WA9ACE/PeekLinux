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
*   FILE NAME:      btl_bmg_request.c
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

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <string.h>
#include <bttypes.h>
#include <utils.h>
#include <btl_bmgi.h>
#include <btl_bmg_request.h>
#include <btl_defs.h>

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BMG);

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/*
	Virtual Tables

	Every virtual table must contain pointers to 2 function, in the specified order:
	1. Destructor
	2. Cleanup
*/

static BtlBmgRequestVirtTable btlBmgRequestVtbl = 
{
	
	BTL_BMG_REQUEST_DESTROY,
	BTL_BMG_REQUEST_Clean,
	BTL_BMG_REQUEST_IsMatching
};

static BtlBmgRequestVirtTable btlBmgDynamicRequestVtbl = 
{
	
	BTL_BMG_DYNAMIC_REQUEST_Destroy,
	BTL_BMG_DYNAMIC_REQUEST_Clean,
	BTL_BMG_REQUEST_IsMatching
};

static BtlBmgRequestVirtTable btlBmgCompoundRequestVtbl = 
{
	
	BTL_BMG_COMPOUND_REQUEST_Destroy,
	BTL_BMG_COMPOUND_REQUEST_Clean,
	BTL_BMG_REQUEST_IsMatching
};

BtStatus BTL_BMG_REQUEST_Create(	BtlBmgRequest			*request,
											BtlBmgRequestType		requestType,
											BtlBmgCompoundRequest	*parentRequest,
											BtlBmgContext			*bmgContext,
											BtlBmgEvmCallBack		bmgCallback,
											const BD_ADDR			*bdAddress)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_REQUEST_Create");

	BTL_VERIFY_FATAL(0 != request, BT_STATUS_INTERNAL_ERROR, ("Null request"));

	InitializeListEntry(&(request->node));
	
	request->vptr = btlBmgRequestVtbl;

	request->requestType = requestType;
	request->parent = parentRequest;
	request->bmgContext = bmgContext;
	request->bmgCallback = bmgCallback;

	if (0 != bdAddress)
	{
		OS_MemCopy((U8*)&request->bdAddress, (const U8*)bdAddress, BD_ADDR_SIZE);
	}
	else
	{
		OS_MemSet((U8*)&request->bdAddress, 0, BD_ADDR_SIZE);
	}
	
	request ->userData = 0;
	
	BTL_FUNC_END();

	return status;
}

void BTL_BMG_REQUEST_DESTROY(BtlBmgRequest *request)
{
	BTL_FUNC_START("BTL_BMG_REQUEST_DESTROY");
	
	request->vptr = btlBmgRequestVtbl;

	BTL_VERIFY_ERR_NO_RETVAR(0 == request->userData, ("User Data is Not Null"));

	BTL_FUNC_END();
}

void BTL_BMG_REQUEST_Clean(BtlBmgRequest *request, BtlDefsMemReleaseFunc releaseFunc)
{
	BTL_FUNC_START("BTL_BMG_REQUEST_Clean");
	
	/* Call the destructor */
	BTL_BMG_REQUEST_DESTROY(request);

	/* Free the memory */
	releaseFunc((void **)(&request));

	BTL_FUNC_END();
}

BOOL BTL_BMG_REQUEST_IsMatching(BtlBmgRequest *thisRequest, BtlBmgRequest *otherRequest)
{
	BOOL	result = FALSE;

	BTL_FUNC_START("BTL_BMG_REQUEST_IsMatching");

	/* [@ToDo][Udi] The assumption is that the */

		
	if (	(thisRequest->requestType != otherRequest->requestType) ||
		(thisRequest->bmgContext != otherRequest->bmgContext) ||
		(FALSE  == AreBdAddrsEqual(&thisRequest->bdAddress, &otherRequest->bdAddress)))
	{
		result = FALSE;
	}
	else
	{
		result = TRUE;
	}
	
	BTL_FUNC_END();
	
	return result;
}

BtStatus BTL_BMG_DYNAMIC_REQUEST_Create(	
				BtlBmgDynamicRequest		*request,
				BtlBmgRequestType		requestType,
				BtlBmgCompoundRequest	*parentRequest,
				BtlBmgContext			*bmgContext,
				BtlBmgEvmCallBack		bmgCallback,
				const BD_ADDR			*bdAddress,
				BtlBmgRequestHandlerType	handlerType,
				BtCallBack				handlerCallback)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_DYNAMIC_REQUEST_Create");

	status = BTL_BMG_REQUEST_Create(	&request->base, 
										requestType, 
										parentRequest,
										bmgContext, 
										bmgCallback, 
										bdAddress);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), status, ("BTL_BMG_REQUEST_Create Failed"));

	/* Set vptr to my vtbl */
	request->base.vptr = btlBmgDynamicRequestVtbl;
	
	request->handlerType = handlerType;

	switch (handlerType)
	{
		case BTL_BMG_REQUEST_HANDLER_TYPE_ME_TOKEN:

			request->token.meCmdToken.callback = handlerCallback;
			
			break;
			
		case BTL_BMG_REQUEST_HANDLER_TYPE_BT_HANDLER:

			InitializeListEntry(&request->token.btHandler.node);
			request->token.btHandler.callback = handlerCallback;

			break;
			
		default:

			BTL_FATAL(BT_STATUS_INTERNAL_ERROR, ("Invalid handler Type (%d)", handlerType));
	};
	
	BTL_FUNC_END();

	return status;
}

void BTL_BMG_DYNAMIC_REQUEST_Destroy(BtlBmgRequest *request)
{
	BTL_FUNC_START("BTL_BMG_DYNAMIC_REQUEST_Destroy");

	/* Finished my (null) destruction, call the base's dtor */
	BTL_BMG_REQUEST_DESTROY(request);

	BTL_FUNC_END();
}

void BTL_BMG_DYNAMIC_REQUEST_Clean(BtlBmgRequest *request, BtlDefsMemReleaseFunc releaseFunc)
{
	BTL_FUNC_START("BTL_BMG_DYNAMIC_REQUEST_Clean");
	
	BTL_BMG_DYNAMIC_REQUEST_Destroy(request);

	releaseFunc((void **)(&request));

	BTL_FUNC_END();
}

BtStatus BTL_BMG_COMPOUND_REQUEST_Create(	
											BtlBmgCompoundRequest	*request,
											BtlBmgRequestType		requestType,
											BtlBmgCompoundRequest	*parentRequest,
											BtlBmgContext			*bmgContext,
											BtlBmgEvmCallBack		bmgCallback,
											const BD_ADDR			*bdAddress)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_COMPOUND_REQUEST_Create");
	
	status = BTL_BMG_REQUEST_Create(	&request->base, 
										requestType, 
										parentRequest,
										bmgContext, 
										bmgCallback, 
										bdAddress);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, ("BTL_BMG_REQUEST_Create Failed"));

	request->base.vptr = btlBmgCompoundRequestVtbl;
	
	InitializeListHead(&(request->childRequests));
	
	BTL_FUNC_END();

	return status;
}

void BTL_BMG_COMPOUND_REQUEST_Destroy(BtlBmgRequest *request)
{
	BtlBmgCompoundRequest	*specificRequest = (BtlBmgCompoundRequest*)request;
	
	BTL_FUNC_START("BTL_BMG_COMPOUND_REQUEST_Destroy");

	BTL_VERIFY_FATAL_NO_RETVAR(	0 != IsListEmpty(&specificRequest->childRequests), 
									("There are still child requests"));
	
	/* Finished my (null) destruction, call the base's dtor */
	BTL_BMG_REQUEST_DESTROY(request);

	BTL_FUNC_END();
}

void BTL_BMG_COMPOUND_REQUEST_Clean(BtlBmgRequest *request, BtlDefsMemReleaseFunc releaseFunc)
{
	BtlBmgCompoundRequest	*specificRequest = (BtlBmgCompoundRequest*)request;
	BtlBmgRequest			*cur = 0;
	BtlBmgRequest			*next = 0;

	BTL_FUNC_START("BTL_BMG_COMPOUND_REQUEST_Clean");

	/* Iterate over all children, cleaning them recursively */
	IterateListSafe(specificRequest->childRequests, cur, next, BtlBmgRequest*) 			
	{
		RemoveEntryList(&(cur->node));
		
		BTL_BMG_REQUEST_CleanVirtFunc(cur, releaseFunc);
	}

	/* Now clean myself */
	
	BTL_BMG_COMPOUND_REQUEST_Destroy(request);

	releaseFunc((void **)(&request));

	 BTL_FUNC_END();
}

/* 
	Virtual Functions Dispatcher
*/

void BTL_BMG_REQUEST_DestroyVirtFunc(BtlBmgRequest *request)
{
	(request->vptr.destroyFunc)(request);
}

void BTL_BMG_REQUEST_CleanVirtFunc(BtlBmgRequest *request, BtlDefsMemReleaseFunc releaseFunc)
{
	(request->vptr.cleanFunc)(request, releaseFunc);
}

BOOL BTL_BMG_REQUEST_IsMatchingVirtFunc(BtlBmgRequest *thisRequest, BtlBmgRequest *otherRequest)
{
	return (thisRequest->vptr.isMatchingFunc)(thisRequest, otherRequest);
}

