
#ifndef LIGHT_DRV_IF
#define LIGHT_DRV_IF

#include "sysdefs.h"
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef enum
{
  HWD_LIGHT_KEYPAD,     /* keypad backlight */
  HWD_LIGHT_LCD_MAIN,   /* Main LCD backlight */
  HWD_LIGHT_LCD_SUB,    /* Sub LCD  backlight */
  HWD_LIGHT_RED,
  HWD_LIGHT_GREEN,
  HWD_LIGHT_BLUE,
  HWD_LIGHT_DEV_A,
  HWD_LIGHT_DEV_B,
  
  HWD_LIGHT_MAX
} HwdLightIdT;

typedef enum 
{
  HWD_LIGHT_LEVEL_1,
  HWD_LIGHT_LEVEL_2,
  HWD_LIGHT_LEVEL_3,
  HWD_LIGHT_LEVEL_4,
  HWD_LIGHT_LEVEL_5,
  HWD_LIGHT_LEVEL_6,
  HWD_LIGHT_LEVEL_7,
  HWD_LIGHT_LEVEL_8,
  HWD_LIGHT_LEVEL_9,
  HWD_LIGHT_LEVEL_10,
  HWD_LIGHT_LEVEL_MAX
} HwdLightLevelT;

bool HwdLightOn(HwdLightIdT LightId);
bool HwdLightOff(HwdLightIdT LightId);
bool HwdLightBrightness(HwdLightIdT LightId, HwdLightLevelT LightLevel);
void FlashCtrlTimerHandler(uint32 TimerId);
void HwdLightFlashInit(void);
bool HwdLightFlash(HwdLightIdT FlashId,uint16 FlashTime,bool IsFlashOn,HwdLightLevelT LevelAfterFlash);
bool HwdLightFlashSet(HwdLightIdT FlashId,HwdLightLevelT FlashLightLevel,HwdLightLevelT FlashDimmingLevel,uint16 FlashLightTime,uint16 FlashDimmingTime);
bool HwdGetFlashOnState(HwdLightIdT FlashId);
void LightTimerHandler(uint32 TimerId);
void DimmingTimerHandler(uint32 TimerId);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


