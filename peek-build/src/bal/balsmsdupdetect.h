

#ifndef RETRANS_DETECT_H
#define RETRANS_DETECT_H

#include "balapi.h"
#include "balsmsapi.h"

bool BalSmsJudgeDupMsg(BalSmsMessageT* RcvMsgP, UINT16 *pRecID, 
	    BalSmsDupliPolicyT* MsgP );

/* function provided to establish the look up table*/
uint16 BalSmsCalculatePduCrc16(uint8* pdu, uint8 len);
uint32 BalSmsCalculatePduTime(uint8* pdu, uint8 len);
BalSmsPriorityT BalSmsCalculatePduPriority(uint8* BalSmsPduP, uint8 Length);
bool BalSmsCalculatePduCBMsg(uint8* BalSmsPduP, uint8 Length);
bool BalSmsCalculatePduDeliverAckMsg(uint8* BalSmsPduP, uint8 Length);

/*==============================================================================
FUNCTION        BalSmsJudgeDupMsg
DESCRIPTION     judge if an incoming delivery message is a duplacated one
PARAMETER       RcvMsgP: the incoming delivery message
RETURN          TRUE for duplicated message, FALSE for a new message
===============================================================================*/
#if 0
bool BalSmsJudgeDupMsg( BalSmsDeliverIndMsgT* RcvMsgP );
#endif

#endif

/*****************************************************************************
* $Log: valsmsdupdetect.h $
* Revision 1.1  2007/10/29 11:01:38  binye
* Initial revision
* Revision 1.1  2007/10/12 13:17:22  lwang
* Initial revision
* Revision 1.1  2007/09/24 14:01:19  binye
* Initial revision
* Revision 1.2  2006/12/05 17:08:49  gdeng
* modified duplicate detection and register function;
* Revision 1.1  2006/11/26 22:16:06  yliu
* Initial revision
* Revision 1.1  2006/10/24 15:04:40  binye
* Initial revision
* Revision 1.1  2005/11/08 13:15:05  vnarayana
* Initial revision
* Revision 1.1  2005/11/07 15:33:32  wavis
* Initial revision
* Revision 1.1  2005/10/12 15:34:27  dorloff
* Initial revision
* Revision 1.1  2005/03/10 13:16:17  lwang
* Initial revision
* Revision 1.1  2004/08/02 15:52:16  xuhua
* Initial revision
* Revision 1.1  2004/05/26 13:14:40  javese
* Initial revision
* Revision 1.1  2003/11/12 10:11:44  shitong
* Initial revision
* Revision 1.2  2003/11/11 17:45:38  york
* merged in
*****************************************************************************/
