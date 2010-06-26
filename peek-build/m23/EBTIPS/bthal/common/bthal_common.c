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
*   FILE NAME:      bthal_common.c
*
*   DESCRIPTION:    Implementation of BTHAL common functions.
*
*   AUTHOR:         V. Abram
*
\*******************************************************************************/


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "bthal_types.h"
#include "bthal_common.h"


/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/
/*-------------------------------------------------------------------------------
 * BTHAL_StatusName()
 *
 *		Gets status' name.
 */
const char *BTHAL_StatusName(int status)
{
    switch (status)
    {
        case BTHAL_STATUS_SUCCESS:
            return "BTHAL_STATUS_SUCCESS";

        case BTHAL_STATUS_FAILED:
            return "BTHAL_STATUS_FAILED";

        case BTHAL_STATUS_PENDING:
            return "BTHAL_STATUS_PENDING";

        case BTHAL_STATUS_BUSY:
            return "BTHAL_STATUS_BUSY";

        case BTHAL_STATUS_NO_RESOURCES:
            return "BTHAL_STATUS_NO_RESOURCES";

        case BTHAL_STATUS_TIMEOUT:
            return "BTHAL_STATUS_TIMEOUT";

        case BTHAL_STATUS_INVALID_PARM:
            return "BTHAL_STATUS_INVALID_PARM";

        case BTHAL_STATUS_NOT_SUPPORTED:
            return "BTHAL_STATUS_NOT_SUPPORTED";

        case BTHAL_STATUS_IN_USE:
            return "BTHAL_STATUS_IN_USE";

        case BTHAL_STATUS_NOT_FOUND:
            return "BTHAL_STATUS_NOT_FOUND";

        default:
            return "Unknown BTHAL status";
    }
}
