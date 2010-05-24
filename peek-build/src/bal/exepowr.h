/*=================================================================================

	HEADER NAME : exepowr.h
	MODULE NAME : BAL
	

GENERAL DESCRIPTION

    This file contains definitions for task priorities, task stack sizes,
    task mail queue sizes, and the system memory pool size.

BYD TECHFAITH Wireless Confidential Proprietary
(c) Copyright 2008 by BYD TECHFAITH Wireless. All Rights Reserved.
===================================================================================
Revision History
Modification Tracking
Date Author Number Description of changes
---------- ------------ --------- --------------------------------------------


	
=================================================================================*/

#ifndef EXEPOWR_H
#define EXEPOWR_H

#if 0
/* Define task priorities */
#define LMD_TASK_PRI                0
#define LMD_S_TASK_PRI              1
#define IPC_TASK_PRI                2
#define L1D_MDM_TASK_PRI            3
#define L1D_AMPS_TASK_PRI           4
#define PSW_TASK_PRI                5
#define RLP_TASK_PRI                6
#define HWD_TASK_PRI                7
#define BAL_TASK_PRI                8
#define HLW_TASK_PRI                9
#define AIW_TASK_PRI                10

#define UIM_TASK_PRI                11
#define IOP_TASK_PRI                12
#define PDE_TASK_PRI                13
#define DBM_TASK_PRI                14
#define MON_TASK_PRI                16
#define TST_TASK_PRI                17
#define PSW_S_TASK_PRI              18
#define UI_TASK_PRI                 19
#define BREW_TASK_PRI                 20
#define BM_TASK_PRI                 21
#define UI_S_TASK_PRI               25
#define LSM_TASK_PRI                30
#define PST_TASK_PRI                40
#define FSM_TASK_PRI                50
#define MON_IDL_TASK_PRI            60

/* Define task stack sizes in bytes */
#if (defined SYS_OPTION_HL)
#define AIW_TASK_STACK              1000
#else
#define AIW_TASK_STACK                 0
#endif

#define DBM_TASK_STACK              1500

#if (defined SYS_OPTION_GPS)
#define PDE_TASK_STACK              3000
#else
#define PDE_TASK_STACK                 0
#endif

#if (defined SYS_OPTION_NTWKRMPKT_RAM_INUSE)
#define HLW_TASK_STACK              1480
#else
#define HLW_TASK_STACK              1000
#endif

#define HWD_TASK_STACK              1000

#define IPC_TASK_STACK              1000   

#define IOP_TASK_STACK              1000

#define L1D_MDM_TASK_STACK          2500

#if (defined SYS_OPTION_AMPS)
#define L1D_AMPS_TASK_STACK         1000
#else
#define L1D_AMPS_TASK_STACK            0
#endif

#define LMD_TASK_STACK              2000

#define LMD_S_TASK_STACK            1000

#define BAL_TASK_STACK              3000
#define BM_TASK_STACK               1000

#define MON_TASK_STACK              3000

#if (!(defined OTTS))
#define MON_IDL_TASK_STACK           500
#else
#define MON_IDL_TASK_STACK             0
#endif

#define PSW_TASK_STACK              6000

#if (defined SYS_OPTION_OTASP) || (defined SYS_OPTION_MIP_DMU)
#define PSW_S_TASK_STACK            2000
#else
#define PSW_S_TASK_STACK               0
#endif

#if ((defined SYS_OPTION_RLP) && (!(defined OTTS)))
#define RLP_TASK_STACK              1000
#else
#define RLP_TASK_STACK                 0
#endif

#if (defined EXE_UTE_TST_TASK)
#define TST_TASK_STACK               500
#else
#define TST_TASK_STACK                 0
#endif

#ifdef SYS_OPTION_RUIM
#define UIM_TASK_STACK                 2000
#else 
#define UIM_TASK_STACK                 0
#endif

#ifdef SYS_OPTION_FSM
#define FSM_TASK_STACK               1500/*FSM_WRITE_TASK_STACK_SIZE*/
#else 
#define FSM_TASK_STACK               0
#endif

#define UI_TASK_STACK               8120
#define UI_S_TASK_STACK             8120

 #ifdef FEATURE_BREW
 #define BREW_TASK_STACK             8120
 #else 
 #define BREW_TASK_STACK               0
 #endif

#define LSM_TASK_STACK              4000

#define PST_TASK_STACK              4000
/* Define the total stack size memory requirement, in bytes.
   Each stack size includes the amount of overhead consumed 
   by Nucleus for memory allocation */
#define TASK_STACK_TOTAL_SIZE \
 (DM_OVERHEAD + AIW_TASK_STACK +\
  DM_OVERHEAD + DBM_TASK_STACK +\
  DM_OVERHEAD + PDE_TASK_STACK +\
  DM_OVERHEAD + HLW_TASK_STACK +\
  DM_OVERHEAD + HWD_TASK_STACK +\
  DM_OVERHEAD + IPC_TASK_STACK +\
  DM_OVERHEAD + IOP_TASK_STACK +\
  DM_OVERHEAD + L1D_MDM_TASK_STACK +\
  DM_OVERHEAD + L1D_AMPS_TASK_STACK +\
  DM_OVERHEAD + LMD_TASK_STACK +\
  DM_OVERHEAD + LMD_S_TASK_STACK +\
  DM_OVERHEAD + BAL_TASK_STACK +\
  DM_OVERHEAD + MON_TASK_STACK +\
  DM_OVERHEAD + MON_IDL_TASK_STACK +\
  DM_OVERHEAD + PSW_TASK_STACK +\
  DM_OVERHEAD + PSW_S_TASK_STACK +\
  DM_OVERHEAD + RLP_TASK_STACK +\
  DM_OVERHEAD + TST_TASK_STACK +\
  DM_OVERHEAD + UI_TASK_STACK +\
  DM_OVERHEAD + UI_S_TASK_STACK +\
  DM_OVERHEAD + UI_TASK_STACK  +\
  DM_OVERHEAD + BREW_TASK_STACK +\
  DM_OVERHEAD + FSM_TASK_STACK +\
  DM_OVERHEAD + BM_TASK_STACK +\
  DM_OVERHEAD + PST_TASK_STACK +\
  DM_OVERHEAD + LSM_TASK_STACK +\
  DM_OVERHEAD + UIM_TASK_STACK)
#endif

/* Define task mailbox queue sizes in number of mail msg per queue.
   This number is multiplied by the mail queue record size to determine
   the size of the mailbox queue in unit32s. */

#if 0
#define AIW_TASK_MAIL_QUEUE_1       20 * EXE_MAIL_QUEUE_REC_SIZE

#define DBM_TASK_MAIL_QUEUE_1       60 * EXE_MAIL_QUEUE_REC_SIZE

#if (defined SYS_OPTION_GPS)
#define PDE_TASK_MAIL_QUEUE_1       15 * EXE_MAIL_QUEUE_REC_SIZE
#else
#define PDE_TASK_MAIL_QUEUE_1        0
#endif

#define HWD_TASK_MAIL_QUEUE_1       20 * EXE_MAIL_QUEUE_REC_SIZE
#define HWD_TASK_MAIL_QUEUE_2       40 * EXE_MAIL_QUEUE_REC_SIZE

#if (defined SYS_OPTION_HL)
#define HLW_TASK_MAIL_QUEUE_1       20 * EXE_MAIL_QUEUE_REC_SIZE
#ifdef SYS_OPTION_MIP_DMU
#define HLW_TASK_MAIL_QUEUE_2        5 * EXE_MAIL_QUEUE_REC_SIZE
#endif 
#else
#define HLW_TASK_MAIL_QUEUE_1        0
#endif

#define IOP_TASK_MAIL_QUEUE_1       70 * EXE_MAIL_QUEUE_REC_SIZE
#define IOP_TASK_MAIL_QUEUE_2       10 * EXE_MAIL_QUEUE_REC_SIZE
#define IOP_TASK_MAIL_QUEUE_3       20 * EXE_MAIL_QUEUE_REC_SIZE

#ifdef OTTS
#define IOP_TASK_MAIL_QUEUE_4       100 * EXE_MAIL_QUEUE_REC_SIZE
#define IOP_TASK_MAIL_QUEUE_5       50 * EXE_MAIL_QUEUE_REC_SIZE
#elif (defined SYS_OPTION_GPS)
#define IOP_TASK_MAIL_QUEUE_4       10 * EXE_MAIL_QUEUE_REC_SIZE
#define IOP_TASK_MAIL_QUEUE_5       0
#else
#define IOP_TASK_MAIL_QUEUE_4       0
#define IOP_TASK_MAIL_QUEUE_5       0
#endif  /* OTTS */

#define IPC_TASK_MAIL_QUEUE_1       20 * EXE_MAIL_QUEUE_REC_SIZE
#define IPC_TASK_MAIL_QUEUE_2       20 * EXE_MAIL_QUEUE_REC_SIZE
#define IPC_TASK_MAIL_QUEUE_3       20 * EXE_MAIL_QUEUE_REC_SIZE
#define IPC_TASK_MAIL_QUEUE_4        5 * EXE_MAIL_QUEUE_REC_SIZE


#if (defined SYS_OPTION_AMPS)
#define L1D_AMPS_TASK_MAIL_QUEUE_1  20 * EXE_MAIL_QUEUE_REC_SIZE
#else
#define L1D_AMPS_TASK_MAIL_QUEUE_1   0 * EXE_MAIL_QUEUE_REC_SIZE
#endif

#define L1D_MDM_TASK_MAIL_QUEUE_1   20 * EXE_MAIL_QUEUE_REC_SIZE
#define L1D_MDM_TASK_MAIL_QUEUE_2   20 * EXE_MAIL_QUEUE_REC_SIZE

#define LMD_TASK_MAIL_QUEUE_1       20 * EXE_MAIL_QUEUE_REC_SIZE
#define LMD_TASK_MAIL_QUEUE_2       20 * EXE_MAIL_QUEUE_REC_SIZE
#define LMD_TASK_MAIL_QUEUE_3       20 * EXE_MAIL_QUEUE_REC_SIZE
#define LMD_TASK_MAIL_QUEUE_4       20 * EXE_MAIL_QUEUE_REC_SIZE
#define LMD_TASK_MAIL_QUEUE_5       20 * EXE_MAIL_QUEUE_REC_SIZE

#define LMD_S_TASK_MAIL_QUEUE_1     20 * EXE_MAIL_QUEUE_REC_SIZE
#define MON_TASK_MAIL_QUEUE_1       60 * EXE_MAIL_QUEUE_REC_SIZE
#define MON_TASK_MAIL_QUEUE_2        5 * EXE_MAIL_QUEUE_REC_SIZE

#define PSW_TASK_MAIL_QUEUE_1       60 * EXE_MAIL_QUEUE_REC_SIZE
#define PSW_TASK_MAIL_QUEUE_2        5 * EXE_MAIL_QUEUE_REC_SIZE

#define BM_TASK_MAIL_QUEUE_1        20 * EXE_MAIL_QUEUE_REC_SIZE
#define BM_TASK_MAIL_QUEUE_2         3 * EXE_MAIL_QUEUE_REC_SIZE

#if (defined SYS_OPTION_OTASP) || (defined SYS_OPTION_MIP_DMU)
#define PSW_S_TASK_MAIL_QUEUE_1      5 * EXE_MAIL_QUEUE_REC_SIZE
#else
#define PSW_S_TASK_MAIL_QUEUE_1      0 * EXE_MAIL_QUEUE_REC_SIZE
#endif

#if ((defined SYS_OPTION_RLP) && (!(defined OTTS)))
#define RLP_TASK_MAIL_QUEUE_1       20 * EXE_MAIL_QUEUE_REC_SIZE
#else
#define RLP_TASK_MAIL_QUEUE_1        0 * EXE_MAIL_QUEUE_REC_SIZE
#endif

#if (defined EXE_UTE_TST_TASK)
#define TST_TASK_MAIL_QUEUE_1       10 * EXE_MAIL_QUEUE_REC_SIZE
#else
#define TST_TASK_MAIL_QUEUE_1        0 * EXE_MAIL_QUEUE_REC_SIZE
#endif

#ifdef FEATURE_BREW
 #define BREW_TASK_MAIL_QUEUE_1        10 * EXE_MAIL_QUEUE_REC_SIZE
 #else
 #define BREW_TASK_MAIL_QUEUE_1        0 * EXE_MAIL_QUEUE_REC_SIZE
 #endif
//#define UI_S_TASK_MAIL_QUEUE_1      30 * EXE_MAIL_QUEUE_REC_SIZE


#ifdef SYS_OPTION_RUIM
#define UIM_TASK_MAIL_QUEUE_1       10 * EXE_MAIL_QUEUE_REC_SIZE
#define UIM_TASK_MAIL_QUEUE_2       5 * EXE_MAIL_QUEUE_REC_SIZE
#else
#define UIM_TASK_MAIL_QUEUE_1       0 * EXE_MAIL_QUEUE_REC_SIZE
#define UIM_TASK_MAIL_QUEUE_2       0 * EXE_MAIL_QUEUE_REC_SIZE
#endif

#define LSM_TASK_MAIL_QUEUE_1       15 * EXE_MAIL_QUEUE_REC_SIZE
#define LSM_TASK_MAIL_QUEUE_2       15 * EXE_MAIL_QUEUE_REC_SIZE

#define PST_TASK_MAIL_QUEUE_1       15 * EXE_MAIL_QUEUE_REC_SIZE
#define PST_TASK_MAIL_QUEUE_2       15 * EXE_MAIL_QUEUE_REC_SIZE
#endif

#define BAL_TASK_MAIL_QUEUE_1       30 * EXE_MAIL_QUEUE_REC_SIZE
#define BAL_TASK_MAIL_QUEUE_2       30 * EXE_MAIL_QUEUE_REC_SIZE
//#define BAL_TASK_MAIL_QUEUE_3       30 * EXE_MAIL_QUEUE_REC_SIZE
#define UI_TASK_MAIL_QUEUE_1        30 * EXE_MAIL_QUEUE_REC_SIZE
#define UI_TASK_MAIL_QUEUE_2        60 * EXE_MAIL_QUEUE_REC_SIZE
#define UI_TASK_MAIL_QUEUE_3        30 * EXE_MAIL_QUEUE_REC_SIZE
#define UI_TASK_MAIL_QUEUE_4        3 * EXE_MAIL_QUEUE_REC_SIZE
#define EM_S_TASK_MAIL_QUEUE_1      30 * EXE_MAIL_QUEUE_REC_SIZE  //added by Randolph Wang 080219
#define AMDB_S_TASK_MAIL_QUEUE_1      10 * EXE_MAIL_QUEUE_REC_SIZE  //added by Randolph Wang 080219

#define EXE_TASK_MAIL_QUEUE_MAX_SIZE    UI_TASK_MAIL_QUEUE_1

/* Define the total stack size memory requirement, in bytes 
   Each mail queue size includes the amount of overhead consumed 
   by Nucleus for memory allocation */
#if 0   
#define TASK_MAIL_QUEUE_TOTAL_SIZE (( \
  //DM_OVERHEAD + AIW_TASK_MAIL_QUEUE_1 +\
  //DM_OVERHEAD + DBM_TASK_MAIL_QUEUE_1 +\
  //DM_OVERHEAD + PDE_TASK_MAIL_QUEUE_1 +\
  //DM_OVERHEAD + HWD_TASK_MAIL_QUEUE_1 + HWD_TASK_MAIL_QUEUE_2 +\
  //DM_OVERHEAD + HLW_TASK_MAIL_QUEUE_1 +\
  //DM_OVERHEAD + IOP_TASK_MAIL_QUEUE_1 + IOP_TASK_MAIL_QUEUE_2 + IOP_TASK_MAIL_QUEUE_3 + IOP_TASK_MAIL_QUEUE_4 + IOP_TASK_MAIL_QUEUE_5 +\
  //DM_OVERHEAD + IPC_TASK_MAIL_QUEUE_1 + IPC_TASK_MAIL_QUEUE_2 + IPC_TASK_MAIL_QUEUE_3 +\
  //DM_OVERHEAD + L1D_AMPS_TASK_MAIL_QUEUE_1 +\
  //DM_OVERHEAD + L1D_MDM_TASK_MAIL_QUEUE_1 + L1D_MDM_TASK_MAIL_QUEUE_2 +\
  //DM_OVERHEAD + LMD_TASK_MAIL_QUEUE_1 + LMD_TASK_MAIL_QUEUE_2 + LMD_TASK_MAIL_QUEUE_3 + LMD_TASK_MAIL_QUEUE_4 + LMD_TASK_MAIL_QUEUE_5 +\
  //DM_OVERHEAD + LMD_S_TASK_MAIL_QUEUE_1 +\
  DM_OVERHEAD + BAL_TASK_MAIL_QUEUE_1 + BAL_TASK_MAIL_QUEUE_2 +\
  //DM_OVERHEAD + MON_TASK_MAIL_QUEUE_1 + MON_TASK_MAIL_QUEUE_2 +\
  //DM_OVERHEAD + PSW_TASK_MAIL_QUEUE_1 + PSW_TASK_MAIL_QUEUE_2 +\
  //DM_OVERHEAD + PSW_S_TASK_MAIL_QUEUE_1 +\
  //DM_OVERHEAD + RLP_TASK_MAIL_QUEUE_1 +\
  //DM_OVERHEAD + UIM_TASK_MAIL_QUEUE_1 + UIM_TASK_MAIL_QUEUE_2 +\
  DM_OVERHEAD + UI_S_TASK_MAIL_QUEUE_1 +\
  //DM_OVERHEAD + BREW_TASK_MAIL_QUEUE_1 +\
  //DM_OVERHEAD + LSM_TASK_MAIL_QUEUE_1 + LSM_TASK_MAIL_QUEUE_2 +\
  //DM_OVERHEAD + PST_TASK_MAIL_QUEUE_1 + PST_TASK_MAIL_QUEUE_2 +\
  DM_OVERHEAD + UI_TASK_MAIL_QUEUE_1  + UI_TASK_MAIL_QUEUE_2 + UI_TASK_MAIL_QUEUE_3 \
  //DM_OVERHEAD + BM_TASK_MAIL_QUEUE_1 + BM_TASK_MAIL_QUEUE_2 +\
  //DM_OVERHEAD + TST_TASK_MAIL_QUEUE_1\
  ) * sizeof(UNSIGNED))
#else
#define TASK_MAIL_QUEUE_TOTAL_SIZE (( \
  DM_OVERHEAD + BAL_TASK_MAIL_QUEUE_1 + BAL_TASK_MAIL_QUEUE_2 +\
  DM_OVERHEAD + EM_S_TASK_MAIL_QUEUE_1 +\
  DM_OVERHEAD + UI_TASK_MAIL_QUEUE_1  + UI_TASK_MAIL_QUEUE_2 + UI_TASK_MAIL_QUEUE_3 + UI_TASK_MAIL_QUEUE_4 +\
  DM_OVERHEAD + AMDB_S_TASK_MAIL_QUEUE_1) * sizeof(UNSIGNED))
#endif

/* Define the total message buffer memory size, in bytes
   This is calculated based on when ExeBufferCreate is called
   to create the buffers from which the message buffers are allocated,
   the buffer size is calculated as:
   BufferSize = (NumRec * RecSize) + (NumRec * 10); 
   Also, when ExeBufferCreate is called to create the exe buffers for the
   message buffers the size of the message buffer is incremented by one.
   Therefore the total size allocated for each message buffer size is
   NumRec * (RecSize + 1 + 10)
   Each message buffer memory allocation size is incremented by the amount of 
   overhead consumed by Nucleus for memory allocation 
*/
#define EXE_MSG_BUFFER_TOTAL_SIZE \
 (DM_OVERHEAD + (EXE_NUM_MSG_BUFF_1 * (EXE_SIZE_MSG_BUFF_1 + 4 + 10)) +\
  DM_OVERHEAD + (EXE_NUM_MSG_BUFF_2 * (EXE_SIZE_MSG_BUFF_2 + 4 + 10)) +\
  DM_OVERHEAD + (EXE_NUM_MSG_BUFF_3 * (EXE_SIZE_MSG_BUFF_3 + 4 + 10)) +\
  DM_OVERHEAD + (EXE_NUM_MSG_BUFF_4 * (EXE_SIZE_MSG_BUFF_4 + 4 + 10)))

/* Define the total HISR stack size, in bytes 
   Each stack size is incremented by the amount of 
   overhead consumed by Nucleus for memory allocation */
#define HISR_STACK_TOTAL_SIZE 2*(EXE_HISR_STACK_SIZE + DM_OVERHEAD)

/* Define a built in memory size padding factor, this will allow
   for memory that is allocated by calls to ExeBufferCreate 
*/
#define SYS_MEMORY_SIZE_PAD_FACTOR    (1.05) /* 5 percent pad */

/* Define the total size of the memory from which EXE will allocate, in bytes */
/* FIXME! FOR TI compiler,unintergal cast to intergral in compile stage is not support*/
/*#define SYS_MEMORY_SIZE (int32)((TASK_MAIL_QUEUE_TOTAL_SIZE +\
                                 EXE_MSG_BUFFER_TOTAL_SIZE +\
                                 HISR_STACK_TOTAL_SIZE) * \
                                 SYS_MEMORY_SIZE_PAD_FACTOR)*/
/*
#define SYS_MEMORY_SIZE (int32)((TASK_MAIL_QUEUE_TOTAL_SIZE +\
                                 EXE_MSG_BUFFER_TOTAL_SIZE +\
                                 HISR_STACK_TOTAL_SIZE) + \
                                 (TASK_MAIL_QUEUE_TOTAL_SIZE +\
                                 EXE_MSG_BUFFER_TOTAL_SIZE +\
                                 HISR_STACK_TOTAL_SIZE \
                                 )>>4)*/
           
 /*
 //FIX ME: the memory size should be ajusted in qwert --zhangxi 2008/4/16
 This size is the memory size of qwert, include mail queue and msg buffer and hisr stack
 */
#define SYS_MEMORY_SIZE_BASE (int32)((TASK_MAIL_QUEUE_TOTAL_SIZE +\
                                 EXE_MSG_BUFFER_TOTAL_SIZE +\
                                 HISR_STACK_TOTAL_SIZE) )		
/*
This is the factor of memory size, in order to match SYS_MEMORY_SIZE_PAD_FACTOR 1.05
*/
#define SYS_MEMORY_SIZE_FACTOR (int32)(SYS_MEMORY_SIZE_BASE>>4)

#define SYS_MEMORY_SIZE_TOTAL (int32)(SYS_MEMORY_SIZE_BASE + SYS_MEMORY_SIZE_FACTOR)

#define SYS_MEMORY_SIZE (int32)(((SYS_MEMORY_SIZE_TOTAL+sizeof(unsigned int)-1)/sizeof(unsigned int))*sizeof(unsigned int))




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



#endif /* EXEPOWR_H */




