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
*   FILE NAME:      btl_commoni.h
*
*   DESCRIPTION:    This file contains internal common BTL definitions
*
*					These definitions are placed in an internal file since they should
*					be used only by internal BTL code.
*
*					Internal BTL files should include this file rather than btl_common.h
*
*   AUTHOR:         Udi Ron
*
\*******************************************************************************/


#ifndef __BTL_COMMONI_H
#define __BTL_COMMONI_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "bttypes.h"
#include "btl_common.h"
#include "sec.h"

/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/
#define BTL_DEFAULT_APP_NAME		"DefaultApp"

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlModuleType type
 *
 *     Defines the type of the module.
 */
typedef U32 BtlModuleType;

/*-------------------------------------------------------------------------------*/
/* IMPROTANT: WHENEVER ADDING A MODULE - Update BTL_MODULE_TYPE_NUM_OF_MODULES */
/*-------------------------------------------------------------------------------*/

#define BTL_MODULE_TYPE_BMG		    (0)
#define BTL_MODULE_TYPE_SPP		    (1)
#define BTL_MODULE_TYPE_OPPC		(2)
#define BTL_MODULE_TYPE_OPPS		(3)
#define BTL_MODULE_TYPE_BPPSND	    (4)
#define BTL_MODULE_TYPE_PBAPS		(5)
#define BTL_MODULE_TYPE_PAN		    (6)
#define BTL_MODULE_TYPE_AVRCPTG	    (7)
#define BTL_MODULE_TYPE_FTPS		(8)
#define BTL_MODULE_TYPE_FTPC		(9)
#define BTL_MODULE_TYPE_VG			(10)
#define BTL_MODULE_TYPE_AG			(11)
#define BTL_MODULE_TYPE_RFCOMM		(12)
#define BTL_MODULE_TYPE_A2DP	(13)
#define BTL_MODULE_TYPE_HID		(14)
#define BTL_MODULE_TYPE_MDG 		(15)
#define BTL_MODULE_TYPE_BIPINT	(16)
#define BTL_MODULE_TYPE_BIPRSP	(17)
#define BTL_MODULE_TYPE_SAPS	(18)
#define BTL_MODULE_TYPE_MAIN	(19)
#define BTL_MODULE_TYPE_BSC 	(20)
#define BTL_MODULE_TYPE_L2CAP	(21)
#define BTL_MODULE_TYPE_L2CAP_THROUGHPUT	(22)
#define BTL_MODULE_TYPE_FMRADIO    (23)
#define BTL_MODULE_TYPE_NUM_OF_MODULES	(BTL_MODULE_TYPE_FMRADIO + 1)


/*---------------------------------------------------------------------------
 * BtlModuleNotificationType type
 *
 */
typedef U8 BtlModuleNotificationType;

#define BTL_MODULE_NOTIFICATION_RADIO_ON		(1)
#define BTL_MODULE_NOTIFICATION_RADIO_OFF		(2)

/*---------------------------------------------------------------------------
 * BtlModuleNotificationType type
 *
 */
typedef U8 BtlModuleNotificationCompletionEventType;

#define BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_ON		(1)
#define BTL_MODULE_NOTIFICATION_COMPLETION_EVENT_TYPE_RADIO_OFF		(2)

typedef struct _BtlModuleNotificationCompletionEvent	BtlModuleNotificationCompletionEvent;

/*-------------------------------------------------------------------------------
 * BtlCommonNotificationsCallback type
 *
 *     Per-module handler of common BTL notifications (e.g., Radio On / Off)
 */
typedef BtStatus (*BtlCommonNotificationsCallback)(BtlModuleNotificationType notificationType);

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlContext structure
 *
 *     	Represents a "base" BTL context, which is associated with one registered callback.
 *		This structure should be the first member of every specific module context
 */
typedef struct _BtlContext 
{
	/* allows the context to be placed on a list */

	/****************************/
	/* IMPORTANT: Must be first field */
	/****************************/
	ListEntry						node;

	/* The type of the module */
	BtlModuleType					moduleType;
	
	/* appHandle with which this context is associated (can be 0) */	
	BtlAppHandle					*appHandle;	
} BtlContext;

/*---------------------------------------------------------------------------
 * _BtlModuleNotificationCompletionEvent structure
 * 
 */
struct _BtlModuleNotificationCompletionEvent
{
	BtlModuleNotificationCompletionEventType		type;
	BtStatus									status;
};

/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/


BOOL BTL_IsInitialized();

BOOL BTL_IsOperational();

BOOL BTL_IsChipOn();

/*-------------------------------------------------------------------------------
 * BTL_HandleModuleInstanceCreation()
 *
 *		Performs common operations that are needed whenever a new module instance
 *		is created.
 *
 *		The following operations are performed:
 *		1. The instance's base context is initialized
 *		2. The context is added to its App Handle's contexts list
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		appHandle [in / out] - App Handle of the applicatin that creates the instance (may be 0)
 *
 *		moduleType [in] - Type of the created instance
 *
 *		context [out] - Context of the created instance
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation succeeded.
 *
 *     	BT_STATUS_NO_RESOURCES - operation failed because the max number of
 *		contexts per appliction was reached
 *
 *		BT_STATUS_INTERNAL_ERROR - Some serious internal error occurred, BTL must be reset.
 */
BtStatus BTL_HandleModuleInstanceCreation(	BtlAppHandle 		*appHandle, 
													BtlModuleType		moduleType, 
													BtlContext 		*context);

/*-------------------------------------------------------------------------------
 * BTL_HandleModuleInstanceDestruction()
 *
 *		Performs common operations that are needed whenever a new module instance
 *		is destroyed.
 *
 *		The following operations are performed:
 *		1. The context is removed from its App Handle's contexts list
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		context [in / out] - Context of the destroyed instance
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation succeeded.
 *
 *		BT_STATUS_INTERNAL_ERROR - Some serious internal error occurred, BTL must be reset.
 */
BtStatus BTL_HandleModuleInstanceDestruction(	BtlContext *context);

/*-------------------------------------------------------------------------------
 * BTL_VerifySecurityLevel()
 *
 *     	Verify the validity of the given security level.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		securityLevel [in] - level of security which should be verified.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Given security level is valid.
 *
 *		BT_STATUS_INVALID_PARM - Given security level is invalid.
 */
BtStatus BTL_VerifySecurityLevel(BtSecurityLevel securityLevel);

void BTL_RegisterForCommonNotifications(BtlModuleType moduleType, BtlCommonNotificationsCallback callback);
void BTL_DeRegisterFromCommonNotifications(BtlModuleType moduleType);
void BTL_ModuleCompleted(BtlModuleType moduleType, const BtlModuleNotificationCompletionEvent *event);

#endif /* __BTL_COMMONI_H */


