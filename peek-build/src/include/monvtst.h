#ifndef _MONVTST_H_
#define _MONVTST_H_


/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/
#include "sysdefs.h"

/*----------------------------------------------------------------------------
 Global MON Data
----------------------------------------------------------------------------*/

/* Info passed down from ETS */
extern uint16  *MonVTstDownloadAddr;        
extern uint16  *MonVTstUploadAddr;        
extern uint16  *MonVTstDownloadStartAddr;        
extern uint16  *MonVTstUploadStartAddr;        
extern uint32   MonVTstDownloadLength;  
extern uint32   MonVTstUploadLength;         
extern bool     MonVTstTransmitPacket;
extern bool     MonVTstEnableHw;
extern uint32   MonVTstPacketSize;

/* Test variables */
extern uint32        MonVTstCounter;
extern MonVTstModeT  MonVTstMode;         

/*----------------------------------------------------------------------------
 Global Function Prototypes
----------------------------------------------------------------------------*/
extern void  MonVTstRsp(void);
extern void  MonVTstInit(void);
extern void  MonVTstMsg(void* MsgDataP);
extern void  MonVtstUploadSpeechPacket(uint16* MsgDataP, uint32 MsgSize);




#endif
