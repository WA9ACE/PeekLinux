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
*   FILE NAME:      bthal_log.h
*
*   BRIEF:          This file defines the API of the BTHAL log MACROs.
*
*   DESCRIPTION:    The BTHAL LOG API implements platform dependent logging
*                   MACROs which should be used for logging messages by 
*					different layers, such as the transport, stack, BTL and BTHAL.
*					The following 5 Macros mast be implemented according to the platform:
*					BTHAL_LOG_DEBUG
*					BTHAL_LOG_INFO
*					BTHAL_LOG_ERROR
*					BTHAL_LOG_FATAL
*					BTHAL_LOG_FUNCTION
*
*   AUTHOR:         Keren Ferdman 
*
\*******************************************************************************/


#ifndef __BTHAL_LOG_H
#define __BTHAL_LOG_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "bthal_log_modules.h"
#include "bthal_types.h"
#include "vsi.h"


/*-------------------------------------------------------------------------------
 * BTHAL_LOG_DEBUG
 *
 *     	Defines trace message in debug level, which should not be used in release build.
 *		This MACRO is not used when EBTIPS_RELEASE is enabled.
 */
#define BTHAL_LOG_DEBUG(file, line, moduleType, msg)		vsi_o_event_ttrace msg






/*-------------------------------------------------------------------------------
 * BTHAL_LOG_INFO
 *
 *     	Defines trace message in info level.
 */
#define BTHAL_LOG_INFO(file, line, moduleType, msg)			vsi_o_state_ttrace msg	





 
/*-------------------------------------------------------------------------------
 * BTHAL_LOG_ERROR
 *
 *     	Defines trace message in error level.
 */
#define BTHAL_LOG_ERROR(file, line, moduleType, msg)		vsi_o_error_ttrace msg	
 




 
/*-------------------------------------------------------------------------------
 * BTHAL_LOG_FATAL
 *
 *     	Defines trace message in fatal level.
 */
#define BTHAL_LOG_FATAL(file, line, moduleType, msg)		vsi_o_error_ttrace msg	






/*-------------------------------------------------------------------------------
 * BTHAL_LOG_FUNCTION
 * 
 *     	Defines trace message in function level, meaning it is used when entering
 *		and exiting a function.
 *		It should not be used in release build.
 *		This MACRO is not used when EBTIPS_RELEASE is enabled.
 */
#define BTHAL_LOG_FUNCTION(file, line, moduleType, msg)		vsi_o_func_ttrace msg	






#endif /* __BTHAL_LOG_H */


