#include "nucleus.h"
#include "typedefs.h"
#include "header.h"

#include "bspTwl3029_Aux.h"
#include "exeapi.h"

ExeTimerT VibrateTimerOut;

void VibratorOn(void) {
        bspTwl3029_Aux_VibEnable(0,1);
}

void VibratorOff(void) {
        bspTwl3029_Aux_VibEnable(0,0);
}

void VibrateTimerOutHandler(uint32 TimerId)
{
        VibratorOff();
}

void VibratorTimeOut(uint32 interval) {

    ExeTimerStop(&VibrateTimerOut);
    ExeTimerDelete(&VibrateTimerOut);

    VibratorOn();
    ExeTimerCreate(&VibrateTimerOut, VibrateTimerOutHandler, 0x71,
	ExeCalMsec(interval), 0);
    ExeTimerStart(&VibrateTimerOut);
}
