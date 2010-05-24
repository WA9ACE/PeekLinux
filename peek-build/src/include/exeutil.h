#ifndef EXEUTIL_H
#define EXEUTIL_H


/*---------------------------------------------------------------------
* Define EXE global utility function prototypes/macros
*---------------------------------------------------------------------*/


/*****************************************************************************
 
  FUNCTION NAME: ExeSpyMsgMboxStats

  DESCRIPTION:

     This routine collects the message mailbox stats for each task and puts
     them in a Spy.

  PARAMETERS:
    
     None

  RETURNED VALUES:

     None

*****************************************************************************/
extern void ExeSpyMsgMboxStats(void);


/*****************************************************************************
 
  FUNCTION NAME: ExeSpyMsgBuffStats

  DESCRIPTION:

     This routine collects the message buffer stats for all types of
     messgae buffers and puts them in a Spy.

  PARAMETERS:
    
     None

  RETURNED VALUES:

     None

*****************************************************************************/
extern void ExeSpyMsgBuffStats(void);


/*****************************************************************************
 
  FUNCTION NAME: ExeSpyMsgBuffPtrStats

  DESCRIPTION:

     This routine collects the message buffer pointer stats and
     puts them in a Spy.

  PARAMETERS:
    
     None

  RETURNED VALUES:

     None

*****************************************************************************/
extern void ExeSpyMsgBuffPtrStats(void);


/*****************************************************************************
 
  FUNCTION NAME: ExeInitMsgBuffStats 

  DESCRIPTION:
     
     This routine initialize all the buffer statistics data structures

  PARAMETERS:
    
     None

  RETURNED VALUES:

     None

*****************************************************************************/
extern void ExeInitMsgBuffStats(void);


/*****************************************************************************
 
  FUNCTION NAME: ExeDecMsgBuffStats

  DESCRIPTION:

     This routine decrements all the msg buffer statistics.

  PARAMETERS:
    
     MsgBuffPtr - Current active msg pointer

  RETURNED VALUES:

     None

*****************************************************************************/
extern void ExeDecMsgBuffStats(void * MsgBuffPtr);


/*****************************************************************************
 
  FUNCTION NAME: ExeIncMsgBuffSendStats

  DESCRIPTION:

     This routine increments the msg buffer send statistics.

  PARAMETERS:
    
     MsgBuffPtr - Current active msg pointer
     MsgId      - Msg id of msg in msg buffer
     TaskId     - Task id receiving the message

  RETURNED VALUES:

     None

*****************************************************************************/
extern void ExeIncMsgBuffSendStats(void * MsgBuffPtr, uint32 MsgId, 
                                   uint32 TaskId);


/*****************************************************************************
 
  FUNCTION NAME: ExeIncMsgBuffStats 

  DESCRIPTION:

     This routine increments all the msg buffer statistics.

  PARAMETERS:
    
     MsgBuffPtr   - Current active msg pointer
     MsgBuffType  - Msg buffer type of active msg pointer
     MsgBuffSize  - Msg buffer size of active msg pointer
     TaskId       - Task id allocating the msg buffer

  RETURNED VALUES:

     None

*****************************************************************************/
extern void ExeIncMsgBuffStats(void * MsgBuffPtr, uint32 MsgBuffType, 
                               uint32 MsgBuffSize, uint32 TaskId);




#endif
