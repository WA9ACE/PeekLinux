

#ifndef UISPECIALKEY_H
#define UISPECIALKEY_H

#include "buikeymap.h"
#include "buiapp.h"

void SetKeyLockState(bool LockState);
bool IsSpecialKey(uint32 KeyCode);
void HandleSpeKey(uint32 KeyMsgId, uint32 KeyCode);
bool UiRegisterSpeKey(uint32 KeyCode, BApplicationC *ApplicationP);
bool UiUnRegisterSpeKey(uint32 KeyCode, BApplicationC *ApplicationP);
DispLCDTypeT LcdTypeOfSpeKey(uint32 KeyCode);





#endif
