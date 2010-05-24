


#ifndef _BAL_LIGHT_H_
#define _BAL_LIGHT_H_

#include "balLightapi.h"


#ifdef __cplusplus
extern "C"
{
#endif
/*user can NOT modify this type define*/
typedef struct 
{
  uint16              FlashMask;         /* bit set to 1 is ON, 0 is OFF */
  uint16              TimeSlot;          /* In millisecond, max value is 60000(1min). last item si 0xFFFF */
} BalLightFlashDataT;

const BalLightFlashDataT   **BalLightFlashDataGet(void);
HwdLightIdT                BalLightIdToHwdId(BalLightIdT LightId);
bool                       valLightRgbGet(void);
uint16                     valLightIsDimmingSup( void );
uint32                     valLightDimmingIntvlGet(void);
#ifdef __cplusplus
}
#endif

#endif 


