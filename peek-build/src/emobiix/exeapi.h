/*=================================================================================

	HEADER NAME : exeapi.h
	MODULE NAME : BAL
	
	PRE-INCLUDE FILES DESCRIPTION
	
	
GENERAL DESCRIPTION
    This file contains function prototypes and type definitions
    for the Executive services. These services are based on 
    Nucleus RTOS functions.

BYD TECHFAITH Wireless Confidential Proprietary
(c) Copyright 2008 by BYD TECHFAITH Wireless. All Rights Reserved.
===================================================================================
Revision History
Modification Tracking
Date Author Number Description of changes
---------- ------------ --------- --------------------------------------------


	
=================================================================================*/

#ifndef EXEAPI_H
#define EXEAPI_H

#include "typedefs.h"
#include "header.h"

typedef char bool;

#ifndef BASIC_TYPES
#define BASIC_TYPES
typedef signed   char  int8;
typedef unsigned char  uint8;
typedef signed   short int16;
typedef unsigned short uint16;
typedef signed   int   int32;
typedef unsigned long  uint32;
#endif

typedef NU_SEMAPHORE      ExeSemaphoreT;
typedef NU_PARTITION_POOL ExeBufferT;
typedef NU_TIMER          ExeTimerT;
typedef NU_HISR           ExeHisrT;

/* Message Id's */
#define HW_KEY_MSG		0x2BC

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

typedef enum
{                 
   EXE_MAILBOX_1_ID  = 0x00,
   EXE_MAILBOX_2_ID  = 0x01,
   EXE_MAILBOX_3_ID  = 0x02,
   EXE_MAILBOX_4_ID  = 0x03,
   EXE_MAILBOX_5_ID  = 0x04,
   EXE_NUM_MAILBOX
} ExeMailboxIdT;   

/* Define constants for ExeMsgBuffer routines */
/* Note: message buffer size suppose to be even to be compatible with IPC interface*/
#define EXE_SIZE_MSG_BUFF_1     32   /* Size of msg data buffer 1 in bytes */
#define EXE_SIZE_MSG_BUFF_2     192  /* Size of msg data buffer 2 in bytes */
#define EXE_SIZE_MSG_BUFF_3     448  /* Size of msg data buffer 3 in bytes */
#define EXE_SIZE_MSG_BUFF_4     564  /* Size of msg data buffer 4 in bytes */
#define EXE_NUM_MSG_BUFF_1      90   /* Number of msg data buffers type 1, dengxd modified it from 45 to 90 on 20080508 */
#define EXE_NUM_MSG_BUFF_2     100   /* Number of msg data buffers type 2  */
#define EXE_NUM_MSG_BUFF_3      15   /* Number of msg data buffers type 3  */
#define EXE_NUM_MSG_BUFF_4      8    /* Number of msg data buffers type 4  */

/* Define all unit fault code numbers */
typedef enum
{
   MON_CP_FAULT_UNIT   = 0x00,
   MON_DBM_FAULT_UNIT  = 0x01,
   MON_EXE_FAULT_UNIT  = 0x02,
   MON_IPC_FAULT_UNIT  = 0x03,
   MON_IOP_FAULT_UNIT  = 0x04,
   MON_L1D_FAULT_UNIT  = 0x05,
   MON_LMD_FAULT_UNIT  = 0x06,
   MON_MON_FAULT_UNIT  = 0x07,
   MON_PSW_FAULT_UNIT  = 0x08,
   MON_HWD_FAULT_UNIT  = 0x09,
   MON_TST_FAULT_UNIT  = 0x0A,
   MON_BAL_FAULT_UNIT  = 0x0B,
   MON_SYS_FAULT_UNIT  = 0x0C,
   MON_RLP_FAULT_UNIT  = 0x0D,
   MON_HLW_FAULT_UNIT  = 0x0E,
   MON_AIW_FAULT_UNIT  = 0x0F,
   MON_PDE_FAULT_UNIT  = 0x10,
   MON_L1A_FAULT_UNIT  = 0x11,
   MON_UIM_FAULT_UNIT  = 0x12,
   MON_UI_FAULT_UNIT   = 0x13,
   MON_FSM_FAULT_UNIT  = 0x14,
   MON_BREW_FAULT_UNIT = 0x18,
   MON_NUM_FAULT_UNIT,  
   MON_CP_BOOT_FAULT_UNIT = 0x20, /* these 2 are only used by boot code and therefore  */
   MON_BOOT_FAULT_UNIT    = 0x21  /* should not be accounted for in MON_NUM_FAULT_UNIT */
} MonFaultUnitT;


typedef enum
{
   MON_CONTINUE  = 0x00,
   MON_HALT      = 0x01
} MonFaultTypeT;

/*--------------------------------------------------------------------
* Define Exe message buffer types and the msg info data structure
*--------------------------------------------------------------------*/

typedef enum
{
   EXE_MSG_BUFF_TYPE_1,             /* small                           */
   EXE_MSG_BUFF_TYPE_2,             /* medium                          */
   EXE_MSG_BUFF_TYPE_3,             /* large                           */
   EXE_MSG_BUFF_TYPE_4,             /* super large                     */
   EXE_NUM_DIFF_MSG_BUFFS           /* Number of different msg buffers */
} ExeMsgBuffTypeT;

typedef struct 
{
    uint32       BuffSize;
    ExeBufferT  *BuffCbP;
} ExeMsgBuffInfoT;

typedef struct
{
   int32           NumMsgs;
   int32           NumMsgsInQueue[EXE_NUM_MAILBOX];
   NU_QUEUE        MailQueueCb[EXE_NUM_MAILBOX];
   NU_TASK         TaskCb;
   NU_EVENT_GROUP  EventGroupCb;
} ExeTaskCbT;

/* Size of mailbox queue record in unit32 data elements */
#define EXE_MAIL_QUEUE_REC_SIZE  3 

#define BAL_TASK_MAIL_QUEUE_1       30 * EXE_MAIL_QUEUE_REC_SIZE
#define BAL_TASK_MAIL_QUEUE_2       30 * EXE_MAIL_QUEUE_REC_SIZE
#define UI_TASK_MAIL_QUEUE_1        30 * EXE_MAIL_QUEUE_REC_SIZE
#define UI_TASK_MAIL_QUEUE_2        60 * EXE_MAIL_QUEUE_REC_SIZE
#define UI_TASK_MAIL_QUEUE_3        30 * EXE_MAIL_QUEUE_REC_SIZE
#define UI_TASK_MAIL_QUEUE_4        3 * EXE_MAIL_QUEUE_REC_SIZE

/* 
   Define the mail queue structure referred to by the task 
   initialization structure. Each mail queue is composed of 
   a size and a mailbox id with which the queue is associated.
*/
typedef struct 
{
   uint32 Size;      /* Mail queue size */
   uint32 MailboxId; /* Mailbox id to associate the queue with */
}MailQueueT;         //added by wangran

typedef enum
{                 
   SYS_FIQ_INT  = 0x40,                 
   SYS_IRQ_INT  = 0x80,
   SYS_ALL_INT  = SYS_IRQ_INT | SYS_FIQ_INT                 
} SysIntT;

//#include "cp_nucleus.h"
//#include "sysdefs.h"

/*--------------------------------------------------------------------
* Define Exe constants
*--------------------------------------------------------------------*/

/* Number of Msec per timer tick */
#define EXE_TIMER_TICK         5             

/*--------------------------------------------------------------------
* Define Exe macros
*--------------------------------------------------------------------*/

/* Cal timer ticks from Msec */
//#define ExeCalMsec(NumMsec) (((NumMsec) / EXE_TIMER_TICK) + 1)
#define ExeCalMsec(NumMsec) ((((NumMsec) * 200) + 462) / 923)

/*--------------------------------------------------------------------
* Define Exe typedefs
*--------------------------------------------------------------------*/

/*---------------------------------------------------------------------
* The enumerated type ExeTaskIdT is used by application tasks
* when they want to call a scheduler service and they need to
* know the task id of a particular task. This enumerated type 
* also contains the number of overall tasks that the scheduler 
* will deal with. The order of task ids is important and must
* match the order of task control blocks defined in ExeTaskCbT.
* New task ids should be added to the end of the list before
* the EXE_NUM_TASKS entry.
*---------------------------------------------------------------------*/

typedef enum
{                 
   EXE_IPC_ID      = 0,
   EXE_IOP_ID      = 1,
   SPARE1          = 2,
   EXE_L1D_MDM_ID  = 3,
   EXE_MON_ID      = 4,
   EXE_MON_IDL_ID  = 5,
   EXE_HWD_ID      = 6,
   EXE_DBM_ID      = 7,
   EXE_TST_ID      = 8,
   EXE_PSW_ID      = 9,
   EXE_LMD_ID      = 10,
   EXE_LMD_S_ID    = 11,
   EXE_BAL_ID      = 12,
   EXE_L1D_AMPS_ID = 13,
   EXE_RLP_ID      = 14,
   EXE_HLW_ID      = 15,
   EXE_AIW_ID      = 16,
   SPARE2          = 17,
   EXE_PSW_S_ID	   = 18,
   SPARE3          = 19,
   EXE_PDE_ID      = 20,
   EXE_UI_ID       = 21,
   EXE_UIM_ID      = 22,
   EXE_FSM_ID      = 23,
   EXE_BM_ID       = 24,
   EXE_LSM_ID      = 25,
   EXE_PST_ID      = 26,
   EXE_UI_S_ID     = 27,
      /* Dummy task is an explicit way of indicating when a task ID is
    * not valid or should not be used.
    * It is set to the max application task id value, one less than
    * the LISR thread IDs.
    */
   EXE_BREW_ID = 28,
   EXE_EM_S_ID = 29,  //added by Randolph Wang 080219
   EXE_AMDB_S_ID = 30,  //added by Randolph Wang 080219
   EXE_NUM_TASKS,
   EXE_DUMMY_TASK

} ExeTaskIdT;

/*------------------------------------------------------------------------
* Define starting range for LISR & HISR thread Ids. If these defines are 
* changed they must also be changed in exeapi.inc
*------------------------------------------------------------------------*/

#define EXE_LISR_START_THREAD_ID      0x40
#define EXE_HISR_START_THREAD_ID      0x80

/*---------------------------------------------------------------------
* The enumerated type ExeLisrIdT is used by LISR to associated a thread
* ID with a LISR. If these defines are changed they must also be changed 
* in exeapi.inc
*---------------------------------------------------------------------*/

typedef enum
{                 
   EXE_TIMER_LISR_THREAD_ID            = EXE_LISR_START_THREAD_ID,
   EXE_UART0_LISR_THREAD_ID            = EXE_LISR_START_THREAD_ID + 0X01,
   EXE_CTS_LISR_THREAD_ID              = EXE_LISR_START_THREAD_ID + 0X02,
   EXE_DSPM_CTL_MBOX_LISR_THREAD_ID    = EXE_LISR_START_THREAD_ID + 0X03,
   EXE_DSPM_DBUF_MBOX_LISR_THREAD_ID   = EXE_LISR_START_THREAD_ID + 0X04,
   EXE_DSPV_MBOX_LISR_THREAD_ID        = EXE_LISR_START_THREAD_ID + 0X05,
   EXE_VITERBI_LISR_THREAD_ID          = EXE_LISR_START_THREAD_ID + 0X06,
   EXE_DSPV_F_MBOX_LISR_THREAD_ID      = EXE_LISR_START_THREAD_ID + 0X07,
   EXE_FAST_MBOX_B_LISR_THREAD_ID      = EXE_LISR_START_THREAD_ID + 0X08,
   EXE_KEYPAD_LISR_THREAD_ID           = EXE_LISR_START_THREAD_ID + 0X09,
   EXE_FAST_DAI_I_LISR_THREAD_ID       = EXE_LISR_START_THREAD_ID + 0X0A, 
   EXE_RESYNC_LISR_THREAD_ID           = EXE_LISR_START_THREAD_ID + 0x0B,
   EXE_VITERBI_SUP_LISR_THREAD_ID      = EXE_LISR_START_THREAD_ID + 0X0C,
   EXE_SLEEP_OVER_LISR_THREAD_ID       = EXE_LISR_START_THREAD_ID + 0X0D,
   EXE_SSTIMER_LISR_THREAD_ID          = EXE_LISR_START_THREAD_ID + 0x0E,
   EXE_FWD_DATA_LISR_THREAD_ID         = EXE_LISR_START_THREAD_ID + 0x0F,
   EXE_BIS_LISR_THREAD_ID              = EXE_LISR_START_THREAD_ID + 0x10,
   EXE_EOC_LISR_THREAD_ID              = EXE_LISR_START_THREAD_ID + 0x11,
   EXE_RNGR_LISR_THREAD_ID             = EXE_LISR_START_THREAD_ID + 0x12,
   EXE_UART1_LISR_THREAD_ID            = EXE_LISR_START_THREAD_ID + 0x13,
   EXE_I2C_LISR_THREAD_ID              = EXE_LISR_START_THREAD_ID + 0x14,
   EXE_SYS_TIME_LISR_THREAD_ID         = EXE_LISR_START_THREAD_ID + 0X15,
   EXE_UART2_LISR_THREAD_ID            = EXE_LISR_START_THREAD_ID + 0x16,
   EXE_UIMRX_LISR_THREAD_ID            = EXE_LISR_START_THREAD_ID + 0x18,
   EXE_UIMTX_LISR_THREAD_ID            = EXE_LISR_START_THREAD_ID + 0x19,
   EXE_M2CFM_FIQ_THREAD_ID             = EXE_LISR_START_THREAD_ID + 0x1A,
   EXE_M2CFM_LISR_THREAD_ID            = EXE_LISR_START_THREAD_ID + 0x1B,
   EXE_USB_LISR_THREAD_ID              = EXE_LISR_START_THREAD_ID + 0x1F,
   EXE_RINGER_LISR_THREAD_ID           = EXE_LISR_START_THREAD_ID + 0x21,
   EXE_RINGER_FIQISR_THREAD_ID         = EXE_LISR_START_THREAD_ID + 0x22,
   EXE_IRAMWF_LISR_THREAD_ID           = EXE_LISR_START_THREAD_ID + 0x23,
   EXE_SHARED_MEM_LISR_THREAD_ID       = EXE_LISR_START_THREAD_ID + 0x24,

    EXE_GPINT_LISR_THREAD_ID            = EXE_LISR_START_THREAD_ID + 0x25,
    EXE_GPINTX_LISR_THREAD_ID           = EXE_LISR_START_THREAD_ID + 0x26,
   /* MUST BE LAST IN LIST!! */
    EXE_LAST_LISR_THREAD_ID             = EXE_LISR_START_THREAD_ID + 0x28
 } ExeLisrIdT;

/*---------------------------------------------------------------------
* The enumerated type ExeHisrIdT is used by application tasks when they
* create an HISR and a thread id needs to associated with the HISR.
* If these defines are changed they must also be changed in exeapi.inc
*---------------------------------------------------------------------*/

typedef enum
{                 
   EXE_TIMER_HISR_THREAD_ID            = EXE_HISR_START_THREAD_ID,
   EXE_UART0_HISR_THREAD_ID            = EXE_HISR_START_THREAD_ID + 0X01,
   EXE_DSPM_CTL_MBOX_HISR_THREAD_ID    = EXE_HISR_START_THREAD_ID + 0X02,
   EXE_DSPM_DBUF_MBOX_HISR_THREAD_ID   = EXE_HISR_START_THREAD_ID + 0X03,
   EXE_DSPV_MBOX_HISR_THREAD_ID        = EXE_HISR_START_THREAD_ID + 0X04,
   EXE_VITERBI_HISR_THREAD_ID          = EXE_HISR_START_THREAD_ID + 0X05,
   EXE_SYS_TIME_HISR_THREAD_ID         = EXE_HISR_START_THREAD_ID + 0X06,          
   EXE_TIME_CHG_HISR_THREAD_ID         = EXE_HISR_START_THREAD_ID + 0X07,          
   EXE_CTS0_HISR_THREAD_ID             = EXE_HISR_START_THREAD_ID + 0X08,          
   EXE_CTS1_HISR_THREAD_ID             = EXE_HISR_START_THREAD_ID + 0X09,          
   EXE_CTS2_HISR_THREAD_ID             = EXE_HISR_START_THREAD_ID + 0X0A,          
   EXE_CTS3_HISR_THREAD_ID             = EXE_HISR_START_THREAD_ID + 0X0B,          
   EXE_CTS4_HISR_THREAD_ID             = EXE_HISR_START_THREAD_ID + 0X0C,          
   EXE_CTS5_HISR_THREAD_ID             = EXE_HISR_START_THREAD_ID + 0X0D,          
   EXE_CTS6_HISR_THREAD_ID             = EXE_HISR_START_THREAD_ID + 0X0E,          
   EXE_KEYPAD_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0X0F,
   EXE_SLEEP_OVER_HISR_THREAD_ID       = EXE_HISR_START_THREAD_ID + 0X10,
   EXE_SPAGE_RESYNC_HISR_THREAD_ID     = EXE_HISR_START_THREAD_ID + 0x11,
   EXE_VITERBI_SUP_HISR_THREAD_ID      = EXE_HISR_START_THREAD_ID + 0X12,
   EXE_SSTIMER_HISR_THREAD_ID          = EXE_HISR_START_THREAD_ID + 0x13,
   EXE_FWD_DATA_HISR_THREAD_ID         = EXE_HISR_START_THREAD_ID + 0x14,
   EXE_BIS_HISR_THREAD_ID              = EXE_HISR_START_THREAD_ID + 0x15,
   EXE_RNGR_HISR_THREAD_ID             = EXE_HISR_START_THREAD_ID + 0x16,
   EXE_UART1_HISR_THREAD_ID            = EXE_HISR_START_THREAD_ID + 0x17,
   EXE_I2C_HISR_THREAD_ID              = EXE_HISR_START_THREAD_ID + 0x18,
   EXE_SYS_EVENT_HISR_THREAD_ID        = EXE_HISR_START_THREAD_ID + 0x19,          
   EXE_UART2_HISR_THREAD_ID            = EXE_HISR_START_THREAD_ID + 0x1A,
   EXE_M2CFM_HISR_THREAD_ID            = EXE_HISR_START_THREAD_ID + 0x1B,
    EXE_UIM_CMD_HISR_THREAD_ID          = EXE_HISR_START_THREAD_ID + 0x1C,
    EXE_USB_HISR_THREAD_ID              = EXE_HISR_START_THREAD_ID + 0x1D,
    EXE_RINGER_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x1E,
   EXE_SHARED_MEM_HISR_THREAD_ID       = EXE_HISR_START_THREAD_ID + 0x1F,
   
    EXE_GPINT0_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x21,
   EXE_GPINT1_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x22,
   EXE_GPINT2_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x23,
   EXE_GPINT3_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x24,
   EXE_GPINT4_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x25,
   EXE_GPINT5_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x26,
   EXE_GIINT0_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x27,
   EXE_GIINT1_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x28,
   EXE_GIINT2_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x29,
   EXE_GIINT3_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x2A,
   EXE_GIINT4_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x2B,
   EXE_GIINT5_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x2C,
   EXE_GIINT6_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x2D,
   EXE_GIINT7_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x2E,
   EXE_GIINT8_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x2F,
   EXE_GIINT9_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x30,
   EXE_GIINT10_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x31,
   EXE_GIINT11_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x32,
   EXE_GIINT12_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x33,
   EXE_GIINT13_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x34,
   EXE_GIINT14_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x35,
   EXE_MMAPPS_HISR_THREAD_ID           = EXE_HISR_START_THREAD_ID + 0x36
} ExeHisrIdT;

/*----------------------------------------------------------------------
* The following typedef defines the 3 event types (signal, mail and 
* time) and the four mailboxes and twenty task signal flags. An application 
* task can use these to determine which particular event, message or
* signal flag or timeout was set to cause their task to be scheduled 
* after calling ExeWaitEvent. 
*----------------------------------------------------------------------*/

typedef enum
{                 
   EXE_MESSAGE_TYPE  = 0x00000001,   /* Event types */           
   EXE_SIGNAL_TYPE   = 0x00000002,   
   EXE_TIMEOUT_TYPE  = 0x00000004,

   EXE_MAILBOX_1     = 0x00000008,   /* Mailbox types */
   EXE_MAILBOX_2     = 0x00000010,              
   EXE_MAILBOX_3     = 0x00000020,              
   EXE_MAILBOX_4     = 0x00000040,              
   EXE_MAILBOX_5     = 0x00000080,              
                         
   EXE_SIGNAL_1      = 0x00000100,   /* Signal types */
   EXE_SIGNAL_2      = 0x00000200,              
   EXE_SIGNAL_3      = 0x00000400,              
   EXE_SIGNAL_4      = 0x00000800,              
   EXE_SIGNAL_5      = 0x00001000,             
   EXE_SIGNAL_6      = 0x00002000,             
   EXE_SIGNAL_7      = 0x00004000,             
   EXE_SIGNAL_8      = 0x00008000,             
   EXE_SIGNAL_9      = 0x00010000,
   EXE_SIGNAL_10     = 0x00020000,             
   EXE_SIGNAL_11     = 0x00040000,             
   EXE_SIGNAL_12     = 0x00080000,             
   EXE_SIGNAL_13     = 0x00100000,             
   EXE_SIGNAL_14     = 0x00200000,             
   EXE_SIGNAL_15     = 0x00400000,             
   EXE_SIGNAL_16     = 0x00800000,             
   EXE_SIGNAL_17     = 0x01000000,             
   EXE_SIGNAL_18     = 0x02000000,             
   EXE_SIGNAL_19     = 0x04000000,             
   EXE_SIGNAL_20     = 0x08000000,
   EXE_SIGNAL_21     = 0x10000000,            
   EXE_SIGNAL_22     = 0x20000000,            
   EXE_SIGNAL_23     = 0x40000000            
} ExeEventWaitT;

/* Define Signal types of event wait */
#define EXE_SIGNAL_TRUE    TRUE
#define EXE_SIGNAL_FALSE   FALSE

/* Define timeout type of event wait */
#define EXE_TIMEOUT_FALSE  NU_SUSPEND

/* Define constant SemaphoreGet Timeout Values */
#define EXE_SEM_WAIT_FOREVER  NU_SUSPEND

/* Define SemaphoreGet Return Code Types
 *    The "ExeSemaphoreGet()" function will only return 1 of 2 codes: _SUCCESS or _TIMEOUT.
 *    It will signal a Nucleus error and NOT return to the caller if:
 *       a) the timeout value is EXE_SEM_WAIT_FOREVER and return value from Nucleus
 *          is not NU_SUCCESS, or
 *       b) the return value from Nucleus is anything other than NU_SUCCESS or NU_TIMEOUT.
 *    These 2 return codes MUST be checked against by the application tasks.
 *    Application tasks MUST call "ExeFaultSet(Status)" if "Status", the
 *       return code from "ExeSemaphoreGet()", is EXE_SEM_TIMEOUT and
 *       corrective actions cannot be taken.
 */
#define EXE_SEM_SUCCESS       NU_SUCCESS     /*   0 */
#define EXE_SEM_TIMEOUT       NU_TIMEOUT     /* -50 */

/* Define message types of event wait */
typedef enum
{                 
   EXE_MESSAGE_FALSE  = 0,
   EXE_MESSAGE_TRUE   = EXE_MESSAGE_TYPE,
   EXE_MESSAGE_MBOX_1 = EXE_MAILBOX_1,
   EXE_MESSAGE_MBOX_2 = EXE_MAILBOX_2,
   EXE_MESSAGE_MBOX_3 = EXE_MAILBOX_3,
   EXE_MESSAGE_MBOX_4 = EXE_MAILBOX_4,
   EXE_MESSAGE_MBOX_5 = EXE_MAILBOX_5
}ExeMessageT;

typedef uint32 ExeSignalT;

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
   BOS_UI_ID            = 21,
   BOS_EM_S_ID  = 29,  //added by Randolph Wang 080219
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

/* Define HISR priorities */
typedef enum
{                 
   EXE_HISR_PRIO_0 = 0x00,
   EXE_HISR_PRIO_1 = 0x01
} ExeHisrPrioT;

/*---------------------------------------------------------------------
* The enumerated type ExeMailboxIdT is used by application tasks
* when they want to send mail to a particular mailbox owned by 
* another task.
*---------------------------------------------------------------------*/
                           
/*---------------------------------------------------------------------
* The enumerated type ExePreemptionT is used by application tasks
* when they want to disable/enable the preemption posture of the
* currently executing task.
*---------------------------------------------------------------------*/
                           
typedef enum
{                 
   EXE_PREEMPTION_DISABLE = NU_NO_PREEMPT,
   EXE_PREEMPTION_ENABLE  = NU_PREEMPT
} ExePreemptionT;                           

typedef struct
{
   uint8          Id;
   uint8          Priority;
   uint8          Status;
   uint8          MboxMsgs[EXE_NUM_MAILBOX];
} ExeTaskStatusT;

typedef enum
{
    EXE_MSG_BUFF_MEM_EMPTY_ERR        = 0x00,
    EXE_MSG_BUFF_MEM_SIZE_ERR         = 0x01,
    EXE_NUCLEUS_ERR                   = 0x02,
    EXE_ILLEGAL_LISR_OPERATION_ERR    = 0x03,
    EXE_MSG_BUFF_OVERWRITE_ERR        = 0x04,
    EXE_PART_MEM_EMPTY_ERR            = 0x05,
    EXE_STACK_OVERFLOW_ERR            = 0x06,
    EXE_TIMER_CREATION_ERR            = 0x07,
    EXE_UNHANDLED_INT_ERR             = 0x08,
    EXE_MAIL_QUEUE_FULL_ERR           = 0x09
} ExeErrsT;

typedef struct
{
   ExeErrsT       ExeError;
   uint32         SysTime;
   uint8          SrcTaskId;
   uint8          Buff1Alloc;
   uint8          Buff2Alloc;
   uint8          Buff3Alloc;
   uint8          Buff4Alloc;
   uint16         MsgBuffSize;
   ExeTaskStatusT TaskStatus[EXE_NUM_TASKS];
} ExeFaultType2T;

/* Define EXE Fault type 3 msg structure */
typedef struct
{
   ExeErrsT       ExeError;
   uint32         SysTime;
   uint8          SrcTaskId;
   uint8          Buff1Alloc;
   uint8          Buff2Alloc;
   uint8          Buff3Alloc;
   uint8          Buff4Alloc;
   uint8          DestTaskId;
   uint8          MboxId;
   uint16         MsgId;
   uint16         MsgSize;
   ExeTaskStatusT TaskStatus[EXE_NUM_TASKS];
} ExeFaultType3T;

typedef enum
{
   EXE_FAULT_TYPE_1 = 0,
   EXE_FAULT_TYPE_2,
   EXE_FAULT_TYPE_3
} ExeFaultTypeT;


/*---------------------------------------------------------------------
* The structure ExeRspMsgT is used in the command-response paradigm
* for many of the tasks.  It is defined in the EXE API to give a single
* definition for this structure.
*---------------------------------------------------------------------*/

/*                           
typedef PACKED struct 
{
    ExeTaskIdT     TaskId;  
    ExeMailboxIdT  MailboxId;  
    uint32         MsgId; 
} ExeRspMsgT;
*/

/*****************************************************************************
 
  FUNCTION NAME: ExeEventWait                       

  DESCRIPTION:

    This routine is called by a task running under control of the exe
    scheduler when the task wishes to suspend itself until one or more
    events have occurred. This routine suspends a task on an signal flag,
    OR message OR timeout. The task remains suspended until one of the 
    specified events becomes true. If a task is waiting on an signal flag 
    it will be started if any of the twenty signal flags are set. If a 
    task is waiting on a message there is an option to wait on a message
    in a particular mailbox or a message in any of the task's mailboxes.

    CAUTION:

    All signal flags owned by a task are cleared by this call. So a
    task must process all signals returned in this call or they will 
    be lost.

  PARAMETERS:

    INPUTS:
    
    TaskId   - Task ID of task that wants to wait on an event
    Signal   - EXE_SIGNAL_TRUE to specify a wait on signal flag or 
               EXE_SIGNAL_FALSE for don't care.
    Message  - EXE_MESSAGE_FALSE for don't care
               EXE_MESSAGE_TRUE to specify a wait on a message in any mailbox
               EXE_MESSAGE_MBOX_1 to only wait on messages in mailbox 1
               EXE_MESSAGE_MBOX_2 to only wait on messages in mailbox 2
               EXE_MESSAGE_MBOX_3 to only wait on messages in mailbox 3
               EXE_MESSAGE_MBOX_4 to only wait on messages in mailbox 4
               EXE_MESSAGE_MBOX_5 to only wait on messages in mailbox 5
    Timeout  - Timeout associated with this call. EXE_TIMEOUT_FALSE
               for no timeout.

  RETURNED VALUES:

    A bit mapped flag indicating which events were set to cause this task 
    to run. Refer to ExeEventWaitT for the bit map assignments.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.

  TASKING CHANGES:

    This routine will cause the calling task to suspend.

*****************************************************************************/
#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeEventWait(TaskId, Signal, Message, Timeout) \
        __ExeEventWait(TaskId, Signal, Message, Timeout, __FILE__, __LINE__)

extern ExeEventWaitT __ExeEventWait(ExeTaskIdT TaskId, bool Signal, 
                                    ExeMessageT Message, uint32 Timeout,
                                    const char *Filename, unsigned Linenumber);

#else

extern ExeEventWaitT ExeEventWait(ExeTaskIdT TaskId, bool Signal, 
                                  ExeMessageT Message, uint32 Timeout);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */

/*****************************************************************************

 FUNCTION NAME: ExeEventRetireve

 DESCRIPTION:

   This routine is called by a task running under control of the exe
   scheduler when the task wishes to suspend itself until one or more
   events have occurred. This routine suspends a task on an signal flag,
   OR timeout. The task remains suspended until one of the specified 
   events becomes true. This routine diffent with ExeEventWait, you can
   wait a specific signal. it is only for BREW task till now

   CAUTION:

   All signal flags owned by a task are cleared by this call. So a
   task must process all signals returned in this call or they will
   be lost.

 PARAMETERS:

   INPUTS:

   TaskId   - Task ID of task that wants to wait on an event
   RequestEvent   - refer to ExeEventWaitT for bit map assignments
   Timeout  - Timeout associated with this call. EXE_TIMEOUT_FALSE
              for no timeout.

 RETURNED VALUES:

   A bit mapped flag indicating which events were set to cause this task
   to run. Refer to ExeEventWaitT for the bit map assignments.

 ERROR HANDLING:

   No error codes are returned. This routine handles all Nucleus errors.

 TASKING CHANGES:

   This routine will cause the calling task to suspend.

*****************************************************************************/
#ifdef SYS_DEBUG_FAULT_FILE_INFO
#define ExeEventRetireve(TaskId, RequestEvent, Timeout ) \
        __ExeEventRetireve( TaskId, RequestEvent, Timeout, __MODULE__, __LINE__)

ExeEventWaitT __ExeEventRetireve( ExeTaskIdT TaskId, uint32 RequestEvent, uint32 Timeout, const char *Filename, unsigned Linenumber);

#else

ExeEventWaitT ExeEventRetireve( ExeTaskIdT TaskId, uint32 RequestEvent, uint32 Timeout );

#endif /* SYS_DEBUG_FAULT_FILE_INFO */

/*****************************************************************************
 
extern void ExeTaskWait(uint32 Ticks);
  FUNCTION NAME: ExeSignalSet

  DESCRIPTION:

    This routine sets a signal flag of a particular task. Each task
    has twelve signal flags. The scheduler will clear the signal
    flag when the task is activated. 
    
  PARAMETERS:

    INPUTS:
    
    TaskId    - The task id of the task whose signal flag is to be set.
    SignalFlg - The number of the signal flag to set. Each task has
                twenty signal flags defined by ExeSignalT
              
  RETURNED VALUES:

    None

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
 
  TASKING CHANGES:
 
    This routine will suspend the calling task if the signal set causes
    a waiting higher priority task to be ready to run.

**************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeSignalSet(TaskId, SignalFlg) \
        __ExeSignalSet(TaskId, SignalFlg, __FILE__, __LINE__)

extern void __ExeSignalSet(ExeTaskIdT TaskId, ExeSignalT SignalFlg,
                           const char *Filename, unsigned Linenumber);

#else

extern void ExeSignalSet(ExeTaskIdT TaskId, ExeSignalT SignalFlg);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */


/*****************************************************************************
 
  FUNCTION NAME: ExeMsgSend

  DESCRIPTION:

    This routine sends a message into a particular task's mailbox. 
    The format of the message is defined by the receiver of the message.
    This message is added to the end of the message list belonging to the 
    mailbox specified by the mboxid parameter.

  PARAMETERS:

    INPUTS:
    
    TaskId       - The task id of the owner of the mailbox
    MailboxId    - The mailbox id of the mailbox receiving the message
    MsgId        - The id of message
    MsgBufferP   - The pointer to the message data buffer. Null if no data
    MsgSize      - The size of the message data in bytes. Zero if no data

  RETURNED VALUES:

    None.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    This routine will suspend the calling task if the message sent
    causes a waiting higher priority task to be ready to run.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeMsgSend(TaskId, MailboxId, MsgId, MsgBufferP, MsgSize) \
        __ExeMsgSend(TaskId, MailboxId, MsgId, MsgBufferP, MsgSize, __FILE__, __LINE__)

extern void __ExeMsgSend(ExeTaskIdT TaskId, ExeMailboxIdT MailboxId, uint32 MsgId, 
                         void *MsgBufferP, uint32 MsgSize,
                         const char *Filename, unsigned Linenumber);

#else

extern int ExeMsgSend(ExeTaskIdT TaskId, ExeMailboxIdT MailboxId, uint32 MsgId, 
                       void *MsgBufferP, uint32 MsgSize);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */

/*****************************************************************************
 
  FUNCTION NAME: ExeMsgSendToFront

  DESCRIPTION:

    This routine sends a message into a particular task's mailbox. 
    The format of the message is defined by the receiver of the message.
    This message is added to the front of the message list belonging to the 
    mailbox specified by the mboxid parameter.

  PARAMETERS:

    INPUTS:
    
    TaskId       - The task id of the owner of the mailbox
    MailboxId    - The mailbox id of the mailbox receiving the message
    MsgId        - The id of message
    MsgBufferP   - The pointer to the message data buffer. Null if no data
    MsgSize      - The size of the message data in bytes. Zero if no data

  RETURNED VALUES:

    None.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    This routine will suspend the calling task if the message sent
    causes a waiting higher priority task to be ready to run.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeMsgSendToFront(TaskId, MailboxId, MsgId, MsgBufferP, MsgSize) \
        __ExeMsgSendToFront(TaskId, MailboxId, MsgId, MsgBufferP, MsgSize, __FILE__, __LINE__)

extern void __ExeMsgSendToFront(ExeTaskIdT TaskId, ExeMailboxIdT MailboxId, 
                uint32 MsgId, void *MsgBufferP, uint32 MsgSize, const char *Filename, unsigned 
                Linenumber);

#else

extern int ExeMsgSendToFront(ExeTaskIdT TaskId, ExeMailboxIdT MailboxId, uint32 
MsgId, void *MsgBufferP, uint32 MsgSize);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */

/*****************************************************************************
 
  FUNCTION NAME: ExeMsgRead

  DESCRIPTION:

    This routine removes a message from the head of a message list
    belonging to the mailbox specified in this call. The MsgId variable 
    is filled in with the appropriate data. This routine also returns a 
    pointer to the data buffer associated with this message. 

  CAUTION:

    Only one task should read from any given mailbox.

  PARAMETERS:

    INPUTS:

    TaskId     - The task id of the owner of the mailbox
    MailboxId  - The mailbox id of the mailbox to check the mail from.
  
    OUTPUTS:

    MsgIdP      - Message id variable is updated
	 MsgBufferP  - Message data buffer ptr is updated, NULL if no data
	 MsgSizeP    - Message data size variable is updated, zero if no data

  RETURNED VALUES:

    bool       - Returns status indicating if the mailbox was full/empty 
                 TRUE = message read, FALSE mailbox empty.
 
  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeMsgRead(TaskId, MailboxId, MsgIdP, MsgBufferP, MsgSizeP) \
        __ExeMsgRead(TaskId, MailboxId, MsgIdP, MsgBufferP, MsgSizeP, __FILE__, __LINE__)

extern bool __ExeMsgRead(ExeTaskIdT TaskId, ExeMailboxIdT MailboxId, uint32 *MsgIdP, 
                         void **MsgBufferP, uint32 *MsgSizeP, 
                         const char *Filename, unsigned Linenumber);

#else

extern bool ExeMsgRead(ExeTaskIdT TaskId, ExeMailboxIdT MailboxId, uint32 *MsgIdP, 
                       void **MsgBufferP, uint32 *MsgSizeP);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */


/*****************************************************************************
 
  FUNCTION NAME: ExeMsgCheck

  DESCRIPTION:

    This routine checks a task's mailbox by returning the number of
    mail messages in the mailbox.

  CAUTION:

    Only one task should read from any given mailbox.

  PARAMETERS:

    INPUTS:

    TaskId     - The task id of the owner of the mailbox
    MailboxId  - The mailbox id of the mailbox to check the mail from.
  
  RETURNED VALUES:

    uint32     - Returns the number of mail messages in the mailbox
 
  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/

extern uint32 ExeMsgCheck(ExeTaskIdT TaskId, ExeMailboxIdT MailboxId);


/*****************************************************************************
 
  FUNCTION NAME: ExeMsgBufferGet

  DESCRIPTION:

    Attempts to allocate a block of memory of the specified size from a 
    semi-dynamic memory pool and returns the address of the allocated block.

  PARAMETERS:

    INPUTS:
    
    MsgBufferSize   - The size of a msg memory buffer (in bytes) needed.

  RETURNED VALUES:

    void * - A pointer to the memory buffer

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeMsgBufferGet(MsgBufferSize) \
        __ExeMsgBufferGet(MsgBufferSize, __FILE__, __LINE__)

extern void * __ExeMsgBufferGet(uint32 MsgBufferSize,
                                const char *Filename, unsigned Linenumber);
#else

extern void * ExeMsgBufferGet(uint32 MsgBufferSize);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */


/*****************************************************************************
 
  FUNCTION NAME: ExeMsgBufferFree

  DESCRIPTION:

    This routine deallocates a variable length block of memory back
    to the semi-dynamic memory pool. The memory being deallocated must have
    been allocated previously by a call to ExeMsgBufferGet.

  PARAMETERS:
      
    INPUTS:
    
    MsgBufferP   - A pointer to a msg memory buffer.

  RETURNED VALUES:

    No error codes are returned. This routine handles all Nucleus errors.

  TASKING CHANGES:

    None.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeMsgBufferFree(MsgBufferP) \
        __ExeMsgBufferFree(MsgBufferP, __FILE__, __LINE__)

extern void __ExeMsgBufferFree(void *MsgBufferP,
                             const char *Filename, unsigned Linenumber);

#else

extern void ExeMsgBufferFree(void *MsgBufferP);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */


/*****************************************************************************
 
  FUNCTION NAME: ExeTimerCreate

  DESCRIPTION:

    This routine creates a an application timer. The specified expiration
    routine is executed each time the timer expires. Application expiration
    routines should avoid using task suspension calls since this can cause
    delays in other application timer requests. The application timer is
    created in the disable mode so that a call to ExeTimerStart() is need 
    to start the timer.

  PARAMETERS:
      
    INPUTS:
    
    TimerCbP       - A pointer to a timer control block
    Routine        - Expiration routine to be called when timer expires.
    TimerId        - Timer ID passed into expiration routine
    InitialTime    - Specifies the initial number of timer ticks for the
                     timer expiration.
    RescheduleTime - Specifies the number of timer ticks for expiration
                     after the first expiration. If this parameter is zero,
                     the timer only expires once.

  RETURNED VALUES:

    None.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeTimerCreate(TimerCbP, Routine, TimerId, InitialTime, RescheduledTime) \
        __ExeTimerCreate(TimerCbP, Routine, TimerId, InitialTime, RescheduledTime, __FILE__, __LINE__)

extern void __ExeTimerCreate(ExeTimerT *TimerCbP, void (*Routine)(uint32), uint32 TimerId,
                             uint32 InitialTime, uint32 RescheduledTime,
                             const char *Filename, unsigned Linenumber);
#else

extern void ExeTimerCreate(ExeTimerT *TimerCbP, void (*Routine)(uint32), uint32 TimerId,
                           uint32 InitialTime, uint32 RescheduledTime);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */


/*****************************************************************************
 
  FUNCTION NAME: ExeTimerStart

  DESCRIPTION:

    This routine starts a previously created application timer.

  PARAMETERS:
      
    INPUTS:
    
    TimerCbP - A pointer to a timer control block

  RETURNED VALUES:

    None.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

******************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeTimerStart(TimerCbP) \
        __ExeTimerStart(TimerCbP, __FILE__, __LINE__)

extern void __ExeTimerStart(ExeTimerT *TimerCbP,
                            const char *Filename, unsigned Linenumber);

#else

extern void ExeTimerStart(ExeTimerT *TimerCbP);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */

/*****************************************************************************
 
  FUNCTION NAME: ExeTimerStop

  DESCRIPTION:

    This routine stops a previously created application timer. 
    
  CAUTIONS:
  
    Once a timer has been stopped it must be reset using the ExeTimerReset()
    routine before it is started again.

  PARAMETERS:
      
    INPUTS:
    
    TimerCbP - A pointer to a timer control block

  RETURNED VALUES:

    None.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeTimerStop(TimerCbP) \
        __ExeTimerStop(TimerCbP, __FILE__, __LINE__)

extern void __ExeTimerStop(ExeTimerT *TimerCbP,
                           const char *Filename, unsigned Linenumber);

#else

extern void ExeTimerStop(ExeTimerT *TimerCbP);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */


/*****************************************************************************
 
  FUNCTION NAME: ExeTimerReset

  DESCRIPTION:

    This routine resets a previously created application timer. The 
    specified expiration routine is executed each time the timer expires. 
    The application timer is placed in the disable mode so that a call to 
    ExeTimerStart() is need to start the timer.

  PARAMETERS:
      
    INPUTS:
    
    TimerCbP       - A pointer to a timer control block
    Routine        - Expiration routine to be called when timer expires.
    InitialTime    - Specifies the initial number of timer ticks for the
                     timer expiration.
    RescheduleTime - Specifies the number of timer ticks for expiration
                     after the first expiration. If this parameter is zero,
                     the timer only expires once.

  RETURNED VALUES:

    None.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeTimerReset(TimerCbP, Routine, InitialTime, RescheduledTime) \
        __ExeTimerReset(TimerCbP, Routine, InitialTime, RescheduledTime, __FILE__, __LINE__)

extern void __ExeTimerReset(ExeTimerT *TimerCbP, void (*Routine)(uint32),
                            uint32 InitialTime, uint32 RescheduledTime,
                            const char *Filename, unsigned Linenumber);
#else

extern void ExeTimerReset(ExeTimerT *TimerCbP, void (*Routine)(uint32),
                          uint32 InitialTime, uint32 RescheduledTime);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */


/*****************************************************************************
 
  FUNCTION NAME: ExeTimerAdjust

  DESCRIPTION:

    This routine adjusts the active timer list by the number of timer ticks
    passed to this routine. The adjust time passed in to this routine is
    subtracted from the active timer list.

  PARAMETERS:
      
    INPUTS:
    
    AdjustTime  - Specifies the amount of time (in timer ticks) that the
                  active list of timers should be adjusted by.

  RETURNED VALUES:

    None.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeTimerAdjust(AdjustTime) \
        __ExeTimerAdjust(AdjustTime, __FILE__, __LINE__)

extern void __ExeTimerAdjust(uint32 AdjustTime,
                             const char *Filename, unsigned Linenumber);

#else

extern void ExeTimerAdjust(uint32 AdjustTime);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */


/*****************************************************************************
 
  FUNCTION NAME: ExeSemaphoreCreate

  DESCRIPTION:

    This routine creates a counting semaphore. Semaphore values range from 0 
    to 4,294,967,294.

  PARAMETERS:
      
    INPUTS:
       
       SemaphoreCbP - A pointer to a semaphore control block
       InitialCount - Initial count of semaphore

  RETURNED VALUES:

    None.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:
 
    None.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeSemaphoreCreate(SemaphoreCbP, InitialCount) \
        __ExeSemaphoreCreate(SemaphoreCbP, InitialCount, __FILE__, __LINE__)

extern void __ExeSemaphoreCreate(ExeSemaphoreT *SemaphoreCbP, uint32 InitialCount,
                                 const char *Filename, unsigned Linenumber);

#else

extern void ExeSemaphoreCreate(ExeSemaphoreT *SemaphoreCbP, uint32 InitialCount);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */


/*****************************************************************************
 
  FUNCTION NAME: ExeSemaphoreDelete

  DESCRIPTION:

    This routine deleates a counting semaphore. Semaphore values range from 0 
    to 4,294,967,294.

  PARAMETERS:
      
    INPUTS:
       
       SemaphoreCbP - A pointer to a semaphore control block

  RETURNED VALUES:

    None.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:
 
    None.

*****************************************************************************/
#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeSemaphoreDelete(SemaphoreCbP) \
         __ExeSemaphoreDelete(SemaphoreCbP, __FILE__, __LINE__)

void __ExeSemaphoreDelete(ExeSemaphoreT *SemaphoreCbP,
                         const char *Filename, unsigned Linenumber);

#else

void ExeSemaphoreDelete(ExeSemaphoreT *SemaphoreCbP);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */

/*****************************************************************************
 
  FUNCTION NAME: ExeSemaphoreGet

  DESCRIPTION:

    This routine gets an instance of a specific semaphore. This translates
    into decrementing the semaphore's internal counter by one. If the
    semaphore counter is zero before this call, the service can not be
    immediately satisfied and the calling task will be suspended. If a
    timeout is specified and the calling task is suspended for a time
    greater then Timeout then this routine will return an error.

  PARAMETERS:
      
    INPUTS:
    
    SemaphoreCbP  - A pointer to a semaphore control block 
    Timeout       - Timeout associated with this call.  Use EXE_SEM_WAIT_FOREVER
                    for no timeout.

  RETURNED VALUES:

    EXE_SEM_SUCCESS - SemaphoreGet was successful
    EXE_SEM_TIMEOUT - SemaphoreGet timed out

  ERROR HANDLING:

    This routine will signal a Nucleus error and NOT return to the caller if:

      a) the timeout value is EXE_SEM_WAIT_FOREVER and return value from Nucleus
            is not NU_SUCCESS, or
      b) the return value from Nucleus is neither NU_SUCCESS or NU_TIMEOUT.

  TASKING CHANGES:

    If the semaphore counter is zero (in locked state) before this call, 
    the service can not be immediately satisfied and the calling task will 
    be suspended.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeSemaphoreGet(SemaphoreCbP, Timeout) \
        __ExeSemaphoreGet(SemaphoreCbP, Timeout, __FILE__, __LINE__)

extern int32 __ExeSemaphoreGet(ExeSemaphoreT *SemaphoreCbP, uint32 Timeout,
                              const char *Filename, unsigned Linenumber);

#else

extern int32 ExeSemaphoreGet(ExeSemaphoreT *SemaphoreCbP, uint32 Timeout);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */

/*****************************************************************************
 
  FUNCTION NAME: ExeSemaphoreGetWithTimeout

  DESCRIPTION:

    This routine gets an instance of a specific semaphore. This translates
    into decrementing the semaphore's internal counter by one. If the
    semaphore counter is zero before this call, the service can not be
    immediately satisfied and the calling task will be suspended. If a
    timeout is specified and the calling task is suspended for a time
    greater then Timeout then this routine will return an error.
    
    This routine differs from ExeSemaphoreGet in the fact that it returns
    on a timeout rather than declaring a critical fault and not returning.

  PARAMETERS:
      
    INPUTS:
    
    SemaphoreCbP  - A pointer to a semaphore control block 
    Timeout       - Timeout associated with this call. EXE_TIMEOUT_FALSE
                    for no timeout.

  RETURNED VALUES:

    EXE_SEM_SUCCESS - SemaphoreGet was successful
    EXE_SEM_TIMEOUT - SemaphoreGet timed out

  ERROR HANDLING:

    This routine will signal a Nucleus error and NOT return to the caller if:

      a) the timeout value is EXE_SEM_WAIT_FOREVER and return value from Nucleus
            is not NU_SUCCESS, or
      b) the return value from Nucleus is neither NU_SUCCESS or NU_TIMEOUT.

  TASKING CHANGES:

    If the semaphore counter is zero (in locked state) before this call, 
    the service can not be immediately satisfied and the calling task will 
    be suspended.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeSemaphoreGetWithTimeout(SemaphoreCbP, Timeout) \
        __ExeSemaphoreGetWithTimeout(SemaphoreCbP, Timeout, __FILE__, __LINE__)

extern int32 __ExeSemaphoreGetWithTimeout(ExeSemaphoreT *SemaphoreCbP, uint32 Timeout,
                              const char *Filename, unsigned Linenumber);

#else

extern int32 ExeSemaphoreGetWithTimeout(ExeSemaphoreT *SemaphoreCbP, uint32 Timeout);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */

/*****************************************************************************
 
  FUNCTION NAME: ExeSemaphoreRelease

  DESCRIPTION:

    This routine releases an instance of a specific semaphore. This 
    translates into incrementing the semaphore's internal counter by one. 

  PARAMETERS:
      
    INPUTS:
    
    SemaphoreCbP   -  A pointer to a semaphore control block 

  RETURNED VALUES:

    None.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    This routine will suspend the calling task if the released semaphore
    causes a waiting higher priority task to be ready to run.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeSemaphoreRelease(SemaphoreCbP) \
        __ExeSemaphoreRelease(SemaphoreCbP, __FILE__, __LINE__)

extern void __ExeSemaphoreRelease(ExeSemaphoreT *SemaphoreCbP,
                                const char *Filename, unsigned Linenumber);

#else

extern void ExeSemaphoreRelease(ExeSemaphoreT *SemaphoreCbP);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */

/*****************************************************************************
 
  FUNCTION NAME: ExeGetSemaphoreCount

  DESCRIPTION:

    This routine returns the number of currently available semaphors in the pool 

  PARAMETERS:
      
    INPUTS:
    
    SemaphoreCbP   -  A pointer to a semaphore control block 

  RETURNED VALUES:

    None.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    This routine will suspend the calling task if the released semaphore
    causes a waiting higher priority task to be ready to run.

*****************************************************************************/
#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeGetSemaphoreCount(SemaphoreCbP) \
        __ExeGetSemaphoreCount(SemaphoreCbP, __FILE__, __LINE__)

uint32 __ExeGetSemaphoreCount(ExeSemaphoreT *SemaphoreCbP,
                         const char *Filename, unsigned Linenumber);

#else

uint32 ExeGetSemaphoreCount(ExeSemaphoreT *SemaphoreCbP);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */

/*****************************************************************************
 
  FUNCTION NAME: ExeMemoryPoolCreate

  DESCRIPTION:

    This routine creates a nucleas memory pool based on the 
    previousely allocated buffer.

  PARAMETERS:
      
    INPUTS:
    
    *pool - assigned nucleas pool ID
    *name - pool name
    *start_address - start address of the pre-allocated buffer
    pool_size - size of the buffer
    min_allocation - min size of the allocation from the pool

  RETURNED VALUES:

    None.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/
#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeMemoryPoolCreate(pool, name, start_address, pool_size, min_allocation) \
        __ExeMemoryPoolCreate(pool, name, \
        start_address, pool_size, min_allocation, __FILE__, __LINE__)

extern void __ExeMemoryPoolCreate(NU_MEMORY_POOL *pool, char *name, 
                           void *start_address, uint32 pool_size,
                           uint32 min_allocation,
                           const char *Filename, unsigned Linenumber); 

#else
extern void ExeMemoryPoolCreate(NU_MEMORY_POOL *pool, char *name, 
                        void *start_address, uint32 pool_size,
                        uint32 min_allocation);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */

/*****************************************************************************
 
  FUNCTION NAME: ExeMemoryPoolCreate

  DESCRIPTION:

    This routine deletes a nucleas memory pool 

  PARAMETERS:
      
    INPUTS:
    
    NU_MEMORY_POOL *pool - nucleas pool ID

  RETURNED VALUES:

    None.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeMemoryPoolDelete(pool) \
        __ExeMemoryPoolDelete(pool, __FILE__, __LINE__)

extern void __ExeMemoryPoolDelete(NU_MEMORY_POOL *pool,
                           const char *Filename, unsigned Linenumber);

#else
extern void ExeMemoryPoolDelete(NU_MEMORY_POOL *pool);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */


/*****************************************************************************
 
  FUNCTION NAME: ExeMalloc

  DESCRIPTION:

    This routine allocates a buffer from any memory pool.

  PARAMETERS:
      
    INPUTS:
    
    pool    - pool ID; if pool==NULL use ExeSystemMemory pool
    size    - The size in bytes of the requested memory.
    suspend - suspend/not suspend option

  RETURNED VALUES:

    The pointer of the allocated buffer.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/
#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeMalloc(pool, size, suspend) \
        __ExeMalloc(pool, size, suspend, __FILE__, __LINE__)

extern void *__ExeMalloc(NU_MEMORY_POOL *pool, uint32 size, uint32 suspend,
                         const char *Filename, unsigned Linenumber);
                   
#else
                   
extern void *ExeMalloc(NU_MEMORY_POOL *pool, uint32 size, uint32 suspend);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */


/*****************************************************************************
 
  FUNCTION NAME: ExeFree

  DESCRIPTION:

    This routine returns a buffer to the system memory pool.

  PARAMETERS:
      
    INPUTS:
    
    ptr    - The pointer of the memory to be returned.

  RETURNED VALUES:

    None.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeFree(ptr) \
        __ExeFree(ptr, __FILE__, __LINE__)

extern void __ExeFree(void * ptr,
                              const char *Filename, unsigned Linenumber);

#else

extern void ExeFree(void * ptr);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */

/*****************************************************************************
 
  FUNCTION NAME: ExeGetFreePoolMemory

  DESCRIPTION:

    This routine returns the number of currently available semaphors in the pool 

  PARAMETERS:
      
    INPUTS:
    
    pool    - pool ID; if pool==NULL use ExeSystemMemory pool

  RETURNED VALUES:

    returns number of available bytes in the pool.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None

*****************************************************************************/
#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeGetFreePoolMemoryCount(pool) \
        __ExeGetFreePoolMemoryCount(pool, __FILE__, __LINE__)

extern uint32 __ExeGetFreePoolMemoryCount(NU_MEMORY_POOL *pool,
                              const char *Filename, unsigned Linenumber);

#else

extern uint32 ExeGetFreePoolMemoryCount(NU_MEMORY_POOL *pool);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */

/*****************************************************************************
 
  FUNCTION NAME: ExeBufferCreate

  DESCRIPTION:

    This routine creates a buffer of fixed size memory records.

  PARAMETERS:
      
    INPUTS:
    
    BufferCbP  - A pointer to a buffer control block
    NumRec     - The number of records to create in the buffer.
    RecSize    - The size in bytes of the fixed memory records.

  RETURNED VALUES:

    None.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeBufferCreate(BufferCbP, NumRec, RecSize) \
        __ExeBufferCreate(BufferCbP, NumRec, RecSize, __FILE__, __LINE__)

extern void __ExeBufferCreate(ExeBufferT *BufferCbP, uint32 NumRec, uint32 RecSize,
                              const char *Filename, unsigned Linenumber);

#else

extern void ExeBufferCreate(ExeBufferT *BufferCbP, uint32 NumRec, uint32 RecSize);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */


/*****************************************************************************
 
  FUNCTION NAME: ExeBufferGet

  DESCRIPTION:

    This routine allocates a fixed size memory record from a specific
    buffer. The buffer must have been created previously with a call
    to ExeBufferCreate().

  PARAMETERS:

    INPUTS:
    
    BufferCbP - A pointer to a memory buffer control block

  RETURNED VALUES:

    void *    - A void pointer to a fixed size memory record

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeBufferGet(BufferCbP) \
        __ExeBufferGet(BufferCbP, __FILE__, __LINE__)

extern void * __ExeBufferGet(ExeBufferT *BufferCbP,
                             const char *Filename, unsigned Linenumber);

#else

extern void * ExeBufferGet(ExeBufferT *BufferCbP);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */


/*****************************************************************************
 
  FUNCTION NAME: ExeBufferFree

  DESCRIPTION:

    This routine deallocates a fixed size memory record back to a specific
    buffer. The buffer must have been created previously with a call
    to ExeBufferCreate().

  PARAMETERS:
      
    INPUTS:
    
    BufferP   -  A pointer to a fixed size memory buffer.

  RETURNED VALUES:

    None.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeBufferFree(BufferP) \
        __ExeBufferFree(BufferP, __FILE__, __LINE__)

extern void __ExeBufferFree(void *BufferP,
                            const char *Filename, unsigned Linenumber);
#else

extern void ExeBufferFree(void *BufferP);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */


/*****************************************************************************
 
  FUNCTION NAME: ExeHisrCreate

  DESCRIPTION:

    This routine creates a Nucleus high level ISR (HISR).

  PARAMETERS:
      
    INPUTS:
    
    HisrCbP       - A pointer to a HISR control block
    ThreadId      - HISR thread id
    Routine       - HISR routine to be called when HISR is activated
    Priority      - HISR priority 

  RETURNED VALUES:

    None

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
 
  TASKING CHANGES:

    None.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeHisrCreate(HisrCbP, ThreadId, Routine, Priority) \
        __ExeHisrCreate(HisrCbP, ThreadId, Routine, Priority, __FILE__, __LINE__)

extern void __ExeHisrCreate(ExeHisrT *HisrCbP, ExeHisrIdT ThreadId, void (*Routine)(void), 
                            ExeHisrPrioT Priority,
                            const char *Filename, unsigned Linenumber);

#else

extern void ExeHisrCreate(ExeHisrT *HisrCbP, ExeHisrIdT ThreadId, void (*Routine)(void), 
                          ExeHisrPrioT Priority);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */


/*****************************************************************************
 
  FUNCTION NAME: ExeHisrActivate

  DESCRIPTION:

    This routine activates a previously created HISR.

  PARAMETERS:
      
    INPUTS:
    
    HisrCbP       - A pointer to a HISR control block

  RETURNED VALUES:

    None

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
 
  TASKING CHANGES:

    None.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeHisrActivate(HisrCbP) \
        __ExeHisrActivate(HisrCbP, __FILE__, __LINE__)

extern void __ExeHisrActivate(ExeHisrT *HisrCbP,
                              const char *Filename, unsigned Linenumber);

#else

extern void ExeHisrActivate(ExeHisrT *HisrCbP);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */


/*****************************************************************************
 
  FUNCTION NAME: ExePreemptionChange

  DESCRIPTION:

    This routine changes the preemption posture of the currently
    executing task to disabled or enabled. If the posture is set
    to disabled no other task can preempt the current task. This
    routine can only be used by tasks and can not be used by HISRs.

  PARAMETERS:
      
    INPUTS:
    
    Preemption  - Preemption posture to set 
                  EXE_PREEMPT_DISABLED or EXE_PREEMPT_ENABLED

  RETURNED VALUES:

    None

  ERROR HANDLING:

    No error codes are returned. 
 
  TASKING CHANGES:

    This routine will cause the calling task to suspend only when 
    preemption is enabled and there is a higher priority task ready
    to run.

*****************************************************************************/

extern void ExePreemptionChange(ExePreemptionT Preemption);

/*****************************************************************************
 
  FUNCTION NAME: ExeMaxMsgBufferGet

  DESCRIPTION:

    This routine returns the largest message buffer that can be allocated.

  PARAMETERS:
      
    INPUTS:

    None

  RETURNED VALUES:

    uint32 - Maximum message buffer size.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/

extern uint32 ExeMaxMsgBufferGet(void);

/*****************************************************************************
 
  FUNCTION NAME: ExeNucleusSystemError

  DESCRIPTION:

    This routine converts a nucleus error to a exe unit error and 
    then calls MonFault.

  PARAMETERS:
      
    INPUTS: Error - nucleus error type

  RETURNED VALUES:

    none 

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/

extern void ExeNucleusSystemError(uint32 NucleusError);

/*****************************************************************************
 
  FUNCTION NAME: ExeSystemClockGet

  DESCRIPTION:

    This routine returns the current system clock value.

  PARAMETERS:
      
    INPUTS:

    None

  RETURNED VALUES:

    uint32 - Current system clock.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/

extern uint32 ExeSystemClockGet(void);

/*****************************************************************************
 
  FUNCTION NAME: ExeSystemClockSet

  DESCRIPTION:

    This routine sets the current system clock value.

  PARAMETERS:
      
    INPUTS:

    ClockValue - The value to which the system clock will be set.

  RETURNED VALUES:

    None

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/

extern void ExeSystemClockSet(uint32 ClockValue);


/*****************************************************************************
 
  FUNCTION NAME: ExeTimerGetRemainTime
 
  DESCRIPTION:
 
    This routine get the remaining time of application timer.
 
  PARAMETERS:
      
    INPUTS:
    
    TimerCbP - A pointer to a timer control block
 
  RETURNED VALUES:
 
    None.
 
  ERROR HANDLING:
 
    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:
 
    None.
 
******************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeTimerGetRemainTime(TimerCbP, RemainTime) \
        __ExeTimerGetRemainTime(TimerCbP, RemainTime, __MODULE__, __LINE__)

  extern void __ExeTimerGetRemainTime(ExeTimerT *TimerCbP, uint32 *RemainTime,
                                 const char *Filename, unsigned Linenumber);

#else

  extern void ExeTimerGetRemainTime(ExeTimerT *TimerCbP, uint32 *RemainTime);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */
/*****************************************************************************
 
  FUNCTION NAME: ExeTimerDelete

  DESCRIPTION:

    This routine deletes a previously created application timer. All timers
    created must be deleted.  Otherwise, Timers created that happen to
    be located at the same address of a previously created but never
    deleted timer, will fail upon creation..

  PARAMETERS:
      
    INPUTS:
    
    TimerCbP       - A pointer to a timer control block

  RETURNED VALUES:

    None.

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeTimerDelete(TimerCbP) \
        __ExeTimerDelete(TimerCbP, __MODULE__, __LINE__)

extern void __ExeTimerDelete(ExeTimerT *TimerCbP, 
                            const char *Filename, unsigned Linenumber);
#else

extern void ExeTimerDelete(ExeTimerT *TimerCbP);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */
/*****************************************************************************
 
  FUNCTION NAME: ExeInterruptDisable

  DESCRIPTION:

    This routine disables interrupts specified by the input bit mask.

    NOTE: This routine must be used in conjunction with ExeInterruptEnable.
    
  PARAMETERS:
      
    INPUTS:
    
    IntMask - Bit Mask of interrupts to disable (IRQ, FIQ, or both)

  RETURNED VALUES:

    None

  ERROR HANDLING:

    No error codes are returned.                                         
 
  TASKING CHANGES:

    None.

*****************************************************************************/
extern void ExeInterruptDisable(SysIntT IntMask);


/*****************************************************************************
 
  FUNCTION NAME: ExeInterruptEnable

  DESCRIPTION:

    This routine restores the interrupt status prior to the most recent
    call to ExeInterruptDisable.

    NOTE: This routine must be used in conjunction with ExeInterruptDisable.

  PARAMETERS:
      

  RETURNED VALUES:

    None

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
 
  TASKING CHANGES:

    None.

*****************************************************************************/
extern void ExeInterruptEnable( void );
extern uint32 ExeThreadIDGet(void);

/*****************************************************************************
 
  FUNCTION NAME: ExeFaultHalt

  DESCRIPTION:

    This routine sends all pending message queue information, message buffer
    statistics, and task control block information to ETS. Since this routine
    should only be called following a fatal crash, this routine will not
    return. Instead the processor is placed in an infinite loop.

  PARAMETERS:
 
    None.

  RETURNED VALUES:

    None.

  TASKING CHANGES:

    None.

*****************************************************************************/
extern void ExeTaskPriorityChange( ExeTaskIdT dwTaskId, uint8 byNewPriority);
void ExeFaultHalt(void);

#endif
