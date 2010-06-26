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
*   FILE NAME:      btl_log.h
*
*   DESCRIPTION:    This file defines common macros that should be used for message logging 
*
*   AUTHOR:         Keren Ferdman
*
\*******************************************************************************/

#ifndef __BTL_LOG_H
#define __BTL_LOG_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "btl_log_modules.h"
#include "bthal_log.h"
#include "btl_defs.h"


#ifdef EBTIPS_RELEASE
#define BTL_LOG_DEBUG(msg)
#define BTL_LOG_FUNCTION(msg) 	
#else
#define BTL_LOG_DEBUG(msg)		BTHAL_LOG_DEBUG(__FILE__, __LINE__, btlLogModuleType, msg)
#define BTL_LOG_FUNCTION(msg) 	BTHAL_LOG_FUNCTION(__FILE__, __LINE__, btlLogModuleType, msg)
#endif /* EBTIPS_RELEASE */

#define BTL_LOG_INFO(msg)		BTHAL_LOG_INFO(__FILE__, __LINE__, btlLogModuleType, msg)				
#define BTL_LOG_ERROR(msg)		BTHAL_LOG_ERROR(__FILE__, __LINE__, btlLogModuleType, msg)			
#define BTL_LOG_FATAL(msg)		BTHAL_LOG_FATAL(__FILE__, __LINE__, btlLogModuleType, msg)	

#endif /* __BTL_LOG_H */


