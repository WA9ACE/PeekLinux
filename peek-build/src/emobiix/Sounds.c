/* 
 * Sound API for both MIDI/Vibrate
 *
 */

#include "balbattapi.h"
#include "balsndapi.h" 
#include "Sounds.h"
#include "ballightapi.h"

void SetSettingValue(const int UIType, const int value)
{
    switch(UIType)
    {
        case SET_UI_RINGTONE:
            BalSetRingToneType(value);
            break;
        case SET_UI_VOLUME:
            BalSetVolumnType(value);
            break;
        case SET_UI_VIBRATE:
            BalSetVibrateType(value);
            break;
      default:
            break;
     }
}

void flash_led(void) {
//    BalLightFlashOnStatus(BAL_LIGHT_FLASH_BLUE, 5, 5);
//    BalLightFlashOnStatus(BAL_LIGHT_FLASH_GREEN, 5, 5);
    BalLightFlashOnStatus(BAL_LIGHT_FLASH_MSG, 20, 1);

}
void Sounds_PlayMidi(void)
{
    BalNewTonePlay(BalGetRingToneType(), BalGetVolumnType());
}
 
void Sounds_StartVibrate(unsigned int timeout)
{
     BalVibratorTimeOut(timeout);
     BalNewVibratorPlay(BalGetVibrateType());
}

