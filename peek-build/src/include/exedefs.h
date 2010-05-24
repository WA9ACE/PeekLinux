#ifndef EXEDEFS_H
#define EXEDEFS_H


#include "cp_nucleus.h"
#include "exeapi.h"
#include "sysdefs.h"
#include "exeerrs.h"

/*--------------------------------------------------------------------
* Define Exe constants
*--------------------------------------------------------------------*/

/* HISR stack size in bytes */
#define EXE_HISR_STACK_SIZE      1024

/* Size of mailbox queue record in unit32 data elements */
#define EXE_MAIL_QUEUE_REC_SIZE  3 

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

/* Define the maximum message buffer size */
/* the max msg buf size is size 3 and not size 4 because size 4
   is used specifically for NAM accesses DBM and ETS */
#define EXE_MAX_MSG_BUFF_SIZE   EXE_SIZE_MSG_BUFF_3

/* Define the maximum message buffer size for IOP use. This should only
   be used by IOP. */
#define EXE_MAX_IOP_MSG_SIZE    EXE_SIZE_MSG_BUFF_4

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

/*--------------------------------------------------------------------
* Define Exe Task Control Block data structure
*--------------------------------------------------------------------*/

typedef struct
{
   int32           NumMsgs;
   int32           NumMsgsInQueue[EXE_NUM_MAILBOX];
   NU_QUEUE        MailQueueCb[EXE_NUM_MAILBOX];
   NU_TASK         TaskCb;
   NU_EVENT_GROUP  EventGroupCb;
} ExeTaskCbT;


/*****************************************************************************
 
  FUNCTION NAME: ExeInit

  DESCRIPTION:

    This routine performs Exe unit initialization of data
    structures.

  PARAMETERS:
      
    None.

  RETURNED VALUES:

    None.
 
*****************************************************************************/

extern void ExeInit(void);

/*****************************************************************************
 
  FUNCTION NAME: ExeFault

  DESCRIPTION:

    This routine sends software fault declared by the EXE unit to
    the ETS. ExeFaults by definition are of HALT type which means this
    routine does not return to the caller and the processor executes
    an infinite loop.
    
  PARAMETERS:
      
    INPUTS:

    ExeFaultType - an EXE unit error
    ExeFaultData - a void pointer to specific fault data to be delivered to ETS

  RETURNED VALUES:

    None

  ERROR HANDLING:

    No error codes are returned. This routine handles all Nucleus errors.
  
  TASKING CHANGES:

    None.

*****************************************************************************/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define ExeFault(ExeFaultType, ExeError, ExeFaultData, FaultSize) \
        __ExeFault(ExeFaultType, ExeError, ExeFaultData, FaultSize, Filename, Linenumber)

extern void __ExeFault(ExeFaultTypeT ExeFaultType, ExeErrsT ExeError,
                       void *ExeFaultData, uint16 FaultSize,
                       const char *Filename, unsigned Linenumber);
#else

extern void ExeFault(ExeFaultTypeT ExeFaultType, ExeErrsT ExeError,
                     void *ExeFaultData, uint16 FaultSize);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */




#endif
