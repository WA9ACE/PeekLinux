#ifndef PDEAPI_H
#define PDEAPI_H
/*****************************************************************************
 
  FILE NAME:  pdeapi.h

  DESCRIPTION : PDE task interface
 
      This include file provides system wide global type declarations and 
      constants
 
  HISTORY     :
      See Log at end of file

*****************************************************************************/

#include "sysdefs.h"
#include "exeapi.h"

/*----------------------------------------------------------------------------
 Defines Constants used in this file
----------------------------------------------------------------------------*/

#if 0
/* simulates u-Nav GPS & u-Nav simulator */
#define GPS_SIMULATION_IN_USE /* not build default */
#endif

/* Amala - 08/07/02 */
/* Keep this on TEMPORARILY so NO GPS download happens unless */
/*  manually initiated.                                       */
#if 1
/* ETS initiates GPS initialization & firmware download */
#define MANUAL_GPS_INIT_IN_USE /* TEMP build default */
#endif

/* Amala K. - 03/04/03 */
/* GPS Code Phases adjusted with System Time Offset */
#if 0
#define GPS_CODE_PHASE_ADJUST_IN_USE
#endif

#if 0 
/* simulates CDMA/GPS switching */
#define SWITCHING_SIMULATION_IN_USE /* not build default */
#endif

/* Amala - Moved from locapi.c */
#if 0
#define PDDM_SIMULATION_IN_USE /* not build default */
#endif

#if 0
/* inhibits sleep/wakeup sequences for u-Nav GPS */
/* NOTE: if   defined & GPS_SIMULATION_IN_USE is defined */
/*       then NO_SLEEP_WAKEUP_IN_USE will be UNdefined in PDE */
#define NO_SLEEP_WAKEUP_IN_USE /* not build default */
#endif

#if 0
/* workaround for UNAV SIMULATOR BUGs */
/* use when UNAV SIMULATOR used */
#define UNAV_SIMULATOR_IN_USE
#endif

/* Define PDE command mailbox ids */
#define PDE_MAILBOX              EXE_MAILBOX_1_ID        /* Command mailbox */

/* Define startup signal for PDE task */
#define PDE_STARTUP_SIGNAL       EXE_SIGNAL_1

/*------------------------------------------------------------------------
* Define typedefs used in PDE API
*------------------------------------------------------------------------*/

/* Define PDE Task msg Ids */
typedef enum 
{
   /* from PDE */
   PDE_PROCESS_INTERNAL_EVENT_MSG,
   PDE_TIMER_EXPIRED_MSG,

   /* from PSW */
   PDE_PSEUDORANGE_MSMT_REQ_MSG,
   PDE_SESSION_END_REQ_MSG,
   PDE_GPS_CONT_GPS_MSG,

#ifdef SYS_OPTION_GPS
   /* from IOP */
   PDE_UNAV_GPS_READY_MSG,
   PDE_UNAV_GPS_INOP_MSG,
   PDE_UNAV_GPS_SLEEP_COMPLETE_RSP_MSG,
   PDE_UNAV_GPS_RX_DATA_MSG,
#endif
   /* from L1D */
   PDE_UNAV_GPS_80MS_TIME_CB_RSP_MSG,
   PDE_GPS_INIT_GPS_ACK_MSG,
   PDE_GPS_END_GPS_ACK_MSG
} PdeMsgIdT;

#ifdef GPS_SIMULATION_IN_USE

#ifdef SYS_OPTION_GPS_CHIP
#include "gpspkt.h"
#endif

#endif /* GPS_SIMULATION_IN_USE */


/* PDE_PSEUDORANGE_MSMT_REQ_MSG:
 *    Sent by PSW when an IS-801 request for Pseudo-Range measurement message
 *    and GPS Acquisition Assistance data are both received from BS.
 *
 *    The parameter records are specified in IS-801.
 */

#define PDE_PRM_MAX_SIZE  	   3
#define PDE_MAX_ASSIST_SIZE	 113

typedef PACKED struct
{
   bool     IdleOrTraffic;  /* TRUE = Idle, FALSE = Traffic */
   bool     SystemTimeOffsetIncl;
   uint16   ReqMsmtRecLen;  /* Request PRM Rec Length */
   uint8    ReqPseudorangesRec[PDE_PRM_MAX_SIZE];  /* Req PRM Buffer */
   uint16   ProvAcqAssistRecLen;  /* Prov GPS Acquisition Assist Rec Length */
   uint8    ProvAcqAssistRec[PDE_MAX_ASSIST_SIZE];  /* Prov GPS Acquisition Assist Buffer */
} PdePseudorangeMsmtReqMsgT;


/* PDE_SESSION_END_REQ_MSG:
 *    Sent by PSW to end a GPS Session. PSW passes on if it reset
 *    the MS to System Determination (TRUE) or not (FALSE). PDE 
 *    responds with a PSW_PDE_SESSION_END_RSP_MSG.
 */
typedef PACKED struct
{
	bool		SystemDeter; /* TRUE=MS reset to SYS DETER */
} PdeSessionEndReqMsgT;

#ifdef SYS_OPTION_GPS
/* PDE_UNAV_GPS_RX_DATA_MSG:
 *    This message is sent when there is a received packet which 
 *    contains packet data from u-Nav GPS. 
 */

/* maximum length of packet pay load */
#define MAX_UNAV_RX_PACKET_LEN  256

typedef PACKED struct
{
   uint8         PacketId;
   uint8         PacketLen;  
   uint8         PacketBody[MAX_UNAV_RX_PACKET_LEN];
} PdeUnavRxPacketMsgT;
#endif /* SYS_OPTION_GPS */

/* PDE_TIMER_EXPIRED_MSG:
 *    Timer message for all PDE timers.
 */
typedef PACKED struct
{
   uint8     	TimerId; /* ID of expired timer */
} PdeTimerExpiredMsgT;


/* PDE_UNAV_GPS_80MS_TIME_CB_RSP_MSG */
typedef PACKED struct
{
   uint8 SystemTime[SYS_SYSTIME_SIZE];
} PdeUnavGps80MsTimeCbRspMsgT;

/* PDE_GPS_END_GPS_ACK_MSG */
typedef PACKED struct
{
	uint16 PilotPN;
	int16  SystemTimeOffset;
} PdeGpsEndGpsAckMsgT;

/*****************************************************************************
* $Log: pdeapi.h $
* Revision 1.1  2007/10/29 10:52:59  binye
* Initial revision
* Revision 1.1  2007/10/09 15:12:51  binye
* Initial revision
* Revision 1.1  2004/01/22 10:42:38  fpeng
* Initial revision
* Revision 1.10  2003/03/04 18:53:12  amala
* Added compile switch GPS_CODE_PHASE_ADJUST_IN_USE for enabling GPS Code Phase adjustments.
* Revision 1.9  2003/01/23 17:16:21  amala
* In PdeUnavGps80MsTimeCbRspMsgT struct, changed SystemTime type from uint32 to uint8 array of SYS_SYSTIME_SIZE, to support 36 bit system time for GPS reference time, CR 1797.
* Revision 1.8  2002/10/31 18:22:34  amala
* changed erroneously changed compile switch for GPS
* Revision 1.7  2002/10/24 09:39:30  jbenders
* Added a PilotPN field to the PdeGpsEndGpsAckMsgT since the
* PDE task needs it to send to the base station at the end of session.
* Revision 1.6  2002/09/21 08:19:16  mshaver
* Extract gps chip specific information to the new file gpspkt.h and
* include it here.
* Revision 1.5  2002/08/22 10:37:05  amala
* Deleted unused interface (message ID) PDE_SESSION_ABORT_RSP_MSG
* Revision 1.4  2002/08/08 15:11:00  robertk
* Updated to conform to documented PDE API plus updates due to implementation and integration.
* Revision 1.3.1.2.1.2  2002/07/03 12:03:13  amala
* Deleted message PDE_GPS_CONT_CDMA_ACK_MSG,
* from PdeMsgIdT structure.
* Revision 1.3.1.2.1.1  2002/06/25 17:12:49  amala
* Duplicate revision
* Revision 1.3.1.2  2002/06/25 17:12:49  amala
* Added GPS message interfaces.
* Revision 1.3.1.1  2002/06/24 19:02:23  amala
* Duplicate revision
* Revision 1.3  2002/06/24 19:02:23  amala
* Changed size of message interface data structures.
* Revision 1.2  2002/06/24 11:34:08  amala
* Added new GPS/AFLT message interfaces
* Revision 1.1  2002/06/17 13:44:38  fpeng
* Initial revision
* Revision 1.1  2002/03/28 13:07:04  wavis
* Initial revision
*****************************************************************************/
#endif
