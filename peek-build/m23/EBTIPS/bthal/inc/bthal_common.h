/*******************************************************************************\
##                                                                           	*
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
##																				*
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
##																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      bthal_common.h
*
*   BRIEF:    		This file defines the common types, defines, and prototypes
*					for the BTHAL component.
*
*   DESCRIPTION:    General
*   
*					The file holds common types , defines and prototypes, 
*					used by the BTHAL layer.
*
*   AUTHOR:         V. Abram
*
\*******************************************************************************/

#ifndef __BTHAL_COMMON_H
#define __BTHAL_COMMON_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "bthal_config.h"

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BthalStatus type
 *
 *     Defines status of BTHAL operation.
 */
typedef unsigned char BthalStatus;

#define BTHAL_STATUS_SUCCESS           (0)
#define BTHAL_STATUS_FAILED            (1)
#define BTHAL_STATUS_PENDING           (2)
#define BTHAL_STATUS_BUSY              (3)
#define BTHAL_STATUS_NO_RESOURCES      (4)
#define BTHAL_STATUS_TIMEOUT           (8)
#define BTHAL_STATUS_INVALID_PARM      (10)
#define BTHAL_STATUS_NOT_FOUND         (13)
#define BTHAL_STATUS_NOT_SUPPORTED     (15)
#define BTHAL_STATUS_IN_USE            (16)
#define BTHAL_STATUS_NO_CONNECTION     (17)
#define BTHAL_STATUS_IMPROPER_STATE    (31)


typedef struct _BthalEvent BthalEvent;

/*-------------------------------------------------------------------------------
 * BthalCallBack type
 *
 *     A function of this type is called to indicate Common BTHAL events
 */
typedef void (*BthalCallBack)(const BthalEvent	*event);


/*---------------------------------------------------------------------------
 * BthalEventType type
 *
 *     All indications and confirmations are sent through a callback
 *     function. The event types are defined below.
 */
typedef BTHAL_U8 BthalEventType;

#define BTHAL_EVENT_INIT_COMPLETE			(1)
#define BTHAL_EVENT_DEINIT_COMPLETE		(2)

/*---------------------------------------------------------------------------
 * BthalErrorType type
 */
typedef BTHAL_U8 BthalErrorType;

#define BTHAL_ERROR_TYPE_NONE				(0)
#define BTHAL_ERROR_TYPE_UNSPECIFIED		(1)

/*---------------------------------------------------------------------------
 * BthalModuleType type
 */
typedef BTHAL_U8 BthalModuleType;

#define BTHAL_MODULE_TYPE_OS			(1)
#define BTHAL_MODULE_TYPE_PM			(2)
#define BTHAL_MODULE_TYPE_DRV			(3)
#define BTHAL_MODULE_TYPE_UART		(4)
#define BTHAL_MODULE_TYPE_UTILS		(5)
#define BTHAL_MODULE_TYPE_SPPOS		(6)

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*---------------------------------------------------------------------------
 * BthalEvent structure
 * 
 *     This structure is sent to the callback specified for BTHAL_Init. It contains
 *	BTHAL events and associated information elements.
 */
struct _BthalEvent
{
	BthalModuleType		module;
	BthalEventType		type;
	BthalErrorType		errCode;
};

/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTHAL_StatusName()
 *
 * Brief:  
 *      Get the status name.
 *
 * Description:
 *      The function returns the status string name.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		status - BTHAL status.
 *
 * Returns:
 *		char * - String with the status name
  */
const char *BTHAL_StatusName(int status);


#endif /* __BTHAL_COMMON_H */


