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
*   FILE NAME:      btl_bmgi.h
*
*   DESCRIPTION:    This file contains internal BTL BMG definitions.
*
*					These definitions are placed in an internal file since they should
*					be used only by internal BTL code.
*
*					Internal BTL files should include this file rather than btl_bmg.h
*
*   AUTHOR:         Udi Ron
*
\*******************************************************************************/


#ifndef __BTL_BMGI_H
#define __BTL_BMGI_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "me.h"
#include "btl_commoni.h"
#include "btl_bmg.h"


/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/

/*
	Max number of concurrent requests of all types (top level + contained)
*/
#define BTL_BMG_MAX_NUM_OF_REQUESTS			(max(BTL_CONFIG_BMG_MAX_NUM_OF_RESPONSES_FOR_SEARCH_BY_COD,	\
																					BTL_CONFIG_BMG_MAX_NUM_OF_DEVICES_FOR_SEARCH_BY_DEVICES) * 2 + \
																					BTL_CONFIG_BMG_MAX_NUM_OF_REQUESTS_IN_PARALLEL_IN_ADDITION_TO_SEARCH)

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

#ifndef BTL_BMG_REQUEST_STRUCT
#define BTL_BMG_REQUEST_STRUCT
typedef struct _BtlBmgRequest BtlBmgRequest;
#endif



/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlBmgContext structure
 *
 *     	Represents BTL BMG context, which is associated with one registered callback.
 *		This context is used as the first agrument for all BMG functions.
 */
struct _BtlBmgContext 
{
	BtlContext					base;	/* Must be first field */

	BtlBmgCallBack 				bmgCallcack;
	BtlBmgEventForwardingMode		forwardingMode;	
};


/* 
	Checks if a request is a top-level request. 

	A request is a top-level request if it is not a sub-request of some other
	request
*/
BOOL BTL_BMG_IsRequestTopLevel(const BtlBmgRequest *request);

BtStatus BTL_BMG_SetHciInitEventsHandlingMode(BOOL blockEvents);
BtStatus BTL_BMG_GenerateHciInitEvent(BtEventType eventType);

#endif /* __BTL_BMGI_H */

