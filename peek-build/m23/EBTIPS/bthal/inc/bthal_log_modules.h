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
*   FILE NAME:      bthal_log_modules.h
*
*   BRIEF:          This file defines the module types used in BTHAL log.
*
*   DESCRIPTION:    This file defines the module types which can send trace 
*					messages via the MACROs defined in bthal_log.h file.
*
*   AUTHOR:         Keren Ferdman 
*
\*******************************************************************************/


#ifndef __BTHAL_LOG_MODULES_H
#define __BTHAL_LOG_MODULES_H


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/


/*---------------------------------------------------------------------------
 * BthalLogModuleType type
 *
 *	   Represents different modules, which can output traces.
 */
typedef BTHAL_U8 BthalLogModuleType;

#define BTHAL_LOG_MODULE_TYPE_BMG			(0)
#define BTHAL_LOG_MODULE_TYPE_SPP			(1)
#define BTHAL_LOG_MODULE_TYPE_OPPC			(2)
#define BTHAL_LOG_MODULE_TYPE_OPPS			(3)
#define BTHAL_LOG_MODULE_TYPE_BPPSND		(4)
#define BTHAL_LOG_MODULE_TYPE_PBAPS			(5)
#define BTHAL_LOG_MODULE_TYPE_PAN			(6)
#define BTHAL_LOG_MODULE_TYPE_AVRCPTG		(7)
#define BTHAL_LOG_MODULE_TYPE_FTPS			(8)
#define BTHAL_LOG_MODULE_TYPE_FTPC			(9)
#define BTHAL_LOG_MODULE_TYPE_VG			(10)
#define BTHAL_LOG_MODULE_TYPE_AG			(11)
#define BTHAL_LOG_MODULE_TYPE_RFCOMM		(12)
#define BTHAL_LOG_MODULE_TYPE_A2DP			(13)
#define BTHAL_LOG_MODULE_TYPE_HID			(14)
#define BTHAL_LOG_MODULE_TYPE_MDG			(15)
#define BTHAL_LOG_MODULE_TYPE_BIPINT		(16)
#define BTHAL_LOG_MODULE_TYPE_BIPRSP		(17)
#define BTHAL_LOG_MODULE_TYPE_SAPS			(18)
#define BTHAL_LOG_MODULE_TYPE_COMMON		(19)
#define BTHAL_LOG_MODULE_TYPE_L2CAP			(20)
#define BTHAL_LOG_MODULE_TYPE_OPP			(21)
#define BTHAL_LOG_MODULE_TYPE_FTP			(22)
#define BTHAL_LOG_MODULE_TYPE_PM			(23)
#define BTHAL_LOG_MODULE_TYPE_BTDRV			(24)
#define BTHAL_LOG_MODULE_TYPE_UNICODE		(25)
#define BTHAL_LOG_MODULE_TYPE_BSC			(26)
#define BTHAL_LOG_MODULE_TYPE_BTSTACK		(27)
#define BTHAL_LOG_MODULE_TYPE_FMSTACK		(28)
#define BTHAL_LOG_MODULE_TYPE_FS			(29)
#define BTHAL_LOG_MODULE_TYPE_MODEM			(30)
#define BTHAL_LOG_MODULE_TYPE_OS			(31)


#endif /* __BTHAL_LOG_MODULES_H */


