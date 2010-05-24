

#ifndef VSMS_PROC_MSG_H
#define VSMS_PROC_MSG_H

/*==================================================================================
                VSMS MESSAGE PROCESS HEAD FILE
===================================================================================*/

/*===================================================================================
                     INCLUDE FILE AREA
====================================================================================*/

#include "balsmsapi.h"

/*===================================================================================
                       DEFINITION AREA
=====================================================================================*/

#ifndef BYD_USE_SIM
#define  SMS_MSG_ID(id) \
         (((id) == BAL_AMPS_EXT_PROTO_MSG)|| \
         ((id) == BAL_NWK_RPT_ALERT_MSG) || \
         ((id) == BAL_NWK_RPT_FLASH_MSG) || \
         ((id) == BAL_SMS_DELIVER_IND_PARMS_MSG) || \
         ((id) == BAL_SMS_DELIVER_IND_DATA_MSG) ||  \
         ((id) == BAL_SMS_ERROR_MSG) ||             \
         ((id) == BAL_SMS_BCAST_IND_PARMS_MSG) ||   \
         ((id) == BAL_SMS_BCAST_ERROR_MSG) || \
         ((id) == BAL_SMS_CAUSE_CODE_STATUS_MSG) || \
         ((id) == BAL_SMS_TST_CONNECT_MSG) || \
         ((id) == BAL_SMS_TST_BCAST_CONNECT_MSG) || \
         ((id) == BAL_SMS_TST_BCAST_DISCONNECT_MSG) || \
         ((id) == BAL_SMS_TST_BCAST_PREF_MSG) || \
         ((id) == BAL_SMS_TST_CANCEL_MSG) || \
         ((id) == BAL_SMS_TST_CAUSE_CODE_STATUS_MSG) || \
         ((id) == BAL_SMS_TST_DISCONNECT_MSG) || \
         ((id) == BAL_SMS_TST_PREF_SRV_OPT_MSG) || \
         ((id) == BAL_SMS_TST_SUBMIT_MSG) || \
         ((id) == BAL_SMS_TST_TERM_STATUS_MSG) || \
         ((id) == BAL_SMS_TST_USER_ACK_MSG) || \
         ((id) == BAL_SMS_TST_RETX_AMOUNT_MSG))
#else
/* following definition added by zhengyi */
#define  SMS_MSG_ID(id) \
         (((id) == BAL_AMPS_EXT_PROTO_MSG)|| \
         ((id) == BAL_NWK_RPT_ALERT_MSG) || \
         ((id) == BAL_NWK_RPT_FLASH_MSG) || \
         ((id) == BAL_SMS_DELIVER_IND_PARMS_MSG) || \
         ((id) == BAL_SMS_DELIVER_IND_DATA_MSG) ||  \
         ((id) == BAL_SMS_ERROR_MSG) ||             \
         ((id) == BAL_SMS_BCAST_IND_PARMS_MSG) ||   \
         ((id) == BAL_SMS_BCAST_ERROR_MSG) || \
         ((id) == BAL_SMS_CAUSE_CODE_STATUS_MSG) || \
         ((id) == BAL_SMS_TST_CONNECT_MSG) || \
         ((id) == BAL_SMS_TST_BCAST_CONNECT_MSG) || \
         ((id) == BAL_SMS_TST_BCAST_DISCONNECT_MSG) || \
         ((id) == BAL_SMS_TST_BCAST_PREF_MSG) || \
         ((id) == BAL_SMS_TST_CANCEL_MSG) || \
         ((id) == BAL_SMS_TST_CAUSE_CODE_STATUS_MSG) || \
         ((id) == BAL_SMS_TST_DISCONNECT_MSG) || \
         ((id) == BAL_SMS_TST_PREF_SRV_OPT_MSG) || \
         ((id) == BAL_SMS_TST_SUBMIT_MSG) || \
         ((id) == BAL_SMS_TST_TERM_STATUS_MSG) || \
         ((id) == BAL_SMS_TST_USER_ACK_MSG) || \
         ((id) == BAL_SMS_TST_RETX_AMOUNT_MSG) || \
         ((id) == BAL_SMS_COYOTE_SMS_MC_INCOMING_MSG) ||\
         ((id) == BAL_SMS_COYOTE_SMS_MC_BROAD_CAST_MSG) ||\
         ((id) == BAL_SMS_COYOTE_SMS_MC_MEMORY_STATUS_MSG))
#endif

/*===================================================================================
                     MESSAGE DEFINITION AREA
=====================================================================================*/
bool MsgNeedSave(BalSmsTeleSrvIdT TeleSrvId);
int8 BalSmsProcessSmsAmpsMsg(void *MsgBuf, uint16 MsgLen);
int8 BalSmsProcessDeliverParaMsg(void *MsgBuf, uint16 MsgLen);
int8 BalSmsProcessDeliverDataMsg(void *MsgBuf, uint16 MsgLen);
int8 BalSmsProcessErrMsg(void *MsgBuf, uint16 MsgLen);
int8 BalSmsProcessCauseCodeMsg(void *MsgBuf, uint16 MsgLen);
int8 BalSmsProcessBcParaMsg(void *MsgBuf, uint16 MsgLen);
int8 BalSmsProcessBcDataMsg(void *MsgBuf, uint16 MsgLen);
void BalSmsProcessMemStatusMsg(void *MsgBuf, uint16 MsgLen);

void BalSmsSendSrvReady( void );
#ifdef BYD_USE_SIM
int ProcessCoyoteSMSEvent(BalSmsEventIdT Event, BalSmsMessageT *MsgBuf);
#endif

#endif

/*****************************************************************************
* $Log: valsmsprocmsg.h $
* Revision 1.1  2007/10/29 11:01:39  binye
* Initial revision
* Revision 1.1  2007/10/12 13:17:27  lwang
* Initial revision
* Revision 1.1  2007/09/24 14:01:19  binye
* Initial revision
* Revision 1.1  2006/11/26 22:16:13  yliu
* Initial revision
* Revision 1.1  2006/10/24 15:04:41  binye
* Initial revision
* Revision 1.2  2006/01/12 11:08:09  wavis
* SMS changes merged from 4.05.
* Revision 1.1  2005/11/08 13:15:11  vnarayana
* Initial revision
* Revision 1.1  2005/11/07 15:33:40  wavis
* Initial revision
* Revision 1.1  2005/10/12 15:34:35  dorloff
* Initial revision
* Revision 1.1  2005/03/10 13:16:25  lwang
* Initial revision
* Revision 1.1.3.3  2005/01/12 16:37:43  yzheng
* Added prototype of PreVmnMsgPosition
* Revision 1.1.3.2  2004/08/06 14:55:18  wf
* Baseline reconstruction.
* Revision 1.2  2004/06/09 09:32:00  gzhu
* Got from old branch on 08 Jun.
* Revision 1.6  2004/06/01 09:04:57  zhengyi
* Added broadcast message in SMS_MSG_ID.
* Revision 1.5  2004/04/22 13:52:14  zhengyi
* added Coyote SMS MC Message Ids
* Revision 1.2  2003/12/23 16:55:17  york
* removed an unnessary function decalration
* Revision 1.1  2003/11/12 10:11:45  shitong
* Initial revision
* Revision 1.2  2003/11/11 17:45:57  york
* merged in
*****************************************************************************/
