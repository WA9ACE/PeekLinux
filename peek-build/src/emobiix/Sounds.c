/* 
 * Sound API for both MIDI/Vibrate
 *
 */

#include "Sounds.h"

void SetSettingValue(const int UIType, const int value)
{
   /*
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
     */
}

void flash_led(void) {
    lcd_led_onoff(1);
    TCCE_Task_Sleep(100);
    lcd_led_onoff(0);
}
void Sounds_PlayMidi(void)
{
    //BalNewTonePlay(BalGetRingToneType(), BalGetVolumnType());
}
 
void Sounds_StartVibrate(unsigned int timeout)
{
     VibratorTimeOut(timeout);
     //BalNewVibratorPlay(BalGetVibrateType());
}

