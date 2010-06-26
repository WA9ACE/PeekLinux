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
*   FILE NAME:      btl_bmg_debug.h
*
*   DESCRIPTION:    This file defines common macros that should be used for message logging, 
*					and exception checking, handling and reporting
*
*					In addition, it contains miscellaneous other related definitions.
*
*   AUTHOR:         Udi Ron
*
\*******************************************************************************/


#ifndef __BTL_BMG_DEBUG_H
#define __BTL_BMG_DEBUG_H

#include "bttypes.h"

const char *BTL_BMG_DEBUG_pBool(BOOL value);

const char *BTL_BMG_DEBUG_pRequestType(U32 type);
const char *BTL_BMG_DEBUG_pForwardingMode(U32 mode);
const char *BTL_BMG_DEBUG_pSdpRetrieveStatus(U32 status);

#endif /* __BTL_BMG_DEBUG_H */


