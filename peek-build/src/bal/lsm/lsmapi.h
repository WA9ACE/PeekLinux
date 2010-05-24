


#ifndef LSM_API_H
#define LSM_API_H

/* Everything else goes here */
#include "exedefs.h"
#include "balfsiapi.h"
#include "balmiscapi.h"

#define LSM_APP_MAILBOX	 EXE_MAILBOX_1_ID

typedef PACKED struct
{
  uint16  DbHandle;
  uint8 iTaskId;
  uint8 iMailboxId;
  uint8 iMsgId;
} DbCreateDataMsgT; 

/* lsm app init func */
void LsmAppInit(void);

/* Lsm App Process Msg func */
void LsmAppProcessMsg(uint32 MsgId, void* MsgDataP, uint32 MsgSize);

#endif


