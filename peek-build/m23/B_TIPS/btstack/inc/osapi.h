#ifndef __OSAPI_H
#define __OSAPI_H

/****************************************************************************
 *
 * File:
 *     $Workfile:osapi.h$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision:66$
 *
 * Description:
 *     Defines the APIs used by the stack to access system services.
 * 
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright 2005-2006 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions,
 * Inc.  This work contains confidential and proprietary information of
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "xatypes.h"
#include "bthal_config.h"
#include "bthal_utils.h"
#include "bthal_os.h"
#include "bthal_log.h"
#include "btl_unicode.h"
#include "bthal_log_modules.h"

 /*---------------------------------------------------------------------------
 * Operating System API layer
 *
 *     The OS API is a set of functions used by the stack to access
 *     system services. If you are porting to a new operating system,
 *     you must provide the functions described here.
 *
 *     The functions in this API are intended for exclusive use by the
 *     stack. Applications should use the native operating system interface
 *     and avoid making OS_ calls themselves.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/* The following events can be received by the stack task */

/* Process event is sent to the stack task */
#define OS_EVENT_STACK_TASK_PROCESS						(0x00000001)

/* Timer event is sent to the stack task */
#define OS_EVENT_STACK_TASK_TIMER						(0x00000002)

/* Phonebook event is sent to the stack task by the phonebook 
 * task, when it is time to perform context switch back to 
 * the stack task */
#define OS_EVENT_STACK_PB_TASK 							(0x00000004)

/* BTIPS State Management events (Init / Deinit / Radio On / off) */
#define OS_EVENT_STACK_TASK_BTL_STATE_CONTROL			(0x00000008)

/* Notification event for the BTL BSC module */
#define OS_EVENT_STACK_TASK_BSC			                (0x00000010)

/* Notification event for the BTL MDG module */
#define OS_EVENT_STACK_TASK_MDG						    (0x00000020)

#if BTL_CONFIG_VG == BTL_CONFIG_ENABLED
/* Notification event for the BTL VG module */
#define OS_EVENT_STACK_TASK_VG							(0x00000040)
#endif

extern BthalOsSemaphoreHandle  stackMutexHandle;	/* Mutex for locking stack */ 


/*---------------------------------------------------------------------------
 * TimeT type
 *
 *     Indicates an amount of time in system ticks or milliseconds. This must
 *     be a 32-bit value because some timeouts may exceed 5 minutes.
 */ 
typedef U32 TimeT;

/* End of TimeT type */


#if XA_MULTITASKING == XA_ENABLED

/*---------------------------------------------------------------------------
 * OsTimerNotify type
 *
 *     The type for a timer notification callback. The notification function
 *     is provided by the stack using OS_StartTimer, and is called
 *     by the OS API to indicate that the timer has fired.
 *
 *     This function may be called from interrupt mode or task mode as
 *     required by the operating system.
 *
 * Requires:
 *     XA_MULTITASKING enabled.
 */ 
typedef void (*OsTimerNotify)(void);

#endif /* XA_MULTITASKING */

/* End of OsTimerNotify type */


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * OS_Init()
 *
 *     Initializes the stack and operating system layer. This function is
 *     not called by the stack, but must be called by your operating system
 *     or application to set up the stack and its operating system services.
 * 
 *     The OS_Init function must perform the following tasks in this order:
 * 
 *     1) Seed the random number generator with a random value such as
 *     the current system time.
 * 
 *     2) Any other operating-system specific functions, such as timer
 *     initialization or the creation of semaphores.
 * 
 *     3) Call EVM_Init to initialize the Event Manager. This also
 *     initializes all stack layers. Note that this function may
 *     return FALSE if stack initialization failed for some reason.
 * 
 *     4) Create and start the stack task. This task calls EVM_Process
 *     at least once for every call to OS_NotifyEvm. See OS_NotifyEvm
 *     for more details.
 * 
 * Returns:
 *     TRUE - Success.
 *
 *     FALSE - Initialization failed for some reason.
 */
BOOL OS_Init(void);


/*---------------------------------------------------------------------------
 * OS_Deinit()
 *
 *     Deinitializes the stack and operating system layer. This function
 *     is not called by the stack, but may be called by your operating
 *     system or application to shut down the stack and free its resources.
 */
void OS_Deinit(void);

/*---------------------------------------------------------------------------
 * OS_GetSystemTime()
 *
 *     Called by the stack to get the current system time in ticks.
 *
 *     The system time provided by this function can start at any value;
 *     it does not to start at 0. However, the time must "roll over" only
 *     when reaching the maximum value allowed by TimeT. For instance, a
 *     16-bit TimeT must roll over at 0xFFFF. A 32-bit TimeT must roll
 *     over at 0xFFFFFFFF.
 *
 *     System ticks may or may not be equivalent to milliseconds. See the
 *     MS_TO_TICKS macro in config.h (General Configuration Constants)
 *     for more information.
 *
 * Returns:
 *     The current time in ticks.
 */
TimeT OS_GetSystemTime(void);


/*---------------------------------------------------------------------------
 * OS_Rand()
 *
 *     Called by the stack to generate a random number between
 *     0x0000 and 0xFFFF.
 *
 * Returns:
 *     A 16-bit random number.
 */
U16 OS_Rand(void);

#define OS_Rand() BTHAL_UTILS_Rand()


/*---------------------------------------------------------------------------
 * OS_StopHardware()
 *
 *     Called by the Blue SDK stack to enter a critical section, during which
 *     the hardware driver must not execute.
 *
 *     The stack calls OS_StopHardware just before it is going to read
 *     or modify data that might be used by the hardware driver.
 *     Immediately following the data access, the stack calls
 *     OS_ResumeHardware. In most systems, the time spent in the critical
 *     section will be less than 50 microseconds.
 *
 *     In systems where the hardware driver is implemented with a system
 *     task, the hardware driver task must be at a higher priority and
 *     run to completion before the stack task can execute. Between
 *     OS_StopHardware and OS_ResumeHardware, the hardware driver
 *     task must not be scheduled to execute. For example, this could be
 *     accomplished by taking a semaphore required by the hardware driver.
 *
 *     In systems where the hardware driver is implemented entirely in an
 *     interrupt service routine, OS_StopHardware must prevent stack-
 *     related hardware interrupts from firing. Other interrupts can
 *     be left as they are.
 *
 *     The stack never "nests" calls to OS_StopHardware.
 *     OS_ResumeHardware is always called before OS_StopHardware
 *     is called again.
 *
 * Requires:
 *     BT_STACK enabled.
 */
void OS_StopHardware(void);

#define OS_StopHardware() BTHAL_OS_StopHardware()


/*---------------------------------------------------------------------------
 * OS_ResumeHardware()
 *
 *     Called by the Blue SDK stack to leave the critical section entered by
 *     OS_StopHardware.
 *
 *     In systems where the hardware driver is implemented with a system
 *     task, OS_ResumeHardware allows the hardware driver task to be
 *     scheduled again. For example, a semaphore could be released
 *     that allows the hardware driver to run.
 *
 *     In systems where the hardware driver is implemented entirely by an
 *     interrupt service routine, OS_ResumeHardware must restore the
 *     interrupts disabled by OS_StopHardware.
 *
 * Requires:
 *     BT_STACK enabled.
 */
void OS_ResumeHardware(void);

#define OS_ResumeHardware()  BTHAL_OS_ResumeHardware()


/*---------------------------------------------------------------------------
 * OS_MemCopy()
 *
 *     Called by the stack to copy memory from one buffer to another.
 *     
 *     This function's implementation could use the ANSI memcpy function.
 *
 * Parameters:
 *     dest - Destination buffer for data.
 *
 *     source - Source buffer for data. "dest" and "source" must not
 *         overlap.
 *
 *     numBytes - Number of bytes to copy from "source" to "dest".
 */
void OS_MemCopy(void *dest, const void *source, U32 numBytes);

#define OS_MemCopy(dest, source, numBytes) BTHAL_UTILS_MemCopy((U8*)(dest), (const U8*)(source), (numBytes))


/*---------------------------------------------------------------------------
 * OS_MemCmp()
 *
 *     Called by the stack to compare the bytes in two different buffers.
 *     If the buffers lengths or contents differ, this function returns FALSE.
 *
 *     This function's implementation could use the ANSI memcmp
 *     routine as shown:
 *
 *     return (len1 != len2) ? FALSE : (0 == memcmp(buffer1, buffer2, len2));
 *     
 *
 * Parameters:
 *     buffer1 - First buffer to compare.
 *
 *     len1 - Length of first buffer to compare.
 *
 *     buffer2 - Second buffer to compare.
 *
 *     len2 - Length of second buffer to compare.
 *
 * Returns:
 *     TRUE - The lengths and contents of both buffers match exactly.
 *
 *     FALSE - Either the lengths or the contents of the buffers do not
 *         match.
 */
BOOL OS_MemCmp(const void *buffer1, U16 len1, const void *buffer2, U16 len2);

#define OS_MemCmp(buffer1, len1, buffer2, len2) BTHAL_UTILS_MemCmp((const U8*)(buffer1), (len1), (const U8*)(buffer2), (len2))


/*---------------------------------------------------------------------------
 * OS_MemSet()
 *
 *     Fills the destination buffer with the specified byte.
 *
 *     This function's implementation could use the ANSI memset
 *     function.
 *
 * Parameters:
 *     dest - Buffer to fill.
 *
 *     byte - Byte to fill with.
 *
 *     len - Length of the destination buffer.
 */
void OS_MemSet(void *dest, U8 byte, U32 len);

#define OS_MemSet(dest, byte, len) BTHAL_UTILS_MemSet((U8*)(dest), (byte), (len))


#if (XA_MULTITASKING == XA_ENABLED)
/*---------------------------------------------------------------------------
 * OS_StartTimer()
 *
 *     Called by the stack to start the event timer. When the time
 *     expires, it is the system's responsibility to call the notify
 *     function provided. The notify function may be called in either
 *     interrupt or task modes as required by the operating system.
 *
 *     If the timer is already active, OS_StartTimer automatically
 *     cancels the previous timer as if OS_CancelTimer was called.
 *
 *     In non-multitasking systems (where XA_MULTITASKING is disabled), this
 *     function is not used. EVM_Process will call OS_GetSystemTime
 *     frequently to determine if its internal timers have elapsed.
 *
 * Requires:
 *     XA_MULTITASKING enabled.
 *
 * Parameters:
 *     time - number of ticks until the timer fires
 * 
 *     func - The function to call when the timer expires.
 *
 */
void OS_StartTimer(TimeT time, OsTimerNotify func);


/*---------------------------------------------------------------------------
 * OS_CancelTimer()
 *
 *     Called by the stack to stops the event timer. This must prevent
 *     the operating system from calling the timer notification
 *     function provided by OS_StartTimer.
 *
 *     If function is called when the event timer is not running, do
 *     nothing.
 *
 *     In non-multitasking systems (where XA_MULTITASKING is disabled), this
 *     function is not used.
 *
 * Requires:
 *     XA_MULTITASKING enabled.
 */
void OS_CancelTimer(void);

/*---------------------------------------------------------------------------
 * OS_NotifyEvm()
 *
 *     Called by the stack to indicate that EVM_Process should be called.
 *     Depending on your operating system, this call may be made in
 *     interrupt mode or task mode.
 *
 *     In multitasking systems, the stack task alternately calls EVM_Process
 *     and blocks to save CPU time. The block may be implemented by an
 *     event semaphore or similar object. OS_NotifyEvm should post the
 *     semaphore, or otherwise cause the stack task to unblock and
 *     call EVM_Process.
 *
 *     In non-multitasking systems (where XA_MULTITASKING is disabled),
 *     tasks are usually simulated using a round-robin execution scheme. In
 *     this case, EVM_Process is called continuously, and no task ever
 *     blocks. Therefore, this function is not used.
 *
 *     If OS_NotifyEvm is called during EVM_Process execution,
 *     EVM_Process must be executed again before the stack task blocks.
 *
 * Requires:
 *     XA_MULTITASKING enabled.
 */
void OS_NotifyEvm(void);


/*---------------------------------------------------------------------------
 * OS_LockStack()
 *
 *     Called by the stack and its APIs to prevent other tasks from
 *     simultaneous access to the stack. When the stack is "locked" by
 *     a task, calls to OS_LockStack from different task will block
 *     until the stack is unlocked. This allows multiple application tasks
 *     and the stack task to coexist without corrupting data.
 *
 *     OS_LockStack calls may be "nested", meaning that a single task
 *     may call OS_LockStack several times without blocking, then call
 *     OS_UnlockStack an equal number of times.
 *
 *     In multitasking systems, OS_LockStack may be implemented with a
 *     mutual-exclusion semaphore or a similar object. It may be necessary
 *     to check the current task's ID and manage a use count if the operating
 *     system does not handle this for you.
 *
 *     In non-multitasking systems, no task can interrupt another task.
 *     Therefore, this call is not used and need not be present.
 *
 * Requires:
 *     XA_MULTITASKING enabled.
 */
void OS_LockStack(void);

#define OS_LockStack() BTHAL_OS_LockSemaphore(stackMutexHandle, 0)

/*---------------------------------------------------------------------------
 * OS_UnlockStack()
 *
 *     Called by the stack and its APIs to re-enable access to the stack.
 *     If a task calls OS_UnlockStack the same number of times as
 *     OS_LockStack, the stack is unlocked and any task may lock the
 *     stack without blocking.
 *
 *     In non-multitasking systems, no task can interrupt another task.
 *     Therefore, this call is not used and need not be present.
 *
 * Requires:
 *     XA_MULTITASKING enabled.
 */
void OS_UnlockStack(void);

#define OS_UnlockStack()  BTHAL_OS_UnlockSemaphore(stackMutexHandle)

#else /* XA_MULTITASKING disabled */
#define OS_LockStack()   (void)0
#define OS_UnlockStack() (void)0
#define OS_NotifyEvm()   (void)0

#endif /* XA_MUTLTITASKING */

/*---------------------------------------------------------------------------
 * OS_StrCmp()
 *
 *     Compares two strings for equality.
 *
 * Parameters:
 *     Str1 - String to compare.
 *     Str2 - String to compare.
 *
 * Returns:
 *     Zero - If strings match.
 *     Non-Zero - If strings do not match.
 */
U8 OS_StrCmp(const char *Str1, const char *Str2);

#define OS_StrCmp(Str1, Str2) BTHAL_UTILS_StrCmp((Str1), (Str2))


/*---------------------------------------------------------------------------
 * OS_StriCmp()
 *
 *     Compares two strings for equality regardless of case.
 *
 * Parameters:
 *     Str1 - String to compare.
 *     Str2 - String to compare.
 *
 * Returns:
 *     Zero - If strings match.
 *     Non-Zero - If strings do not match.
 */
U8 OS_StriCmp(const char *Str1, const char *Str2);

#define OS_StriCmp(Str1, Str2) BTHAL_UTILS_StriCmp((Str1), (Str2))


/*---------------------------------------------------------------------------
 * OS_StrLen()
 *
 *     Calculate the length of the string.
 *
 * Parameters:
 *     Str - String to count length.
 *
 * Returns:
 *     Returns length of string.
 */
U16 OS_StrLen(const char *Str);

#define OS_StrLen(Str) BTHAL_UTILS_StrLen((Str))


/*---------------------------------------------------------------------------
 * OS_StrCpy()
 *
 *    Copy a string (same as ANSI C strcpy)
 *
 * 	The OS_StrCpy function copies StrSource, including the terminating null character, 
 *	to the location specified by StrDest. No overflow checking is performed when strings 
 *	are copied or appended. 
 *
 *	The behavior of OS_StrCpy is undefined if the source and destination strings overlap 
 *
 * Parameters:
 *     StrDest - Destination string.
 *
 *	StrSource - Source string
 *
 * Returns:
 *     Returns StrDest. No return value is reserved to indicate an error.
 */
char* OS_StrCpy(char* StrDest, const char *StrSource);

#define OS_StrCpy(StrDest, StrSource) BTHAL_UTILS_StrCpy((StrDest), (StrSource))


/*---------------------------------------------------------------------------
 * OS_StrnCpy()
 *
 *    Copy characters of one string to another (same as ANSI C strncpy)
 *
 * 	The OS_StrnCpy function copies the initial Count characters of StrSource to StrDest and 
 *	returns StrDest. If Count is less than or equal to the length of StrSource, a null character 
 *	is not appended automatically to the copied string. If Count is greater than the length of 
 *	StrSource, the destination string is padded with null characters up to length Count. 
 *
 *	The behavior of OS_StrnCpy is undefined if the source and destination strings overlap.
 *
 * Parameters:
 *     StrDest - Destination string.
 *
 *	StrSource - Source string
 *
 *	Count - Number of characters to be copied
 *
 * Returns:
 *     Returns strDest. No return value is reserved to indicate an error.
 */
char* OS_StrnCpy(char* StrDest, const char *StrSource, U32 Count);

#define OS_StrnCpy(StrDest, StrSource, Count) BTHAL_UTILS_StrnCpy((StrDest), (StrSource), (Count))


/*---------------------------------------------------------------------------
 * OS_AtoU32()
 *
 *    Convert strings to double  (same as ANSI C atol)
 *
 * 	  The OS_AtoU32 function converts a character string to an integer value.
 *    The function do not recognize decimal points or exponents.
 *	
 * Parameters:
 *
 *    string - String to be converted which has the following form:
 *
 *    [whitespace][sign][digits]
 *
 *	  Where whitespace consists of space and/or tab characters, which are ignored; 
 *	  sign is either plus (+) or minus (–); and digits are one or more decimal digits. 
 *
 * Returns:
 *	  A U32 value produced by interpreting the input characters as a number.
 *    The return value is 0 if the input cannot be converted to a value of that type. 
 *	  The return value is undefined in case of overflow.
 */
U32 OS_AtoU32(const char *string);

#define OS_AtoU32(string) BTHAL_UTILS_AtoU32((string))


/*---------------------------------------------------------------------------
 * OS_StrCat()
 *
 *    Append a string (same as ANSI C strcat)
 *
 * 	The OS_StrrChr function finds the last occurrence of c (converted to char) in string. 
 *	The search includes the terminating null character.
 *	The OS_StrCat function appends strSource to strDest and terminates the resulting string 
 *	with a null character. The initial character of strSource overwrites the terminating null 
 *	character of strDest. No overflow checking is performed when strings are copied or 
 *	appended. The behavior of OS_StrCat is undefined if the source and destination strings 
 *	overlap
 *
 * Parameters:
 *     strDest - Null-terminated destination string.
 *
 *	strSource - Null-terminated source string
 *
 * Returns:
 *     Returns the destination string (strDest). No return value is reserved to indicate an error.
 */
char *OS_StrCat(char *strDest, const char *strSource );

#define OS_StrCat(strDest, strSource) BTHAL_UTILS_StrCat((strDest), (strSource))


/*---------------------------------------------------------------------------
 * OS_StrrChr()
 *
 *    Scan a string for the last occurrence of a character (same as ANSI C strrchr)
 *
 * 	The OS_StrrChr function finds the last occurrence of c (converted to char) in string. 
 *	The search includes the terminating null character.
 *
 * Parameters:
 *     Str - Null-terminated string to search.
 *
 *	c - Character to be located
 *
 * Returns:
 *     Returns a pointer to the last occurrence of c in Str, or 0 if c is not found.
 */
char *OS_StrrChr(const char *Str, int c);

#define OS_StrrChr(Str, c) BTHAL_UTILS_StrrChr((Str), (c))

/*---------------------------------------------------------------------------
 * OS_StrCmpUtf8()
 *
 *     Compares two strings for equality.
 *
 * Parameters:
 *     Str1 - String to compare.
 *     Str2 - String to compare.
 *
 * Returns:
 *     Zero - If strings match.
 *     Non-Zero - If strings do not match.
 */
U8 OS_StrCmpUtf8(const BtlUtf8 *Str1, const BtlUtf8 *Str2);

#define OS_StrCmpUtf8(Str1, Str2) OS_StrCmp((char*)(Str1), (char*)(Str2))


/*---------------------------------------------------------------------------
 * OS_StriCmpUtf8()
 *
 *     Compares two strings for equality regardless of case.
 *
 * Parameters:
 *     Str1 - String to compare.
 *     Str2 - String to compare.
 *
 * Returns:
 *     Zero - If strings match.
 *     Non-Zero - If strings do not match.
 */
U8 OS_StriCmpUtf8(const BtlUtf8 *Str1, const BtlUtf8 *Str2);

#define OS_StriCmpUtf8(Str1, Str2) OS_StriCmp((char*)(Str1), (char*)(Str2))


/*---------------------------------------------------------------------------
 * OS_StrLenUtf8()
 *
 *     Calculate the length of the string.
 *
 *	NOTE: Currently calculates the number of bytes and not the number of characters
 *
 * Parameters:
 *     Str - String to count length.
 *
 * Returns:
 *     Returns length of string.
 */
U16 OS_StrLenUtf8(const BtlUtf8 *Str);

#define OS_StrLenUtf8(Str) OS_StrLen((char*)(Str))


/*---------------------------------------------------------------------------
 * OS_StrCpyUtf8()
 *
 *    Copy a string (same as ANSI C strcpy)
 *
 * 	The OS_StrCpy function copies StrSource, including the terminating null character, 
 *	to the location specified by StrDest. No overflow checking is performed when strings 
 *	are copied or appended. 
 *
 *	The behavior of OS_StrCpy is undefined if the source and destination strings overlap 
 *
 * Parameters:
 *     StrDest - Destination string.
 *
 *	StrSource - Source string
 *
 * Returns:
 *     Returns StrDest. No return value is reserved to indicate an error.
 */
char* OS_StrCpyUtf8(BtlUtf8 *StrDest, const BtlUtf8 *StrSource);

#define OS_StrCpyUtf8(StrDest, StrSource) OS_StrCpy((char*)(StrDest), (char*)(StrSource))


/*---------------------------------------------------------------------------
 * OS_StrnCpyUtf8()
 *
 *    Copy characters of one string to another (same as ANSI C strncpy)
 *
 *	NOTE: Currently works for ASCII strings only
 *
 * 	The OS_StrnCpy function copies the initial Count characters of StrSource to StrDest and 
 *	returns StrDest. If Count is less than or equal to the length of StrSource, a null character 
 *	is not appended automatically to the copied string. If Count is greater than the length of 
 *	StrSource, the destination string is padded with null characters up to length Count. 
 *
 *	The behavior of OS_StrnCpy is undefined if the source and destination strings overlap.
 *
 * Parameters:
 *     StrDest - Destination string.
 *
 *	StrSource - Source string
 *
 *	Count - Number of characters to be copied
 *
 * Returns:
 *     Returns strDest. No return value is reserved to indicate an error.
 */
char* OS_StrnCpyUtf8(BtlUtf8 *StrDest, const BtlUtf8 *StrSource, U32 Count);

#define OS_StrnCpyUtf8(StrDest, StrSource, Count) OS_StrnCpy((char*)(StrDest), (char*)(StrSource), (Count))


/*---------------------------------------------------------------------------
 * OS_StrCat()
 *
 *    Append a string (same as ANSI C strcat)
 *
 * 	The OS_StrrChr function finds the last occurrence of c (converted to char) in string. 
 *	The search includes the terminating null character.
 *	The OS_StrCat function appends strSource to strDest and terminates the resulting string 
 *	with a null character. The initial character of strSource overwrites the terminating null 
 *	character of strDest. No overflow checking is performed when strings are copied or 
 *	appended. The behavior of OS_StrCat is undefined if the source and destination strings 
 *	overlap
 *
 * Parameters:
 *     strDest - Null-terminated destination string.
 *
 *	strSource - Null-terminated source string
 *
 * Returns:
 *     Returns the destination string (strDest). No return value is reserved to indicate an error.
 */
char *OS_StrCatUtf8(BtlUtf8 *strDest, const BtlUtf8 *strSource);

#define OS_StrCatUtf8(strDest, strSource) OS_StrCat((char*)(strDest), (char*)(strSource))


/*---------------------------------------------------------------------------
 * OS_StrrChr()
 *
 *    Scan a string for the last occurrence of a character (same as ANSI C strrchr)
 *
 * 	The OS_StrrChr function finds the last occurrence of c (converted to char) in string. 
 *	The search includes the terminating null character.
 *
 *	NOTE: Currently works for ASCII strings only
 
 * Parameters:
 *     Str - Null-terminated string to search.
 *
 *	c - Character to be located
 *
 * Returns:
 *     Returns a pointer to the last occurrence of c in Str, or 0 if c is not found.
 */
char *OS_StrrChrUtf8(const BtlUtf8 *Str, int c);

#define OS_StrrChrUtf8(Str, c) OS_StrrChrUtf8((char*)(Str), (c))

#if XA_DEBUG == XA_ENABLED
/*---------------------------------------------------------------------------
 * OS_Assert()
 *
 *     Called by the stack to indicate that an assertion failed. OS_Assert
 *     should display the failed expression and the file and line number
 *     where the expression occurred.
 *
 *     The stack uses Assert calls, which are redefined to call OS_Assert
 *     if their expression fails. These calls verify the correctness of
 *     function parameters, internal states, and data to detect problems
 *     during debugging.
 *
 * Requires:
 *     XA_DEBUG enabled.
 *
 * Parameters:
 *     expression - A string containing the failed expression.
 *
 *     file - A string containing the file in which the expression
 *         occurred.
 *
 *     line - The line number that tested the expression.
 */
void OS_Assert(const char *expression, const char *file, U16 line);

#define OS_Assert(expression, file, line) BTHAL_UTILS_Assert((expression), (file), (line))


/* A define to convert asserts to OS_Assert calls */
#define Assert(exp)  (((exp) != 0) ? (void)0 : OS_Assert(#exp,__FILE__,(U16)__LINE__))

#define Assert_msg(exp,msg)  ((exp) ? (void)0 : OS_Assert(msg,__FILE__,(U16)__LINE__))


/* A similar define that always evaluates the expression regardless of
 * the DEBUG mode
 */
#define AssertEval(exp) Assert(exp)

#else /* XA_DEBUG */

/* When XA_DEBUG is disabled, null versions of these functions are
 * substituted for the calls.
 */
#define Assert(ex) (void)0
#define AssertEval(exp) (void)(exp)

#endif /* XA_DEBUG */


#if XA_DEBUG_PRINT == XA_ENABLED

/*---------------------------------------------------------------------------
 * OS_Report()
 *
 *     Called by the stack to report debugging information.
 *
 * Requires:
 *     XA_DEBUG_PRINT enabled.
 *
 * Parameters:
 *     format - A string containing the failed expression.
 *
 *     ... - printf style arguments.
 */
void OS_Report(const char *format, ...);

#define OS_Report(s)	 BTHAL_LOG_INFO(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_BTSTACK, s)

/* DebugPrint and Report are used in place of OS_Report() */
#define DebugPrint(s) OS_Report(s)
#define Report(s) OS_Report(s)

#else /* XA_DEBUG_PRINT */

#define DebugPrint(s) (void)0
#define Report(s) (void)0

#endif /* XA_DEBUG_PRINT */

#endif /* __OSAPI_H */


