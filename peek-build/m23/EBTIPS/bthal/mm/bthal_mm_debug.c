/*******************************************************************************\
*                                                                               *
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION            *
*                                                                               *
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE          *
*   UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE       *
*   APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO       *
*   BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT        *
*   OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL         *
*   DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.          *
*                                                                               *
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      bthal_mm_debug.c
*
*   DESCRIPTION:    This file implements debug functions for BTHAL multimedia.
*
*   AUTHOR:         Ronen Kalish
*
\*******************************************************************************/

#if BTL_CONFIG_A2DP == BTL_CONFIG_ENABLED

#include "bthal_mm_debug.h"

#ifdef BTHAL_MM_DEBUG

/* Include files */
#include "ffs/ffs_api.h" /* TODO ronenk: replace with some decent abstraction */
#include "bthal_os.h"

static U32 uPullHistRange[ BTHAL_MM_DEBUG_PULL_TIMING_HIST_SIZE ] = 
    { 5, 10, 15, 20, 25, 30, 35, 40, 50, 70, 100 };

static BthalOsTime     tLastTime, tCurrentTime;

/* BTHAL MM debug information structure */
TDbgBthalMmValues tDbgBthalMm;

/*-------------------------------------------------------------------------------
 * BTHAL_MM_DEBUG_Init ()
 */
void BTHAL_MM_DEBUG_Init (void)
{
    BTHAL_UTILS_MemSet (tDbgBthalMm, 0, sizeof (tDbgBthalMm) );
    tLastTime = 0;
}

/*-------------------------------------------------------------------------------
 * BTHAL_MM_DEBUG_Deinit ()
 */
void BTHAL_MM_DEBUG_Deinit (void)
{
}

/*-------------------------------------------------------------------------------
 * BTHAL_MM_DEBUG_NotifyPull ()
 */
void BTHAL_MM_DEBUG_NotifyPull (void)
{
    U32 uIndex;

    /* Increase number of PCM pull requests */
    tDbgBthalMm.uPullRequestCount++;

    /* Update pull timing histogram */
    /* if this is the first pull request, just update the last time (for the next request) */
    if (0 == tLastTime)
    {
        BTHAL_OS_GetSystemTime(&tLastTime);
        return;
    }
    else
    {
        /* get current time */
        BTHAL_OS_GetSystemTime(&tCurrentTime);

        /* get time diff in millisec */
        tDbgBthalMm.uLastPullTimeDiff = BTHAL_OS_TICKS_TO_MS (tCurrentTime) - BTHAL_OS_TICKS_TO_MS(tLastTime);

        /* Find the index matching the time diff */
        for (uIndex = 0; 
             (uIndex < (BTHAL_MM_DEBUG_PULL_TIMING_HIST_SIZE-1)) && (tDbgBthalMm.uLastPullTimeDiff > uPullHistRange[ uIndex ]);
             uIndex++);

        /* and increase histogram value */
        tDbgBthalMm.uPullTimingsHist[ uIndex ]++;

        /* set last time as current time for the next pull operation */
        tLastTime = tCurrentTime;
    }
}

/*-------------------------------------------------------------------------------
 * BTHAL_MM_DEBUG_Reset ()
 */
void BTHAL_MM_DEBUG_Reset (void)
{
    BTHAL_MM_DEBUG_Init();
}

/*-------------------------------------------------------------------------------
 * BTHAL_MM_DEBUG_Print ()
 */
void BTHAL_MM_DEBUG_Print (void)
{
    BTHAL_MM_LOG_INFO(("BTHAL Multimedia debug information:"));
    BTHAL_MM_LOG_INFO(("Number of PCM pull requests                      = %d", tDbgBthalMm.uPullRequestCount));
    BTHAL_MM_LOG_INFO(("Number of pending replies to PCM pull requsts    = %d", tDbgBthalMm.uPendingReplyCount));
    BTHAL_MM_LOG_INFO(("Number of PCM callbacks                          = %d", tDbgBthalMm.uPcmCBCount));
    BTHAL_MM_LOG_INFO(("Maximum number of PCM blocks cuncurrently in use = %d", tDbgBthalMm.uMaxPcmBlocksInIuse));
    BTHAL_MM_LOG_INFO(("PCM Pull timing histogram (in milli-sec):"));
    BTHAL_MM_LOG_INFO(("0-5:%d, 6-10:%d, 11-15:%d, 16-20:%d, 20-25:%d, 25-30:%d", tDbgBthalMm.uPullTimingsHist[ 0 ],
            tDbgBthalMm.uPullTimingsHist[ 1 ], tDbgBthalMm.uPullTimingsHist[ 2 ],
            tDbgBthalMm.uPullTimingsHist[ 3 ], tDbgBthalMm.uPullTimingsHist[ 4 ],
            tDbgBthalMm.uPullTimingsHist[ 5 ]));
    BTHAL_MM_LOG_INFO(("30-35:%d, 35-40:%d, 40-50:%d, 50-70:%d, 70-100:%d, 100+:%d", tDbgBthalMm.uPullTimingsHist[ 6 ],
            tDbgBthalMm.uPullTimingsHist[ 7 ], tDbgBthalMm.uPullTimingsHist[ 8 ],
            tDbgBthalMm.uPullTimingsHist[ 9 ], tDbgBthalMm.uPullTimingsHist[ 10 ],
            tDbgBthalMm.uPullTimingsHist[ 11 ]));
    if (tDbgBthalMm.uDebugMask & BTHAL_MM_DEBUG_PCM_RECORD)
    {
        BTHAL_MM_LOG_INFO(("PCM recording is enabled"));
    }
    if (tDbgBthalMm.uDebugMask & BTHAL_MM_DEBUG_DUPLICATION_DETECTION)
    {
        BTHAL_MM_LOG_INFO(("Duplication detection is enabled"));
    }
}

/*-------------------------------------------------------------------------------
 * BTHAL_MM_DEBUG_NotifyPlayStop ()
 */
void BTHAL_MM_DEBUG_NotifyPlayStop (void)
{
    const S8        sFilename[] = "/a.pcm";
    T_FFS_FD        ffs_fd;

    /* only dump to file if PCM recording is on */
    if (tDbgBthalMm.uDebugMask & BTHAL_MM_DEBUG_PCM_RECORD)
    {
        if (tDbgBthalMm.uPcmMemIndex <= 0)
        {
            return;
        }
        
        ffs_fd = ffs_open((const char *)sFilename, FFS_O_WRONLY | FFS_O_TRUNC | FFS_O_APPEND);
        if (ffs_fd >= 0)
        {
            ffs_write(ffs_fd, tDbgBthalMm.uPcmMemoryBuf, tDbgBthalMm.uPcmMemIndex);
            tDbgBthalMm.uPcmMemIndex = 0;
            ffs_close(ffs_fd);
        }
        else
        {
            BTHAL_MM_LOG_ERROR(("BTHAL_MM_DEBUG_DumpPcmBuffer: ffs_open (%s) returned status %d",
                                sFilename, ffs_fd));
        }
    }
}

/*-------------------------------------------------------------------------------
 * BTHAL_MM_DEBUG_NotifyNewBuffer ()
 */
void BTHAL_MM_DEBUG_NotifyNewBuffer (U8* pBuffer, U32 uBufferLength)
{
    if (tDbgBthalMm.uDebugMask & BTHAL_MM_DEBUG_DUPLICATION_DETECTION)
    {
        /* check if the buffer address is equal to last buffer address - possible duplication */
        if ((NULL != tDbgBthalMm.pLastBufferAddress) && (tDbgBthalMm.pLastBufferAddress == pBuffer))
        {
            BTHAL_MM_LOG_ERROR(("BTHAL_MM_DEBUG_NotifyNewBuffer: received buffer %p twice!",
                                tDbgBthalMm.pLastBufferAddress));
        }
        tDbgBthalMm.pLastBufferAddress = pBuffer; 
        
        /* also compare buffer contents */
        if ((0 != tDbgBthalMm.uLastBufferLength) &&
            (BTHAL_TRUE == BTHAL_UTILS_MemCmp (tDbgBthalMm.uLastBufferContent, tDbgBthalMm.uLastBufferLength, 
                                               pBuffer, uBufferLength)))
        {
            BTHAL_MM_LOG_ERROR(("BTHAL_MM_DEBUG_NotifyNewBuffer: new buffer at %p, size %d equals last buffer", 
                                pBuffer, uBufferLength));
            BTHAL_MM_LOG_ERROR(("BTHAL_MM_DEBUG_NotifyNewBuffer: Time diff from last pull: %d ms", 
                                tDbgBthalMm.uLastPullTimeDiff));
        }
    
        /* copy new buffer */
        BTHAL_UTILS_MemCopy (tDbgBthalMm.uLastBufferContent, pBuffer, uBufferLength);
        tDbgBthalMm.uLastBufferLength = uBufferLength;
    }

    /* store the buffer for PCM recording */
    if ((tDbgBthalMm.uDebugMask & BTHAL_MM_DEBUG_PCM_RECORD) && 
        ((tDbgBthalMm.uPcmMemIndex + uBufferLength) < BTAHL_MM_DEBUG_PCM_MEM_BUF_SIZE))
    {
        BTHAL_UTILS_MemCopy (&(tDbgBthalMm.uPcmMemoryBuf[ tDbgBthalMm.uPcmMemIndex ]), 
                             pBuffer, uBufferLength);
        tDbgBthalMm.uPcmMemIndex += uBufferLength;
    }
}

#else

void BTHAL_MM_DEBUG_Init (void) {}
void BTHAL_MM_DEBUG_Deinit (void) {}
void BTHAL_MM_DEBUG_NotifyPull (void) {}
void BTHAL_MM_DEBUG_Reset (void) {}
void BTHAL_MM_DEBUG_Print (void) {}
void BTHAL_MM_DEBUG_NotifyPlayStop (void) {}
void BTHAL_MM_DEBUG_NotifyNewBuffer (U8* pBuffer, U32 uBufferLength) {}

#endif /* BTHAL_MM_DEBUG */

#endif /* BTL_CONFIG_A2DP == BTL_CONFIG_ENABLED */

