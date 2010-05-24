#ifndef  _BALATCMD_H__
#define _BALATCMD_H__


#include "sysdefs.h"
#include "aiwapi.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef SYS_OPTION_ENHANCEDAT

typedef enum {
  BAL_AT_EVENT_REQUEST = 1,
  BAL_AT_EVENT_ACK	
}BalAtEventT;

typedef void (*BalAtEventFunc) 
(
    BalAtEventT MsgId, 
    AiwSendAtMsgT* MsgBufferP
);

/*-----------------------------------------------------------------
 *   Enhanced AT command  interface   
 *----------------------------------------------------------------*/
extern void SendCorgToAiw(uint8* digits, uint8 len);
extern void AtEnhancedInit(void);

extern void ProcessAtCmdMsg (AiwSendAtMsgT* MsgDataP);
extern void ProcessATBufferAckMsg(AiwSendAtMsgT* MsgDataP);
void sendUnslctResult(AiwSendAtRespMsgT*  pResult);
void BalATCmdRegister(BalAtEventFunc	EventFunc);
#endif

#ifdef __cplusplus
}
#endif
#endif

