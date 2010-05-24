

#ifndef BAL_SMS_DEF_H
#define BAL_SMS_DEF_H
/*=================================================================================
         include file
==================================================================================*/
#include "buimapi.h"
#include "balsmsapi.h"
#include "balsmsfmtconv.h"
#include "balsmsdupdetect.h"
#include "balsmsprocmsg.h"
#include "balsmssendmsg.h"
#include "balutils.h"
#define BAL_SMS_REC_HEAD_LENGTH       2  /*the header length of the saved record*/


/*=================================================================================
                              FUNCTION DEFINITION
==================================================================================*/
void BalSmsSetInitialized( bool init );

void BalSmsDeliverUimMsg( uint32 msg_id,
                          void*  msg_buffer,
                          uint32 msg_size );

int  BalSmsSendTermStatus( BalSmsTermStatusT StatusCode );

bool BalSmsSendOnTraffic( void );

/*================================================================================

FUNCTION  ProcessSmsMsg

DESCRIPTIN
   message handler for messages from PSW task

PARAMETERS
    MsgId: the message id
    MsgBuf: message buffer for  received message
    MsgLen: the length of the received message

RETURN
    TRUE for success, else for failure

=================================================================================*/
bool BalProcessSmsMsg( uint32 MsgId,
                       void*  MsgBuf,
                       uint32 MsgLen );

/*===========================================================================

FUNCTION BalSmsSetChannel

DESCRIPTION

  Validate the 'channel'. Set channel to be used for the next SMS message
  sent out with the validated 'channel'.

DEPENDENCIES

  'channel' should be a valid one.

RETURN VALUE

  void

SIDE EFFECTS
  None

===========================================================================*/
void BalSmsSetChannel( BalSmsChannelT channel );

/*****************************************************************************

  FUNCTION NAME: BalSmsCauseCodeMsg

  DESCRIPTION:

   Cause Code information from PSW SMS used for sending back cause codes on acks

  PARAMETERS:

   MsgP    - pointer status message data
   MsgSize - size of the message in bytes

  RETURNED VALUES: None

*****************************************************************************/
void BalSmsCauseCodeMsg( void*  MsgP,
                         uint32 MsgSize );

void BalSmsSetMsgId( uint32 num );

uint16 BalSmsGetMsgId( void );

uint16 BalSmsGetMsgIdValueOnly( void );

/* TEST FUNCTIONS */
void BalSmsTstRetxAmountMsg( void*  MsgDataP,
                             uint32 MsgSize );

void BalSmsTstUserAckMsg( void*  MsgDataP,
                          uint32 MsgSize );

void BalSmsTstCauseCodeStatusMsg( void );

void BalSmsTstSendSpy( void*  MsgDataP,
                       uint32 MsgSize );

void BalSmsTstSubmitMsg( void*  MsgDataP,
                         uint32 MsgSize );

void BalSmsTstPrefSrvOptMsg( void*  MsgDataP,
                             uint32 MsgSize );

void BalSmsTstBCastParmsMsg( void*  MsgDataP,
                             uint32 MsgSize );

void BalSmsTstCancelMsg( void*  MsgDataP,
                         uint32 MsgSize );

void BalSmsTstBCastConnectMsg( void*  MsgDataP,
                               uint32 MsgSize );

void BalSendSmsIncomingMsgToUi(void);

#endif
/*****************************************************************************
* $Log: valsmsdef.h $
* Revision 1.1  2007/10/29 11:01:38  binye
* Initial revision
* Revision 1.1  2007/10/12 13:17:20  lwang
* Initial revision
* Revision 1.1  2007/09/24 14:01:18  binye
* Initial revision
* Revision 1.6  2006/12/28 13:55:43  yjin
* Revision 1.5  2006/12/07 16:54:51  gdeng
* Rename BalSmsInitUimSms to BalSmsStorageInit, check in for lirong
* Revision 1.4  2006/12/06 15:20:28  gdeng
* fixed sms crash, check in for lirong
* Revision 1.3  2006/12/05 17:08:33  gdeng
* modified duplicate detection and register function;
* Revision 1.2  2006/12/04 20:32:21  xuhua
* merge EswPlatform_move2sw_baseline
* Revision 1.2  2006/11/29 18:44:09  rli
* Initial revision
* Revision 1.1  2006/10/24 15:04:40  binye
* Initial revision
* Revision 1.2  2006/02/08 17:08:23  sbenz
* moved non-global function headers to this file from ..\inc\valsmsapi.h
* Revision 1.1  2005/11/08 13:15:03  vnarayana
* Initial revision
* Revision 1.1  2005/11/07 15:33:30  wavis
* Initial revision
* Revision 1.1.1.2  2005/10/12 15:36:27  dorloff
* new BAL\UI\FSM merge
* Revision 1.1  2005/03/10 13:15:37  lwang
* Initial revision
* Revision 1.1  2004/08/02 15:45:09  xuhua
* Initial revision
* Revision 1.1  2004/05/26 13:14:27  javese
* Initial revision
* Revision 1.3  2004/03/23 12:54:02  york
* add teleservice id into the structure of lookuptable
* Revision 1.2  2003/11/12 17:47:51  shitong
* add other sms head file
* Revision 1.1  2003/11/12 10:21:21  shitong
* Initial revision
* Revision 1.2  2003/11/11 13:54:45  xugang
* Merge in
* Initial revision
*****************************************************************************/


