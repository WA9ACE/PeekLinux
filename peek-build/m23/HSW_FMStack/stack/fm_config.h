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
*   FILE NAME:      fm_config.h
*
*   BRIEF:          FM Configuration Parameters
*
*   DESCRIPTION:
*
*     The constants in this file configure the FM for a specific platform and project.
*	To change a constant, simply change its value in this file and recompile the entire BTIPS package.
*
*     Some constants are numeric, and others indicate whether a feature
*     is enabled. 
*
*	The values in this specific file are tailored for a Windows distribution. To change a constant, 
*	simply change its value in this file and recompile the entire BTIPS package.
*
*   AUTHOR:   Yaniv Rabin
*
\******************************************************************************/

#ifndef __FM_CONFIG_H
#define __FM_CONFIG_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/


/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/


/*-------------------------------------------------------------------------------
 * Common
 *
 *     Represents common configuration parameters.
 */


/*-------------------------------------------------------------------------------
 * FMS
 *
 *     Represents configuration parameters for FMS module.
 */

/*
*	define the time from the moment the FM stack switches to the AF before it allows another switch. This is necessary to avoid
*	excessive changes between the primary and alternate frequencies.
*/
#define	FM_CONFIG_AF_TIMER_MS								(30000)

/*
*	Must wait at least 20msec before starting to send commands to the FM.
*/
#define	FM_CONFIG_WAKEUP_TIMEOUT_MS						(40)




#endif /* __FM_CONFIG_H */



