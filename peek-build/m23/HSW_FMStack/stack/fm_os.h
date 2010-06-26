/*******************************************************************************\
##                                                                             *
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION          *
##                                                                             *
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE        *
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE      *
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO      *
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT       *
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL        *
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC          *
##                                                                             *
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      fm_os.h
*
*   BRIEF:          This file defines the API of the FM OS.
*
*   DESCRIPTION:    General
*
*                   FM OS API layer.
*                   
*   AUTHOR:         Keren Gazit
*
\*******************************************************************************/

#ifndef __FMS_OS_H
#define __FMS_OS_H

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "fm_types.h"
#include "fm_utils.h"
#include "bthal_os.h"

/* FM Stack event */
#define OS_EVENT_FM_STACK_TASK_PROCESS						(0x00000001)
#define OS_EVENT_FM_TIMER_EXPIRED								(0x00000002)

extern BthalOsSemaphoreHandle  fmStackMutexHandle;	/* Mutex for locking the FM stack */ 

/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * FM_OsInit()
 *
 * Brief:  
 *	    Initializes the FM operating system layer.
 *
 * Description:
 *	    Initializes the FM operating system layer.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void
 *
 * Returns:
 *		TIFM_TRUE - The FM OS initialized successfully.
 *
 *		TIFM_FALSE - The FM OS initialization failed.
 */
TIFM_BOOL FM_OsInit(void);

/*-------------------------------------------------------------------------------
 * FM_OsDeinit()
 *
 * Brief:  
 *	    DeInitializes the FM operating system layer.
 *
 * Description:
 *	    DeInitializes the FM operating system layer.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void
 *
 * Returns:
 *		TIFM_TRUE - The FM OS deinitialized successfully.
 *
 *		TIFM_FALSE - The FM OS deinitializaztion failed.
 */
TIFM_BOOL FM_OsDeinit(void);

/*-------------------------------------------------------------------------------
 * OS_LockFmStack()
 *
 * Brief:  
 *	    Locks the FM semaphore.
 *
 * Description:
 *	    Locks the FM semaphore.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void
 *
 * Returns:
 *		SUCCESS - successfully locked the FM semaphore.
 *
 *		FAILED - Lock the FM semaphore failed.
 */
void OS_LockFmStack(void);
#define OS_LockFmStack() BTHAL_OS_LockSemaphore(fmStackMutexHandle, 0)

/*-------------------------------------------------------------------------------
 * OS_UnlockFmStack()
 *
 * Brief:  
 *	    Unlock the FM semaphore.
 *
 * Description:
 *	    Unlock the FM semaphore.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void
 *
 * Returns:
 *		SUCCESS - successfully locked the FM semaphore.
 *
 *		FAILED - Lock the FM semaphore failed.
 */
void OS_UnlockFmStack(void);
#define OS_UnlockFmStack()  BTHAL_OS_UnlockSemaphore(fmStackMutexHandle)

/*-------------------------------------------------------------------------------
 * NotifyFms()
 *
 * Brief:  
 *	    Send OS event to the FM task.
 *
 * Description:
 *	    Send OS event to the FM task.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void
 *
 * Returns:
 *		SUCCESS - Sending the event successfully.
 *
 */
void NotifyFms(void);

#endif	/* __FMS_OS_H */
