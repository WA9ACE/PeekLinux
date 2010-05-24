#ifndef EXEERRS_H
#define EXEERRS_H


#include "cp_nucleus.h"
#include "exeapi.h"
#include "sysdefs.h"

/*------------------------------------------------------------------------
*  The following definitions are fault ids for MonFault routine.
*-----------------------------------------------------------------------*/

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

typedef enum
{
   EXE_FAULT_TYPE_1 = 0,
   EXE_FAULT_TYPE_2,
   EXE_FAULT_TYPE_3
} ExeFaultTypeT;

typedef PACKED struct
{
   uint8          Id;
   uint8          Priority;
   uint8          Status;
   uint8          MboxMsgs[EXE_NUM_MAILBOX];
} ExeTaskStatusT;

/* Define EXE Fault type 1 msg structure */
typedef PACKED struct
{
   ExeErrsT       ExeError;
   uint32         SysTime;
   uint8          SrcTaskId;
   int32          NucleusError;
   ExeTaskStatusT TaskStatus[EXE_NUM_TASKS];
} ExeFaultType1T;

/* Define EXE Fault type 2 msg structure */
typedef PACKED struct
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
typedef PACKED struct
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


#endif



