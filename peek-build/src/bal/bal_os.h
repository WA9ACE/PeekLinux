/*=================================================================================

	HEADER NAME : bal_os.h
	MODULE NAME : Bal
	
	PRE-INCLUDE FILES DESCRIPTION
	
	
GENERAL DESCRIPTION
  OS send Msg and Signal interface

BYD TECHFAITH Wireless Confidential Proprietary
(c) Copyright 2008 by BYD TECHFAITH Wireless. All Rights Reserved.
===================================================================================
Revision History
Modification Tracking
Date Author Number Description of changes
---------- ------------ --------- --------------------------------------------


	
=================================================================================*/

#ifndef _BAL_OS_H
#define _BAL_OS_H

#include "monapi.h"
#include "nucleus.h"

/* Define Signal types of event wait */
#define BOS_SIGNAL_TRUE    TRUE
#define BOS_SIGNAL_FALSE   FALSE

/* Define timeout type of event wait */
#define BOS_TIMEOUT_FALSE  NU_SUSPEND

/* Define constant SemaphoreGet Timeout Values */
#define BOS_SEM_WAIT_FOREVER  NU_SUSPEND

/* Define SeamphoreGet return code */
#define BOS_SEM_SUCCESS       NU_SUCCESS
#define BOS_SEM_TIMEOUT       NU_TIMEOUT


/* Number of Msec per timer tick */
#define BOS_TIMER_TICK         5             

/* Cal timer ticks from Msec */
//#define BOSCalMsec(NumMsec) (((NumMsec) / BOS_TIMER_TICK) + 1)
#define BOSCalMsec(NumMsec) ((((NumMsec) * 200) + 462) / 923)

/*----------------------------------------------------------------------
* The following typedef defines the 3 event types (signal, mail and 
* time) and the four mailboxes and twenty task signal flags. An application 
* task can use these to determine which particular event, message or
* signal flag or timeout was set to cause their task to be scheduled 
* after calling BOSWaitEvent. 
*----------------------------------------------------------------------*/
typedef enum
{                 
   BOS_MESSAGE_TYPE  = 0x00000001,   /* Event types */           
   BOS_SIGNAL_TYPE   = 0x00000002,   
   BOS_TIMEOUT_TYPE  = 0x00000004,

   BOS_MAILBOX_1     = 0x00000008,   /* Mailbox types */
   BOS_MAILBOX_2     = 0x00000010,              
   BOS_MAILBOX_3     = 0x00000020,              
   BOS_MAILBOX_4     = 0x00000040,              
   BOS_MAILBOX_5     = 0x00000080,              
                         
   BOS_SIGNAL_1      = 0x00000100,   /* Signal types */
   BOS_SIGNAL_2      = 0x00000200,              
   BOS_SIGNAL_3      = 0x00000400,              
   BOS_SIGNAL_4      = 0x00000800,              
   BOS_SIGNAL_5      = 0x00001000,             
   BOS_SIGNAL_6      = 0x00002000,             
   BOS_SIGNAL_7      = 0x00004000,             
   BOS_SIGNAL_8      = 0x00008000,             
   BOS_SIGNAL_9      = 0x00010000,
   BOS_SIGNAL_10     = 0x00020000,             
   BOS_SIGNAL_11     = 0x00040000,             
   BOS_SIGNAL_12     = 0x00080000,             
   BOS_SIGNAL_13     = 0x00100000,             
   BOS_SIGNAL_14     = 0x00200000,             
   BOS_SIGNAL_15     = 0x00400000,             
   BOS_SIGNAL_16     = 0x00800000,             
   BOS_SIGNAL_17     = 0x01000000,             
   BOS_SIGNAL_18     = 0x02000000,             
   BOS_SIGNAL_19     = 0x04000000,             
   BOS_SIGNAL_20     = 0x08000000,
   BOS_SIGNAL_21     = 0x10000000,            
   BOS_SIGNAL_22     = 0x20000000,            
   BOS_SIGNAL_23     = 0x40000000            
} BOSEventWaitT;

/*---------------------------------------------------------------------
* The enumerated type BOSTaskIdT is used by application tasks
* when they want to call a scheduler service and they need to
* know the task id of a particular task. This enumerated type 
* also contains the number of overall tasks that the scheduler 
* will deal with. The order of task ids is important and must
* match the order of task control blocks
* here only two task open, UI and Email Server Task.
*---------------------------------------------------------------------*/
typedef enum
{                 
   BOS_UI_ID		= 21,
   BOS_EM_S_ID	= 29,  //added by Randolph Wang 080219
   BOS_AMDB_S_ID = 30
} BOSTaskIdT;

typedef enum
{                 
   BOS_MAILBOX_1_ID  = 0x00,
   BOS_MAILBOX_2_ID  = 0x01,
   BOS_MAILBOX_3_ID  = 0x02,
   BOS_MAILBOX_4_ID  = 0x03,
   BOS_MAILBOX_5_ID  = 0x04,
   BOS_NUM_MAILBOX
} BOSMailboxIdT;

/* Define message types of event wait */
typedef enum
{                 
   BOS_MESSAGE_FALSE  = 0,
   BOS_MESSAGE_TRUE   = BOS_MESSAGE_TYPE,
   BOS_MESSAGE_MBOX_1 = BOS_MAILBOX_1,
   BOS_MESSAGE_MBOX_2 = BOS_MAILBOX_2,
   BOS_MESSAGE_MBOX_3 = BOS_MAILBOX_3,
   BOS_MESSAGE_MBOX_4 = BOS_MAILBOX_4,
   BOS_MESSAGE_MBOX_5 = BOS_MAILBOX_5
}BOSMessageT;

typedef uint32 BOSSignalT;
typedef NU_SEMAPHORE      BOSSemaphoreT;    

#ifdef __cplusplus
extern "C"
{
#endif
void BOSSignalSet(BOSTaskIdT TaskId, BOSSignalT SignalFlg);
uint32 BOSMsgCheck(BOSTaskIdT TaskId, BOSMailboxIdT MailboxId);
bool BOSMsgRead(BOSTaskIdT TaskId, BOSMailboxIdT MailboxId, uint32 *MsgIdP, 
                       void **MsgBufferP, uint32 *MsgSizeP);
int BOSMsgSend(BOSTaskIdT TaskId, BOSMailboxIdT MailboxId, uint32 MsgId, 
                       void *MsgBufferP, uint32 MsgSize);
int BOSMsgSendToFront(BOSTaskIdT TaskId, BOSMailboxIdT MailboxId, uint32 MsgId, 
                       void *MsgBufferP, uint32 MsgSize);

void * BOSMsgBufferGet(uint32 MsgBufferSize);
void BOSMsgBufferFree(void *MsgBufferP);
BOSEventWaitT BOSEventWait(BOSTaskIdT TaskId, bool Signal, 
                                  BOSMessageT Message, uint32 Timeout);
void* BOSMalloc( uint32 size );
void  BOSFree( void* Ptr );
uint32 BOSGetAvailableSpace( void );

void    BOSSemaphoreCreate(BOSSemaphoreT *SemaphoreCbP, uint32 InitialCount);
void    BOSSemaphoreDelete(BOSSemaphoreT *SemaphoreCbP);
uint32  BOSGetSemaphoreCount(BOSSemaphoreT *SemaphoreCbP);
int32   BOSSemaphoreGet(BOSSemaphoreT *SemaphoreCbP, uint32 Timeout);
void    BOSSemaphoreRelease(BOSSemaphoreT *SemaphoreCbP);
int32   BOSSemaphoreGetWithTimeout(BOSSemaphoreT *SemaphoreCbP, uint32 Timeout);
#ifdef __cplusplus
}
#endif

#endif	//ifndef _BAL_OS_H
