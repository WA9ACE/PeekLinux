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
*   DESCRIPTION:    XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
*
*   AUTHOR:         Udi Ron
*
\*******************************************************************************/


#ifndef __BTL_BMG_EVM_DEFS_H
#define __BTL_BMG_EVM_DEFS_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <bttypes.h>
#include <me.h>

/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/


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

#if 0
typedef enum _BtlBmgEvmForwardingMode
{
	BTL_BMG_EVM_EVENT_FORWARD_TO_ALL,
	BTL_BMG_EVM_EVENT_FORWARD_TO_ALL_BUT_ME,
	BTL_BMG_EVM_EVENT_FORWARD_TO_ME_ONLY
} BtlBmgEvmForwardingMode;
#endif

typedef BOOL (*BtlBmgEvmCallBack)(const BtEvent *event, BtlBmgRequest *request);


#endif /* __BTL_BMG_EVM_DEFS_H */



