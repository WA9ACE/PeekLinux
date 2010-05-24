

#ifndef UIS_H
#define UIS_H

#include "sysdefs.h"

typedef enum
{
  UI_S_SCHEDULER_INIT_APPT,
  UI_S_SCHEDULER_PREPARE_APPT
}UiSMsgIdT;

void UiSInitFunc(void);
void UiSMailFunc(uint32 MsgId, void* MsgDataP, uint32 MsgSize);


#endif


