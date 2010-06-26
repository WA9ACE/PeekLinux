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
*******************************************************************************/
/*******************************************************************************
*
*   FILE NAME:      bthal_config.h
*
*   BRIEF:          BTIPS Hardware Adaptation Layer Configuration Parameters
*
*   DESCRIPTION:
*
*     The constants in this file configure the BTHAL layer for a specific platform and project.
*
*     Some constants are numeric, and others indicate whether a feature
*     is enabled (defined as BTL_CONFIG_ENABLED) or disabled (defined as
*     BTL_CONFIG_DISABLED).

*	The values in this specific file are tailored for a Windows distribution. To change a constant,
*	simply change its value in this file and recompile the entire BTIPS package.
*
*   AUTHOR:         Yaniv Rabin
*
*****************************************************************************/

#ifndef __BTHAL_CONFIG_H
#define __BTHAL_CONFIG_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <stdlib.h>
#include "bthal_types.h"
#include "btl_config.h"
#include "EBTIPS_version.h"

/* Comment this line for embedded Neptune & Locosto platforms */
/* #define WIN_WARNING_PRAGMAS */
#include "bthal_pragmas.h"

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

#define BTHAL_PLATFORM                          PLATFORM_LOCOSTO


#define BTHAL_REPORT_STRING_MAX_LENGTH	        20


/*-------------------------------------------------------------------------------
 * FS
 *
 *     Represents configuration parameters for FS module.
 */

/*
*	Specific the path of TI init script file name on the OS system.
*	The init script locate on "\EBTIPS\bthal\init script\TIInit_X.X.XX.bts" in the delivery packet.
*/
#define	BTHAL_FS_TI_INIT_SCRIPT_PATH								"/BTInitScript/"

/*
*	Specifies the FS path of a temporary image file that is created when BIP Responder receives
*	images from a BIP responder.
*/
#define	BTHAL_FS_BIPRSP_TEMP_PUSHED_IMAGE_PATH				"/bip/TempPushedImage.jpg"

/*
	The FFS path in which the Device Database will be saved.
	The Device Database is used by the Management Entity and by the applicaiton to store link
    keys and other information about peer devices in a non-volatile way.
*/
#define	BTHAL_FS_DDB_FILE_NAME									"/BtDeviceDb.ddb"

/*
*	The maximum length of a file system path
*/
#define BTHAL_FS_MAX_PATH_LENGTH									256

/*
*	The maximum name of the file name part on the local file system
*/
#define BTHAL_FS_MAX_FILE_NAME_LENGTH							128

/*
*	The folder separator character of files on the file system
*/
#define BTHAL_FS_PATH_DELIMITER									'/'

/*
*	Status Mask
*/
#define BTHAL_FS_STAT_MASK										(BTHAL_FS_S_USERPERM | BTHAL_FS_S_SIZE)

/*
*	define if the file system is case sensitive
*/
#define	BTHAL_FS_CASE_SENSITIVE									(FALSE)

/*-------------------------------------------------------------------------------
 * OS
 *
 *     Represents configuration parameters for OS module.
 */

/*
*	The maximum number of A2DP-Specific events
*
*	Currently, the configuration accomodates a total of 7:
* 	1. process event
*	2. timer event
*	3. BSC
*	4. phonebook task (PBAP profile)
*	5. MDG event
*	6. Init
*	7. Radio Off
*/
#define BTHAL_OS_MAX_NUM_OF_EVENTS_STACK						(7)

/*
*	The maximum number of Bluetooth transport-related events.
*
*	Currently the configuration accomodates 1 Rx event and 1 Tx event
*/
#define BTHAL_OS_MAX_NUM_OF_EVENTS_TRANSPORT					(2)

/*
*	The maximum number of A2DP-Specific events
*
*	Currently, the configuration accomodates a total of 4:
*	1. 1 data ind event
*	2. 1 data sent event
*	3. 1 config ind event
*	4. 1 timer event
*/
#define BTHAL_OS_MAX_NUM_OF_EVENTS_A2DP							(4)


/*-------------------------------------------------------------------------------
 * 	BTHAL_A2DP_MM_PCM_SUPPORT
 *
 *     	If enabled it means the MM has support for streaming PCM to BT
 */
#define BTHAL_A2DP_MM_PCM_SUPPORT															(BTL_CONFIG_ENABLED)

/*
*	The maximum number of FM-Specific Events
*
*	1 fm process event + 1 timer event
*/
#if (0)
#define BTHAL_OS_MAX_NUM_OF_EVENTS_FM							(2)
#endif

/*
 *	The maximum number of tasks: BTS, BTU, BTT, FMS, BTAV.
 */
#if (0)
#define BTHAL_OS_MAX_NUM_OF_TASKS							    (5)
#else
#define BTHAL_OS_MAX_NUM_OF_TASKS							    (4)
#endif

/* 1 (stack) + 1 (BSC) + 2 (A2DP) + 1 (MDG) + 1 (Chip Manager) + 1 (FM) +
 * 1 (BTHAL_MC) semaphores */
#if (0)
#define BTHAL_OS_MAX_NUM_OF_SEMAPHORES							(8)
#else
#define BTHAL_OS_MAX_NUM_OF_SEMAPHORES							(7)
#endif

/*
*	The maximum number of OS timers used by BTIPS.
*
*	Currently, the configuration accomodates 1 stack timer + 1 A2DP timer + 1 FM Timer
*	+ 1 VG Ring/CLIP timer
*/
#if (0)
#define BTHAL_OS_MAX_NUM_OF_TIMERS								(4)
#else
#define BTHAL_OS_MAX_NUM_OF_TIMERS								(3)
#endif


/*-------------------------------------------------------------------------------
 * MD
 *
 *     Represents configuration parameters for the MD module.
 */

/*
 * 	A type of buffer owner for data downloaded from the network:
 *
 * 	Case 0: modem has not its own buffers and data is read into caller's buffer.
 *	Case 1: modem supplies downloaded data with its buffer, which should be freed later
 */
#define BTHAL_MD_DOWNLOAD_BUF_OWNER_MODEM					(BTL_CONFIG_ENABLED)

/*
 *	A type of buffer owner for data to be uploaded to the network:
 *
 * 	Case 0: modem does not expose its own buffers and data is written to modem by
 *			means of MD function from application's buffer which should be returned later
 *			with event BTHAL_MD_EVENT_UPLOAD_BUF_FROM_MODEM.
 *
 * 	Case 1: modem supplies its buffer to be filled with data to be uploaded to
 *			the network using event BTHAL_MD_EVENT_UPLOAD_BUF_FROM_MODEM
 *			later
 */
#define BTHAL_MD_UPLOAD_BUF_OWNER_MODEM						(BTL_CONFIG_ENABLED)


/*-------------------------------------------------------------------------------
 * PB
 *
 *     Represents configuration parameters for PB module.
 */

/*
 *     Defines the maximum number of phonebook entries in a single phonebook.
 */
#define BTHAL_PB_MAX_ENTRIES_NUM  		 (300)

/*
 *     Defines the maximum phonebook entry length that is requested in the
*	BTHAL_PB_GetEntryData function.
 */
#define BTHAL_PB_MAX_ENTRY_LEN  		 (500)

/*
 *     Maximum number of btyes allowed for entry name + 1 (null-
 *     terminating character). Please note that the entry name may be
 *	   in unicode, where a character may be represeted by more than
 *	   one byte.
 *
 */
#define BTHAL_PB_MAX_ENTRY_NAME           (32)


/********************************************************************************
 *
 * platform dependent timing macros
 *
 *******************************************************************************/

/*
*	Macro that converts OS ticks to milliseconds
*/
#define	BTHAL_OS_TICKS_TO_MS(ticks) 	(ticks)

/*
*	Macro that converts milliseconds to OS ticks
*/
#define	BTHAL_OS_MS_TO_TICKS(ms) 	(ms)


#endif /* __BTHAL_CONFIG_H */


