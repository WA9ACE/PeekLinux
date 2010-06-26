/*******************************************************************************\
##                                                                              *
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION           *
##                                                                              *
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE         *
##  UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE       *
##  APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO       *
##  BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT        *
##  OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL         *
##  DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.          *
##                                                                              *
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      bthal_mm_debug.h
*
*   BRIEF:          This file defines the debug API of the BTHAL multimedia.
*                  
*
*   DESCRIPTION:    General
*                   -------
*                   The BTHAL multimedia is the interface between the BTL_A2DP
*                   module and the platform dependent multimedia module.
*
*                   It provides access to platform dependant multimedia services:
*
*                   1) Receiving media data (in different formats) from the MM, 
*                   and hand them over to BTL_A2DP module.
*
*                   2) Access and set codec capabilites and configuration.
*
*                   3) Indicate the current state of the transport channel to the MM.
*
*
*                   Debug interface
*                   ---------------
*                   This file defines the structure and functions used to retrieve
*                   debug information from BTHAL multimedia module
*
*   AUTHOR:         Ronen Kalish
*
\*******************************************************************************/


#ifndef __BTHAL_MM_DEBUG_H
#define __BTHAL_MM_DEBUG_H

/* Include files */
#include <bthal_common.h>
#include <bthal_log.h>

/* Enables debug functionality */
#define BTHAL_MM_DEBUG 

#define BTHAL_MM_LOG_ERROR(msg)      BTHAL_LOG_ERROR(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_A2DP, msg)   
#define BTHAL_MM_LOG_INFO(msg)       BTHAL_LOG_INFO(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_A2DP, msg)
#define BTHAL_MM_LOG_FUNCTION(msg)   BTHAL_LOG_FUNCTION(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_A2DP, msg)

/* Debug features definitions (to allow run-time enable / disable */
#define BTHAL_MM_DEBUG_PCM_RECORD               0x1
#define BTHAL_MM_DEBUG_DUPLICATION_DETECTION    0x2

/* The size of the pull timing histogram */
#define BTHAL_MM_DEBUG_PULL_TIMING_HIST_SIZE                            (12)
/* 0-5, 5-10, 10-15, 15-20, 20-25, 25-30, 30-35, 35-40, 40-50, 50-70, 70-100, 100+ */

#define BTAHL_MM_DEBUG_PCM_MEM_BUF_SIZE                                 1024 /* (160*2304) */
#define BTAHL_MM_DEBUG_L1_BUF_SIZE                                      5000

/*---------------------------------------------------------------------------
 * TDbgBthalMmValues structure
 *
 * Contains BTHAl MM debug information
 */
typedef struct
{
    U32 uDebugMask;
    U32 uPullRequestCount; /* number of L1 requests */
    U32 uPendingReplyCount; /* number of L1 requests that returned pending */
    U32 uPcmCBCount; /* number of L1 callbacks */
    U32 uMaxPcmBlocksInIuse; /* maximum number of PCM blocks in use concurrently */
    U32 uPullTimingsHist[ BTHAL_MM_DEBUG_PULL_TIMING_HIST_SIZE ]; /* histogram of timings between L1 pull requests */
    U32 uLastPullTimeDiff; /* diff between last and the one before pull operations (last value to update the histogram) */
    U8  uPcmMemoryBuf[ BTAHL_MM_DEBUG_PCM_MEM_BUF_SIZE + 4 ]; /* buffer holding PCM stream */
    U32 uPcmMemIndex; /* index into above buffer */
    U8 *pLastBufferAddress; /* address of last buffer supplied by L1 */
    U32 uLastBufferLength; /* length of last buffer supplied by L1 */
    U8  uLastBufferContent[ BTAHL_MM_DEBUG_L1_BUF_SIZE ]; /* content of last buffer supplied by L1 */
} TDbgBthalMmValues;

/* BTHAL MM debug information structure */
extern TDbgBthalMmValues tDbgBthalMm;

/*-------------------------------------------------------------------------------
 * BTHAL_MM_DEBUG_Init()
 *
 * Brief:  
 *      Initialize the BTHAL MM Debug.
 *
 * Description: 
 *      This function should only be called once at system-startup.
 *      
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      None
 *
 * Returns:
 *      None
 */
void BTHAL_MM_DEBUG_Init (void);

/*-------------------------------------------------------------------------------
 * BTHAL_MM_DEBUG_Init()
 *
 * Brief:  
 *      De-initialize the BTHAL MM Debug.
 *
 * Description: 
 *      This function should only be called once at system-shutdown.
 *      
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      None
 *
 * Returns:
 *      None
 */
void BTHAL_MM_DEBUG_Deinit (void);

/*-------------------------------------------------------------------------------
 * BTHAL_MM_DEBUG_Init()
 *
 * Brief:  
 *      Notify of a pull operation start.
 *
 * Description: 
 *      This function is called whenever a pull request is placed. It updates
 *      The pull timing histogram.
 *      
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      None
 *
 * Returns:
 *      None
 */
void BTHAL_MM_DEBUG_NotifyPull (void);

/*-------------------------------------------------------------------------------
 * BTHAL_MM_DEBUG_Reset()
 *
 * Brief:  
 *      Resets debug information.
 *
 * Description: 
 *      This function is called When debug information should be cleared, e.g.
 *      when playing a new song
 *      
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      None
 *
 * Returns:
 *      None
 */
void BTHAL_MM_DEBUG_Reset (void);

/*-------------------------------------------------------------------------------
 * BTHAL_MM_DEBUG_Print()
 *
 * Brief:  
 *      Prints debug information.
 *
 * Description: 
 *      This function is called When the user request debug information to be printed
 *      
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      None
 *
 * Returns:
 *      None
 */
void BTHAL_MM_DEBUG_Print (void);

/*-------------------------------------------------------------------------------
 * BTHAL_MM_DEBUG_NotifyPlayStop()
 *
 * Brief:  
 *      Notifies the MM debug module that play has stopped
 *
 * Description: 
 *      This function is called When the play operation is stopped for any reason.
 *      The function will dump the PCM buffer into a file, if PCM recording is on.
 *      
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      None
 *
 * Returns:
 *      None
 */
void BTHAL_MM_DEBUG_NotifyPlayStop (void);

/*-------------------------------------------------------------------------------
 * BTHAL_MM_DEBUG_NotifyNewBuffer()
 *
 * Brief:  
 *      Notifies the MM debug module that a new buffer is available from L1
 *
 * Description: 
 *      This function is called When L1 has supplied a new buffer. The function
 *      handles the buffer according to current activated debug features.
 *      
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      pBuffer - pointer to the new L1 data
 *      uBufferLength - data size
 *
 * Returns:
 *      None
 */
void BTHAL_MM_DEBUG_NotifyNewBuffer (U8* pBuffer, U32 uBufferLength);

#endif /* __BTHAL_MM_DEBUG_H */

