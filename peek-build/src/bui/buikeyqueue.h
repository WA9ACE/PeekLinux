

#ifndef UIKEYQUEUE_H
#define UIKEYQUEUE_H

#include "sysdefs.h"

#include "buikeymap.h"
typedef int (*WheelCallbackFuncT)(int count,int direction);
typedef PACKED struct
{
  uint32 Id;
  UiKeypadKeyIdT Value;
} UiKeyT;

void UiKeyQueueInitialize(void);
void UiKeyQueueUnInitialize(void);
bool UiKeyQueuePut(uint32 Id, UiKeypadKeyIdT Value);
bool UiKeyQueueGet(uint32* IdP, UiKeypadKeyIdT* ValueP);
void UiKeyQueueEnable(bool Enable);

void UiWheelTestEnable(bool Enable,WheelCallbackFuncT CallbackFunc);//zhangfanghui add for testing whell count.
bool GetUiWheelTestEnable();//zhangfanghui add.
void SetWheelCount(int step,int direction);//zhangfanghui add.

#endif

