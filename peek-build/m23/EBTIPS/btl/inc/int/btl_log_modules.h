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
*   FILE NAME:      btl_log_modules.h
*
*   DESCRIPTION:    This file defines module types for logging purposes
*
*   AUTHOR:         Udi Ron
*
\*******************************************************************************/


#ifndef __BTL_LOG_MODULES_H
#define __BTL_LOG_MODULES_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "xatypes.h"
#include "bthal_log_modules.h"

typedef U8	BtlLogModuleType;

/*
	IMPORTANT NOTE: WHENEVER ADDDING A NEW MODULE TYPE DO THE FOLLOWING:
	1. Update BTL_LOG_MODULE_TYPE_NUM_OF_TYPES below
	2. Add its name to btlLogBtlModuleNames (in btl_defs.c)
*/
#define BTL_LOG_MODULE_TYPE_BMG					(BTHAL_LOG_MODULE_TYPE_BMG)
#define BTL_LOG_MODULE_TYPE_SPP					(BTHAL_LOG_MODULE_TYPE_SPP)
#define BTL_LOG_MODULE_TYPE_OPPC				(BTHAL_LOG_MODULE_TYPE_OPPC)
#define BTL_LOG_MODULE_TYPE_OPPS				(BTHAL_LOG_MODULE_TYPE_OPPS)
#define BTL_LOG_MODULE_TYPE_BPPSND				(BTHAL_LOG_MODULE_TYPE_BPPSND)
#define BTL_LOG_MODULE_TYPE_PBAPS				(BTHAL_LOG_MODULE_TYPE_PBAPS)
#define BTL_LOG_MODULE_TYPE_PAN					(BTHAL_LOG_MODULE_TYPE_PAN)
#define BTL_LOG_MODULE_TYPE_AVRCPTG				(BTHAL_LOG_MODULE_TYPE_AVRCPTG)
#define BTL_LOG_MODULE_TYPE_FTPS				(BTHAL_LOG_MODULE_TYPE_FTPS)
#define BTL_LOG_MODULE_TYPE_FTPC				(BTHAL_LOG_MODULE_TYPE_FTPC)
#define BTL_LOG_MODULE_TYPE_VG					(BTHAL_LOG_MODULE_TYPE_VG)
#define BTL_LOG_MODULE_TYPE_AG					(BTHAL_LOG_MODULE_TYPE_AG)
#define BTL_LOG_MODULE_TYPE_RFCOMM				(BTHAL_LOG_MODULE_TYPE_RFCOMM)
#define BTL_LOG_MODULE_TYPE_A2DP 				(BTHAL_LOG_MODULE_TYPE_A2DP)
#define BTL_LOG_MODULE_TYPE_HID					(BTHAL_LOG_MODULE_TYPE_HID)
#define BTL_LOG_MODULE_TYPE_MDG				    (BTHAL_LOG_MODULE_TYPE_MDG)
#define BTL_LOG_MODULE_TYPE_BIPINT				(BTHAL_LOG_MODULE_TYPE_BIPINT)
#define BTL_LOG_MODULE_TYPE_BIPRSP          	(BTHAL_LOG_MODULE_TYPE_BIPRSP)
#define BTL_LOG_MODULE_TYPE_SAPS            	(BTHAL_LOG_MODULE_TYPE_SAPS)
#define BTL_LOG_MODULE_TYPE_BTL_COMMON  		(BTHAL_LOG_MODULE_TYPE_COMMON)
#define BTL_LOG_MODULE_TYPE_L2CAP				(BTHAL_LOG_MODULE_TYPE_L2CAP)
#define BTL_LOG_MODULE_TYPE_OPP	  				(BTHAL_LOG_MODULE_TYPE_OPP)
#define BTL_LOG_MODULE_TYPE_FTP	  				(BTHAL_LOG_MODULE_TYPE_FTP)
#define BTL_LOG_MODULE_TYPE_HAL_PM				(BTHAL_LOG_MODULE_TYPE_PM)
#define BTL_LOG_MODULE_TYPE_HAL_BTDRV			(BTHAL_LOG_MODULE_TYPE_BTDRV)
#define BTL_LOG_MODULE_TYPE_UNICODE				(BTHAL_LOG_MODULE_TYPE_UNICODE)
#define BTL_LOG_MODULE_TYPE_BSC  				(BTHAL_LOG_MODULE_TYPE_BSC)

#define BTL_LOG_MODULE_TYPE_NUM_OF_TYPES	(BTL_LOG_MODULE_TYPE_BSC + 1)

#endif /* __BTL_LOG_MODULES_H */


